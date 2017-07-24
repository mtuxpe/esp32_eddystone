/*
 * app_bt_eddystone.c
 *
 *  Created on: 6 de jul de 2017
 *      Author: msam
 */

#include <stdio.h>
#include <string.h>
//#include "xos_types.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"


#include "bt.h"
#include "esp_gap_ble_api.h"
#include "app_bt_hci.h"

//#define BD_ADDR_LEN     (6)                     /* Device address length */
//typedef uint8_t bd_addr_t[BD_ADDR_LEN];         /* Device address */
//typedef uint8_t bd_addr_t[BD_ADDR_LEN];         /* Device address */


// ESP_BLE_ADV_DATA_LEN_MAX 31
// ver esp_gap_ble_api.h

#define EDDYSTONE_FRAME_TYPE_UID                0x00
#define EDDYSTONE_FRAME_TYPE_URL                0x10
#define EDDYSTONE_FRAME_TYPE_TLM                0x20

#define EDDYSTONE_FRAME_OVERHEAD_LEN            8
#define EDDYSTONE_SVC_DATA_OVERHEAD_LEN         3
#define EDDYSTONE_MAX_URL_LEN                   18

// # of URL Scheme Prefix types
#define EDDYSTONE_URL_PREFIX_MAX        4
// # of encodable URL words
#define EDDYSTONE_URL_ENCODING_MAX      14

#define 	HI_UINT16(a)   (((a) >> 8) & 0xFF)
#define 	LO_UINT16(a)   ((a) & 0xFF)


const uint8_t namespace[10] = { 0x9,0x91,0x58,0x2f,0xa8,0xca,0x21,0x02,0x6a,0x6a };
const uint8_t instance[6]  = { 1,2,3,4,5,6 };

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
  // complete name
  0x16,   // length of this data
  ESP_BLE_AD_TYPE_NAME_CMPL,
  'S',
  'i',
  'm',
  'p',
  'l',
  'e',
  'E',
  'd',
  'd',
  'y',
  's',
  't',
  'o',
  'n',
  'e',
  'B',
  'e',
  'a',
  'c',
  'o',
  'n',

  // connection interval range
  0x05,   // length of this data
  ESP_BLE_AD_TYPE_INT_RANGE,
  LO_UINT16(DEFAULT_MIN_CONN_INTERVAL),   // 100ms
  HI_UINT16(DEFAULT_MIN_CONN_INTERVAL),
  LO_UINT16(DEFAULT_MAX_CONN_INTERVAL),   // 1s
  HI_UINT16(DEFAULT_MAX_CONN_INTERVAL),
};

// Eddystone UID frame
typedef struct
{
  uint8_t   frameType;      // UID
  int8_t    txpower;
  uint8_t   namespaceID[10];
  uint8_t   instanceID[6];
  uint8_t   reserved[2];
} eddystoneUID_t;

// Eddystone URL frame
typedef struct
{
  uint8_t   frameType;      // URL | Flags
  int8_t    txPower;
  uint8_t   encodedURL[EDDYSTONE_MAX_URL_LEN];  // the 1st byte is prefix
} eddystoneURL_t;

// Eddystone TLM frame
typedef struct
{
  uint8_t   frameType;      // TLM
  uint8_t   version;        // 0x00 for now
  uint8_t   vBatt[2];       // Battery Voltage, 1mV/bit, Big Endian
  uint8_t   temp[2];        // Temperature. Signed 8.8 fixed point
  uint8_t   advCnt[4];      // Adv count since power-up/reboot
  uint8_t   secCnt[4];      // Time since power-up/reboot
                          // in 0.1 second resolution
} eddystoneTLM_t;

typedef struct
{
  uint8_t               length1;        // 2
  uint8_t               flags1;      // for Flags data type (0x01)
  uint8_t               data1;          // for Flags data (0x04)
  uint8_t               length2;        // 3
  uint8_t               data2type;      // for 16-bit Svc UUID list data type (0x03)
  uint8_t               data2lsb;          // for Eddystone UUID LSB (0xAA)
  uint8_t               data2msb;          // for Eddystone UUID MSB (0xFE)
  uint8_t               length3;         // Eddystone service data length
  uint8_t               service3;      // for Svc Data data type (0x16)
  uint8_t               data3lsb;          // for Eddystone UUID LSB (0xAA)
  uint8_t               data3msb;          // for Eddystone UUID MSB (0xFE)
  eddystoneUID_t        frameUID;
} eddystoneAdvData_t;



void sendEddystoneUID ( )
{
	eddystoneAdvData_t eddystoneAdvData;

	eddystoneAdvData.length1 = 2;      // len
	eddystoneAdvData.flags1 =  ESP_BLE_AD_TYPE_FLAG ;   // Type Flags
	eddystoneAdvData.data1 = 0x06;   // GENERAL_DISC_MODE 0x02 | BR_EDR_NOT_SUPPORTED 0x04

	eddystoneAdvData.length2= 3;      // len
	eddystoneAdvData.data2type=  ESP_BLE_AD_TYPE_16SRV_CMPL;   // Type Flags
	eddystoneAdvData.data2lsb = 0xaa;
	eddystoneAdvData.data2msb = 0xfe;

	eddystoneAdvData.length3 = sizeof(eddystoneUID_t )+ 3;
	eddystoneAdvData.service3 = ESP_BLE_AD_TYPE_SERVICE_DATA;
	eddystoneAdvData.data3lsb= 0xaa;
	eddystoneAdvData.data3msb = 0xfe;

	eddystoneAdvData.frameUID.frameType = EDDYSTONE_FRAME_TYPE_UID;
	eddystoneAdvData.frameUID.txpower = 0xce;
	eddystoneAdvData.frameUID.reserved[0] = 0;
	eddystoneAdvData.frameUID.reserved[1] = 0;
	memcpy(eddystoneAdvData.frameUID.namespaceID,namespace,10);
	memcpy(eddystoneAdvData.frameUID.instanceID,instance,6);

    uint8_t adv_data_len = sizeof(eddystoneAdvData_t);

//    printf("Eddystone adv_data [%d]=",adv_data_len);
//    for (int i=0; i<adv_data_len; i++) {
//        printf("%02x",adv_data[i]);
//    }
//    printf("\n");
	hci_cmd_send_eddystone ( adv_data_len, (uint8_t*)&eddystoneAdvData);

//    uint16_t sz = make_cmd_ble_set_adv_data(hci_cmd_buf, sizeof(eddystoneAdvData_t), (uint8_t *)eddystoneAdvData);
//    esp_vhci_host_send_packet(hci_cmd_buf, sz);
}

