#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "espnow.h"
#include "espnow_storage.h"
#include "espnow_utils.h"


static const char *TAG = "app_main";

static void (*write_callback)(char*, uint16_t, uint8_t*) = NULL;

uint8_t broadcast_addr[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static void app_wifi_init()
{
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static esp_err_t write_handle(uint8_t *src_addr, void *data,
                                       size_t size, wifi_pkt_rx_ctrl_t *rx_ctrl)
{

    ESP_PARAM_CHECK(src_addr);
    ESP_PARAM_CHECK(data);
    ESP_PARAM_CHECK(size);
    ESP_PARAM_CHECK(rx_ctrl);

    if (write_callback)
    {
        write_callback((char *)data, size, src_addr);
    }

    return ESP_OK;
}

void radio_broadcast(const char *data, uint16_t len) {
    esp_err_t ret = espnow_send(ESPNOW_DATA_TYPE_DATA, broadcast_addr, data, len, NULL, portMAX_DELAY);
    ESP_ERROR_CHECK(ret);
}

void radio_send(const char *data, uint16_t len, uint8_t *addr) {
    esp_err_t ret = espnow_send(ESPNOW_DATA_TYPE_DATA, addr, data, len, NULL, portMAX_DELAY);
    ESP_ERROR_CHECK(ret);
}

void radio_init(void (*write_cb)(char*, uint16_t, uint8_t*))
{

    write_callback = write_cb;

    espnow_storage_init();

    app_wifi_init();

    espnow_config_t espnow_config = ESPNOW_INIT_CONFIG_DEFAULT();
    espnow_init(&espnow_config);

    espnow_set_config_for_data_type(ESPNOW_DATA_TYPE_DATA, true, write_handle);

}
