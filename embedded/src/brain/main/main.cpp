#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <lwip/sockets.h>

const char *WIFI_SSID = "ESP32-Access-Point";
const char *WIFI_PASSWORD = "123456789";
size_t ssid_length = strlen(WIFI_SSID);
size_t password_length = strlen(WIFI_PASSWORD);

// TCP server parameters
#define PORT 12345
#define MAX_CONNECTIONS 5

void wifi_init_softap() {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_wifi_set_mode(WIFI_MODE_AP);

    wifi_config_t ap_config = {};
    memcpy(ap_config.ap.ssid, WIFI_SSID, ssid_length);
    memcpy(ap_config.ap.password, WIFI_PASSWORD, password_length);
    ap_config.ap.ssid_len = strlen(WIFI_SSID);
    ap_config.ap.max_connection = MAX_CONNECTIONS;
    ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();
}

void tcp_server_task(void *pvParameters) {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, MAX_CONNECTIONS);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);

        // Handle data from the client
        char buffer[256];
        int recv_len = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            printf("Received data from client: %s\n", buffer);
            // Handle the received data as needed
        }

        // Close the client socket
        close(client_socket);
    }
}

extern "C" void app_main() {
    nvs_flash_init();

    wifi_init_softap();

    xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

