#include "esp_gap_ble_api.h"


enum
{
    IDX_SVC,

    // Write (from esp32 to client i.e. webapp) 
    IDX_CHAR_WRITE,
    IDX_CHAR_VAL_WRITE,
    IDX_CHAR_CFG_WRITE,

    HRS_IDX_NB,

};


esp_err_t ble_init(void);

esp_err_t send_data(uint8_t *data, uint16_t len);

esp_err_t send_string(const char *str);
