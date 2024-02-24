#include "esp_gap_ble_api.h"


enum
{
    IDX_SVC,

    IDX_CHAR,
    IDX_CHAR_VAL,
    IDX_CHAR_CFG,

    HRS_IDX_NB,

};

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t ble_init(void (*write_cb)(char* data, uint16_t len));

esp_err_t send_data(uint8_t *data, uint16_t len);

esp_err_t send_string(const char *str);

#ifdef __cplusplus
}
#endif
