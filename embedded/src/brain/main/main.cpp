#include <cstring>
#include <mutex>
#include <iomanip>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "ble.h"
#include "radio.h"

#include "tile_types.h"
#include "flags.h"

#include "ast.hpp"
#include "error.hpp"
#include "interpreter.hpp"
#include "outputStream.hpp"
#include "tokenizer.hpp"


std::mutex script_mutex;
std::mutex tile_mutex;

std::string script = "{print(420);}";  // Will be saved script later

// Map of mac address to what kind of sensor it is
struct MACAddressComparator {
    bool operator()(const uint8_t* lhs, const uint8_t* rhs) const {
        return std::memcmp(lhs, rhs, 6) < 0;
    }
};
std::map<const uint8_t*, TileType, MACAddressComparator> tile_map;

std::string get_script() {
    std::lock_guard<std::mutex> lock(script_mutex);
    return script;
}

void set_script(const std::string &s) {
    std::lock_guard<std::mutex> lock(script_mutex);
    script = s;
}

class BLEOutputStream : public OutputStream {
   public:
    void write(const std::string &message) override {
        send_string(message.c_str());
    }
};

BLEOutputStream outputStream;
ErrorHandler errorHandler(outputStream);

// Callback for when a client writes to the characteristic
void ble_write_cb(char* data, uint16_t len) {
    if(data && len > 2 * strlen(SEND_SCRIPT_FLAG)) {
        printf("Received data: %s\n", data);
        errorHandler.triggerStopExecution();

        set_script(std::string(data + strlen(SEND_SCRIPT_FLAG), len - strlen(SEND_SCRIPT_FLAG) * 2));

        errorHandler.resetStopExecution();

        send_string(SENT_SCRIPT_FLAG);
    }    
}

void radio_write_cb(char* data, uint16_t len, uint8_t* src_addr) {
    printf("From: %02X:%02X:%02X:%02X:%02X:%02X\n", src_addr[0], src_addr[1], src_addr[2], src_addr[3], src_addr[4], src_addr[5]);
    printf("Data: %.*s\n", len, data);


    // If data contains IDENTIFY_FLAG
    if (strstr(data, IDENTIFY_FLAG) != NULL) {
        // Get the type of sensor (sandwiched between two IDENTIFY_FLAGs)
        char* type = strstr(data, IDENTIFY_FLAG) + strlen(IDENTIFY_FLAG);
        char* end = strstr(type, IDENTIFY_FLAG);
        *end = '\0';

        printf("Type: %s\n", type);

        std::lock_guard<std::mutex> lock(tile_mutex);
        tile_map[src_addr] = string_to_tile_type(type);

        //print the number of tiles
        printf("Number of tiles: %d\n", tile_map.size());

        //Print the map
        for (auto const& x : tile_map)
        {
            printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X, Type: %s\n", x.first[0], x.first[1], x.first[2], x.first[3], x.first[4], x.first[5], tile_type_to_string(x.second));
        }
    }

}


void query_tiles(void) {

    radio_broadcast(QUERY_FLAG, strlen(QUERY_FLAG));
}

extern "C" void app_main(void) {

    ble_init(ble_write_cb);

    radio_init(radio_write_cb);

    query_tiles();

    while(1) {

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        
        Tokenizer tokenizer(get_script());

        const std::vector<Token> tokens = tokenizer.tokenize();

        // Create a Parser object
        Parser parser(tokens, outputStream, errorHandler);

        BlockNode* block = parser.parseProgram();

        // Create an Interpreter object
        if (block != nullptr) {

            Interpreter interpreter(*block, outputStream, errorHandler);

            // Interpret the AST
            interpreter.interpret();

            delete block;
        }

    }
}
