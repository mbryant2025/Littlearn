#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "radio.h"

#include "flags.h"


void write_cb(char* data, uint16_t len, uint8_t* src_addr) {
    printf("From: %02X:%02X:%02X:%02X:%02X:%02X\n", src_addr[0], src_addr[1], src_addr[2], src_addr[3], src_addr[4], src_addr[5]);
    printf("Data: %.*s\n", len, data);

    // If data contains QUERY_FLAG
    if (strstr(data, QUERY_FLAG) != NULL) {
        // Send a targeted message containing the tile type
        const char* type = tile_type_to_string(SINK_BOOL);
        // Wrap in IDENTIFY_FLAG
        char response[32];
        snprintf(response, sizeof(response), "%s%s%s", IDENTIFY_FLAG, type, IDENTIFY_FLAG);
        radio_send(response, strlen(response), src_addr);
    }
}

void app_main(void) {
    radio_init(write_cb);

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}