/*
 * app_bt_hci.c
 *
 *  Created on: 6 de jul de 2017
 *      Author: msam
 */
#include <stdio.h>
#include <string.h>

#include "bt.h"
#include "app_bt_hci.h"
#include "esp_gap_ble_api.h"

uint8_t hci_cmd_buf[128];
typedef uint8_t bd_addr_t[BD_ADDR_LEN];         /* Device address */

static uint16_t make_cmd_reset(uint8_t *buf)
{
    UINT8_TO_STREAM (buf, H4_TYPE_COMMAND);
    UINT16_TO_STREAM (buf, HCI_RESET);
    UINT8_TO_STREAM (buf, 0);
    return HCI_H4_CMD_PREAMBLE_SIZE;
}

static uint16_t make_cmd_ble_set_adv_enable (uint8_t *buf, uint8_t adv_enable)
{
    UINT8_TO_STREAM (buf, H4_TYPE_COMMAND);
    UINT16_TO_STREAM (buf, HCI_BLE_WRITE_ADV_ENABLE);
    UINT8_TO_STREAM  (buf, HCIC_PARAM_SIZE_WRITE_ADV_ENABLE);
    UINT8_TO_STREAM (buf, adv_enable);
    return HCI_H4_CMD_PREAMBLE_SIZE + HCIC_PARAM_SIZE_WRITE_ADV_ENABLE;
}

static uint16_t make_cmd_ble_set_adv_param (uint8_t *buf, uint16_t adv_int_min, uint16_t adv_int_max,
                                            uint8_t adv_type, uint8_t addr_type_own,
                                            uint8_t addr_type_dir, bd_addr_t direct_bda,
                                            uint8_t channel_map, uint8_t adv_filter_policy)
{
    UINT8_TO_STREAM (buf, H4_TYPE_COMMAND);
    UINT16_TO_STREAM (buf, HCI_BLE_WRITE_ADV_PARAMS);
    UINT8_TO_STREAM  (buf, HCIC_PARAM_SIZE_BLE_WRITE_ADV_PARAMS );

    UINT16_TO_STREAM (buf, adv_int_min);
    UINT16_TO_STREAM (buf, adv_int_max);
    UINT8_TO_STREAM (buf, adv_type);
    UINT8_TO_STREAM (buf, addr_type_own);
    UINT8_TO_STREAM (buf, addr_type_dir);
    BDADDR_TO_STREAM (buf, direct_bda);
    UINT8_TO_STREAM (buf, channel_map);
    UINT8_TO_STREAM (buf, adv_filter_policy);
    return HCI_H4_CMD_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_WRITE_ADV_PARAMS;
}

static uint16_t make_cmd_ble_set_adv_data(uint8_t *buf, uint8_t data_len, uint8_t *p_data)
{
    UINT8_TO_STREAM (buf, H4_TYPE_COMMAND);
    UINT16_TO_STREAM (buf, HCI_BLE_WRITE_ADV_DATA);
    UINT8_TO_STREAM  (buf, HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA + 1);

    memset(buf, 0, HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA);

    if (p_data != NULL && data_len > 0) {
        if (data_len > HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA) {
            data_len = HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA;
        }

        UINT8_TO_STREAM (buf, data_len);

        ARRAY_TO_STREAM (buf, p_data, data_len);
    }
    return HCI_H4_CMD_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA + 1;
}

void hci_cmd_send_eddystone ( uint8_t eddystoneLen, uint8_t*eddystoneData)
{
//uint16_t sz = make_cmd_ble_set_adv_data(hci_cmd_buf, sizeof(eddystoneAdvData_t), (uint8_t *)eddystoneAdvData);
	uint16_t sz = make_cmd_ble_set_adv_data(hci_cmd_buf, eddystoneLen,eddystoneData );
	esp_vhci_host_send_packet(hci_cmd_buf, sz);

}

