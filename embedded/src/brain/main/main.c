#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "ble.h"

void app_main(void) {

    ble_init();

    while(1) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        
        send_string("__PRINT__Hello, BLE!__PRINT__");

    }
}
