#include "esp_gap_ble_api.h"


enum
{
    IDX_SVC,

    // Write (from esp32 to client i.e. webapp) 
    IDX_CHAR,
    IDX_CHAR_VAL,
    IDX_CHAR_CFG,

    HRS_IDX_NB,

};


esp_err_t ble_init(void (*write_cb)(char* data, uint16_t len));

esp_err_t send_data(uint8_t *data, uint16_t len);

esp_err_t send_string(const char *str);
