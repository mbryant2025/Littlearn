#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

// #include "ble.h"
#include "radio.h"

// Callback for when a client writes to the characteristic
void write_cb(char* data, uint16_t len) {
    printf("Received data: %s\n", data);
}

void app_main(void) {

    // ble_init(write_cb);

    radio_init();

    while(1) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        
        // send_string("__PRINT__Hello, BLE!__PRINT__");

    }
}
