#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#define LED_BUILTIN 2

void app_main() {

    // Set the GPIO as a push/pull output
    gpio_config_t io_conf;
    io_conf.pin_bit_mask = (1ULL << LED_BUILTIN);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    // Blink the LED
    while (true) {
        gpio_set_level(LED_BUILTIN, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(LED_BUILTIN, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}