void hci_cmd_send_ble_set_adv_data(void)
{
    uint8_t adv_data[31];
    uint8_t adv_data_len;

    adv_data[0] = 2;      // Len
    adv_data[1] = 0x01;   // Type Flags
    adv_data[2] = 0x06;   // GENERAL_DISC_MODE 0x02 | BR_EDR_NOT_SUPPORTED 0x04
    adv_data[3] = 3;      // Len
    adv_data[4] = 0x03;   // Type 16-Bit UUID
    adv_data[5] = 0xAA;   // Eddystone UUID 2 -> 0xFEAA LSB
    adv_data[6] = 0xFE;   // Eddystone UUID 1 MSB
    adv_data[7] = 19;     // Length of Beacon Data
    adv_data[8] = 0x16;   // Type Service Data
    adv_data[9] = 0xAA;   // Eddystone UUID 2 -> 0xFEAA LSB
    adv_data[10] = 0xFE;  // Eddystone UUID 1 MSB
    adv_data[11] = 0x10;  // Eddystone Frame Type
    adv_data[12] = 0x20;  // Beacons TX power at 0m
    adv_data[13] = 0x03;  // URL Scheme 'https://'
    adv_data[14] = 0x67;  // URL add  1 'g'
    adv_data[15] = 0x6F;  // URL add  2 'o'
    adv_data[16] = 0x6F;  // URL add  3 'o'
    adv_data[17] = 0x2E;  // URL add  4 '.'
    adv_data[18] = 0x67;  // URL add  5 'g'
    adv_data[19] = 0x6C;  // URL add  6 'l'
    adv_data[20] = 0x2F;  // URL add  7 '/'
    adv_data[21] = 0x32;  // URL add  8 '2'
    adv_data[22] = 0x79;  // URL add  9 'y'
    adv_data[23] = 0x43;  // URL add 10 'C'
    adv_data[24] = 0x36;  // URL add 11 '6'
    adv_data[25] = 0x4B;  // URL add 12 'K'
    adv_data[26] = 0x58;  // URL add 13 'X'

    adv_data_len = 27;

    printf("Eddystone adv_data [%d]=",adv_data_len);
    for (int i=0; i<adv_data_len; i++) {
        printf("%02x",adv_data[i]);
    }
    printf("\n");


    uint16_t sz = make_cmd_ble_set_adv_data(hci_cmd_buf, adv_data_len, (uint8_t *)adv_data);
    esp_vhci_host_send_packet(hci_cmd_buf, sz);
}

void hci_cmd_send_ble_set_adv_param(void)
{
    uint16_t adv_intv_min = DEFAULT_ADVERTISING_INTERVAL; //256; // 160ms
    uint16_t adv_intv_max = DEFAULT_ADVERTISING_INTERVAL;  //256; // 160ms
    uint8_t adv_type = 0; // connectable undirected advertising (ADV_IND)
    uint8_t own_addr_type = 0; // Public Device Address
    uint8_t peer_addr_type = 0; // Public Device Address
    uint8_t peer_addr[6] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85};
    uint8_t adv_chn_map = 0x07; // 37, 38, 39
    uint8_t adv_filter_policy = 0; // Process All Conn and Scan

    uint16_t sz = make_cmd_ble_set_adv_param(hci_cmd_buf,
                                             adv_intv_min,
                                             adv_intv_max,
                                             adv_type,
                                             own_addr_type,
                                             peer_addr_type,
                                             peer_addr,
                                             adv_chn_map,
                                             adv_filter_policy);
    esp_vhci_host_send_packet(hci_cmd_buf, sz);
}

void hci_cmd_send_reset(void)
{
    uint16_t sz = make_cmd_reset (hci_cmd_buf);
    esp_vhci_host_send_packet(hci_cmd_buf, sz);
}

void hci_cmd_send_ble_adv_start(void)
{
    uint16_t sz = make_cmd_ble_set_adv_enable (hci_cmd_buf, 1);
    esp_vhci_host_send_packet(hci_cmd_buf, sz);
}


