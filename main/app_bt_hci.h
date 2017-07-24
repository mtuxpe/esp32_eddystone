/*
 * app_bt_hci.h
 *
 *  Created on: 6 de jul de 2017
 *      Author: msam
 */

#ifndef APP_BT_HCI_H_
#define APP_BT_HCI_H_

#define GATTS_TAG "MAIN"

#define HCI_H4_CMD_PREAMBLE_SIZE           (4)

/*  HCI Command opcode group field(OGF) */
#define HCI_GRP_HOST_CONT_BASEBAND_CMDS    (0x03 << 10)            /* 0x0C00 */
#define HCI_GRP_BLE_CMDS                   (0x08 << 10)

#define HCI_RESET                          (0x0003 | HCI_GRP_HOST_CONT_BASEBAND_CMDS)
#define HCI_BLE_WRITE_ADV_ENABLE           (0x000A | HCI_GRP_BLE_CMDS)
#define HCI_BLE_WRITE_ADV_PARAMS           (0x0006 | HCI_GRP_BLE_CMDS)
#define HCI_BLE_WRITE_ADV_DATA             (0x0008 | HCI_GRP_BLE_CMDS)

#define HCIC_PARAM_SIZE_WRITE_ADV_ENABLE        (1)
#define HCIC_PARAM_SIZE_BLE_WRITE_ADV_PARAMS    (15)
#define HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA      (31)

//#define BD_ADDR_LEN     (6)                     /* Device address length */
//typedef uint8_t bd_addr_t[BD_ADDR_LEN];         /* Device address */

#define UINT16_TO_STREAM(p, u16) {*(p)++ = (uint8_t)(u16); *(p)++ = (uint8_t)((u16) >> 8);}
#define UINT8_TO_STREAM(p, u8)   {*(p)++ = (uint8_t)(u8);}
#define BDADDR_TO_STREAM(p, a)   {int ijk; for (ijk = 0; ijk < BD_ADDR_LEN;  ijk++) *(p)++ = (uint8_t) a[BD_ADDR_LEN - 1 - ijk];}
#define ARRAY_TO_STREAM(p, a, len) {int ijk; for (ijk = 0; ijk < len;        ijk++) *(p)++ = (uint8_t) a[ijk];}

enum {
    H4_TYPE_COMMAND = 1,
    H4_TYPE_ACL     = 2,
    H4_TYPE_SCO     = 3,
    H4_TYPE_EVENT   = 4
};

#define BD_ADDR_LEN     (6)                     /* Device address length */
#define DEFAULT_MIN_CONN_INTERVAL     80

// Maximum connection interval (units of 1.25ms, 800=1000ms)
#define DEFAULT_MAX_CONN_INTERVAL     800

// Slave latency to use
#define DEFAULT_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s)
#define DEFAULT_CONN_TIMEOUT          1000

// this is the advertising interval  (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160

void hci_cmd_send_reset(void);
void hci_cmd_send_ble_adv_start(void);
void hci_cmd_send_ble_set_adv_param(void);
void hci_cmd_send_ble_set_adv_data(void);
void hci_cmd_send_eddystone ( uint8_t eddystoneLen, uint8_t*eddystoneData);
#endif /* APP_BT_HCI_H_ */
