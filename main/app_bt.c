#include <stdio.h>
#include <string.h>
//#include "xos_types.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

//#include "esp_wifi.h"
//#include "esp_wifi_types.h"

#include "bt.h"
#include "app_bt_hci.h"

#include "sys/time.h"
#include "sdkconfig.h"
#include "esp_deep_sleep.h"
#include "nvs_flash.h"

extern void sendEddystoneUID();
/*
//#define GATTS_TAG "MAIN"
//
//#define HCI_H4_CMD_PREAMBLE_SIZE           (4)
//
//   HCI Command opcode group field(OGF)
// #define HCI_GRP_HOST_CONT_BASEBAND_CMDS    (0x03 << 10)             0x0C00
//#define HCI_GRP_BLE_CMDS                   (0x08 << 10)
//
//#define HCI_RESET                          (0x0003 | HCI_GRP_HOST_CONT_BASEBAND_CMDS)
//#define HCI_BLE_WRITE_ADV_ENABLE           (0x000A | HCI_GRP_BLE_CMDS)
//#define HCI_BLE_WRITE_ADV_PARAMS           (0x0006 | HCI_GRP_BLE_CMDS)
//#define HCI_BLE_WRITE_ADV_DATA             (0x0008 | HCI_GRP_BLE_CMDS)
//
//#define HCIC_PARAM_SIZE_WRITE_ADV_ENABLE        (1)
//#define HCIC_PARAM_SIZE_BLE_WRITE_ADV_PARAMS    (15)
//#define HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA      (31)
//

//#define UINT16_TO_STREAM(p, u16) {*(p)++ = (uint8_t)(u16); *(p)++ = (uint8_t)((u16) >> 8);}
//#define UINT8_TO_STREAM(p, u8)   {*(p)++ = (uint8_t)(u8);}
//#define BDADDR_TO_STREAM(p, a)   {int ijk; for (ijk = 0; ijk < BD_ADDR_LEN;  ijk++) *(p)++ = (uint8_t) a[BD_ADDR_LEN - 1 - ijk];}
//#define ARRAY_TO_STREAM(p, a, len) {int ijk; for (ijk = 0; ijk < len;        ijk++) *(p)++ = (uint8_t) a[ijk];}

//enum {
//    H4_TYPE_COMMAND = 1,
//    H4_TYPE_ACL     = 2,
//    H4_TYPE_SCO     = 3,
//    H4_TYPE_EVENT   = 4
//};

*/
/* 
 * @brief: BT controller callback function, used to notify the upper layer that
 *         controller is ready to receive command
 */
static void controller_rcv_pkt_ready(void)
{
    printf("controller rcv pkt ready\n");
}

/* 
 * @brief: BT controller callback function, to transfer data packet to upper
 *         controller is ready to receive command
 */
static int host_rcv_pkt(uint8_t *data, uint16_t len)
{
    printf("host rcv pkt: ");
    for (uint16_t i=0; i<len; i++)
        printf("%02x", data[i]);
    printf("\n");
    return 0;
}

static esp_vhci_host_callback_t vhci_host_cb = {
    controller_rcv_pkt_ready,
    host_rcv_pkt
};

/*
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

uint16_t make_cmd_ble_set_adv_param (uint8_t *buf, uint16_t adv_int_min, uint16_t adv_int_max,
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

*/

/*
static void hci_cmd_send_reset(void)
{
    uint16_t sz = make_cmd_reset (hci_cmd_buf);
    esp_vhci_host_send_packet(hci_cmd_buf, sz);
}

static void hci_cmd_send_ble_adv_start(void)
{
    uint16_t sz = make_cmd_ble_set_adv_enable (hci_cmd_buf, 1);
    esp_vhci_host_send_packet(hci_cmd_buf, sz);
 }
*/
/*
static void hci_cmd_send_ble_set_adv_param(void)
{
    uint16_t adv_intv_min = 256; // 160ms
    uint16_t adv_intv_max = 256; // 160ms
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
*/
//ble_eddystone_set_adv_data_uid

static void deep_sleep_task(void* arg)
{
    esp_deep_sleep_start();
}


/*
 * @brief: send HCI commands to perform BLE advertising;
 */
void bleAdvtTask(void *pvParameters)
{
    int cmd_cnt = 0;
    bool send_avail = false;
    esp_vhci_host_register_callback(&vhci_host_cb);
    printf("BLE advt task start\n");
    while (1) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        send_avail = esp_vhci_host_check_send_available();
        if (send_avail) {
            switch (cmd_cnt) {
            case 0: hci_cmd_send_reset(); ++cmd_cnt; break;
            case 1: hci_cmd_send_ble_set_adv_param(); ++cmd_cnt; break;
 //           case 2: hci_cmd_send_ble_set_adv_data(); ++cmd_cnt; break;
            case 2: sendEddystoneUID(); ++cmd_cnt; break;
            case 3: hci_cmd_send_ble_adv_start(); ++cmd_cnt; break;
            }
        }
        printf("BLE Advertise, flag_send_avail: %d, cmd_sent: %d\n", send_avail, cmd_cnt);
    }
}

int app_main() {
    esp_err_t ret;


    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s initialize controller failed\n", __func__);
        return 1;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    if (ret) {
        ESP_LOGE(GATTS_TAG, "%s enable controller failed\n", __func__);
        return 1;
    }

    xTaskCreatePinnedToCore(&bleAdvtTask, "bleAdvtTask", 2048, NULL, 5, NULL, 0);
    ret = esp_deep_sleep_pd_config( ESP_PD_DOMAIN_RTC_PERIPH,ESP_PD_OPTION_AUTO);
    if ( ret )
    {
    	 ESP_LOGE(GATTS_TAG, "%s deep sleep config failed\n", __func__);
    }
    ret = esp_deep_sleep_enable_timer_wakeup(1000000UL);
    if ( ret )
    {
    	 ESP_LOGE(GATTS_TAG, "%s deep sleep enable failed\n", __func__);
    }
 //   xTaskCreatePinnedToCore(&deep_sleep_task, "ds", 2048, NULL, 5, NULL, 1);
 //   vTaskSuspendAll();
 //   while(true) {

 //   }
 //   esp_deep_sleep_start();
    return 0;
}

