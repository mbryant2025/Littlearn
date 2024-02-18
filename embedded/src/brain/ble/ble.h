#include "esp_gap_ble_api.h"

#define ESP_APP_ID 0x55
#define GATTS_TABLE_TAG "GATTS_TABLE_DEMO"

enum
{
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,

    IDX_CHAR_B,
    IDX_CHAR_VAL_B,

    IDX_CHAR_C,
    IDX_CHAR_VAL_C,

    HRS_IDX_NB,
};


esp_err_t ble_init(void);

esp_err_t send_data(uint8_t *data, uint16_t len);

esp_err_t send_string(const char *str);
