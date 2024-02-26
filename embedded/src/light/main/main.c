#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <driver/gpio.h>

#include "freertos/FreeRTOS.h"

#include "radio.h"
#include "flags.h"

#define LED_PIN 2


void write_cb(char* data, uint16_t len, uint8_t* src_addr) {

    if (len == 0) {
        return;
    }

    // If data contains TILE_REQUEST_FLAG
    if (strstr(data, TILE_COMMAND_FLAG) != NULL && strlen(data) > strlen(TILE_COMMAND_FLAG) + 1) {
        // Data should be of format TILE_COMMAND_FLAG + {1 or 0} + TILE_COMMAND_FLAG
        uint8_t sink_bool = (uint8_t)atoi(data + strlen(TILE_COMMAND_FLAG));
        // Set the LED to the value of sink_bool
        gpio_set_level(LED_PIN, sink_bool);
    }


    // If data contains QUERY_FLAG
    else if (strstr(data, QUERY_FLAG) != NULL) {
        // Send a targeted message containing the tile type
        const char* type = tile_type_to_string(SINK_BOOL);
        // Wrap in IDENTIFY_FLAG
        char response[32];
        snprintf(response, sizeof(response), "%s%s%s", IDENTIFY_FLAG, type, IDENTIFY_FLAG);
        
        // Repeat sending the message 3 times to ensure it is received
        for(int i = 0; i < 3; i++) {
            // Wait for a random amount of time--at most 100ms
            // This is to prevent collisions with other tiles, and if one does occur, the message will be resent
            vTaskDelay((rand() % 100) / portTICK_PERIOD_MS);
            radio_send(response, strlen(response), src_addr);
        }
    }
}

void app_main(void) {

    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_PIN);
    gpio_config(&io_conf);

    radio_init(write_cb);

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}