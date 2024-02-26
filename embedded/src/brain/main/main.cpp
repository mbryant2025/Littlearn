#include <cstring>
#include <iomanip>
#include <mutex>

#include "ast.hpp"
#include "ble.h"
#include "error.hpp"
#include "esp_system.h"
#include "flags.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

#include "interpreter.hpp"
#include "outputStream.hpp"
#include "radio.h"
#include "radioFormatter.hpp"
#include "tile_types.h"
#include "tokenizer.hpp"

std::mutex script_mutex;
std::mutex tile_mutex;
std::mutex interpreter_mutex;

std::string script = "{for(int i = 0; i < 10; i=i+1) {print(i); wait(1000); send_bool(0, 1); wait(1000); send_bool(0, 0);}}";

// Map of mac address to what kind of sensor it is
struct MACAddressComparator {
    bool operator()(const std::array<uint8_t, 6>& lhs, const std::array<uint8_t, 6>& rhs) const {
        return std::memcmp(lhs.data(), rhs.data(), 6) < 0;
    }
};

std::map<std::array<uint8_t, 6>, TileType, MACAddressComparator> tile_map;

uint8_t* get_mac_for_type(TileType type, int index) {
    int i = 0;
    for (auto const& x : tile_map) {
        if (x.second == type) {
            if (i == index) {
                return const_cast<uint8_t*>(x.first.data());
            }
            i++;
        }
    }
    return nullptr;
}

void RadioFormatter::send_bool(int tile_idx, bool value) {
    char data[32];
    sprintf(data, "%s%d%s", TILE_COMMAND_FLAG, value, TILE_COMMAND_FLAG);
    uint8_t* addr = get_mac_for_type(SINK_BOOL, tile_idx);
    if (addr != nullptr) {
        radio_send(data, strlen(data), addr);
    }
}

std::string get_script() {
    std::lock_guard<std::mutex> lock(script_mutex);
    return script;
}

class BLEOutputStream : public OutputStream {
   public:
    void write(const std::string& message) override {
        send_string(message.c_str());
    }
};

BLEOutputStream outputStream;
ErrorHandler errorHandler(outputStream);
Interpreter* interpreter = nullptr;
BlockNode* block = nullptr;
RadioFormatter radioFormatter;

void generate_ast() {

    errorHandler.resetStopExecution();

    // Problem because we need to not have execution stopped to not throw error from the ast
    // however, we do want to stop execution if we are reuploading code

    std::lock_guard<std::mutex> lock(interpreter_mutex);

    if (interpreter != nullptr) {
        delete interpreter;
        interpreter = nullptr;
    }

    if (block != nullptr) {
        delete block;
        block = nullptr;
    }

    Tokenizer tokenizer(get_script());

    const std::vector<Token> tokens = tokenizer.tokenize();

    if (tokens.empty()) {
        return;
    }

    // Create a Parser object
    Parser parser(tokens, outputStream, errorHandler);

    // Parse the source code
    block = parser.parseProgram();

    if (block == nullptr) {
        return;
    }

    // Create an Interpreter object
    interpreter = new Interpreter(*block, outputStream, errorHandler, radioFormatter);
}

void set_script(const std::string& s) {
    {
        std::lock_guard<std::mutex> lock(script_mutex);
        printf("Setting script to: %s\n", s.c_str());
        script = s;
    }
    generate_ast();
}

// Callback for when a client writes to the characteristic
void ble_write_cb(char* data, uint16_t len) {
    if (data && len > 2 * strlen(SEND_SCRIPT_FLAG)) {
        printf("Received data: %s\n", data);
        errorHandler.triggerStopExecution();

        set_script(std::string(data + strlen(SEND_SCRIPT_FLAG), len - strlen(SEND_SCRIPT_FLAG) * 2));

        send_string(SENT_SCRIPT_FLAG);
    }
}

void radio_write_cb(char* data, uint16_t len, uint8_t* src_addr) {
    // If data contains IDENTIFY_FLAG
    if (strstr(data, IDENTIFY_FLAG) != NULL) {
        // Get the type of sensor (sandwiched between two IDENTIFY_FLAGs)
        char* type = strstr(data, IDENTIFY_FLAG) + strlen(IDENTIFY_FLAG);
        char* end = strstr(type, IDENTIFY_FLAG);
        *end = '\0';

        std::array<uint8_t, 6> mac_address;
        std::memcpy(mac_address.data(), src_addr, 6);

        std::lock_guard<std::mutex> lock(tile_mutex);
        tile_map[mac_address] = string_to_tile_type(type);
    }
}

void query_tiles(void) {
    radio_broadcast(QUERY_FLAG, strlen(QUERY_FLAG));
}

// Yield to other tasks
void buffer(void* arg) {
    vTaskDelay(50 / portTICK_PERIOD_MS); // 50 ms
}

extern "C" void app_main(void) {

    // Setup timer based interrupt for delays to allow the various stacks to run
    esp_timer_create_args_t timer_args = {
		.callback = &buffer,
		.name = "buffer"
	};
	
	esp_timer_handle_t timer;
	esp_timer_create(&timer_args, &timer);
	esp_timer_start_periodic(timer, 4000000); // 0.25 Hz

    ble_init(ble_write_cb);

    radio_init(radio_write_cb);

    query_tiles();

    generate_ast();

    while (1) {

        //print available heap
        printf("Free heap: %ld\n", esp_get_free_heap_size());

        // Interpret the AST
        {
            std::lock_guard<std::mutex> lock(interpreter_mutex);
            if (interpreter != nullptr) {
                interpreter->interpret();
            }
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);

    }
}
