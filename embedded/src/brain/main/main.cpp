#include <cstring>
#include <mutex>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "ble.h"
#include "radio.h"

#include "ast.hpp"
#include "error.hpp"
#include "interpreter.hpp"
#include "outputStream.hpp"
#include "tokenizer.hpp"
#include "flags.h"


std::mutex mutex;

std::string script = "{print(420);}";

std::string get_script() {
    std::lock_guard<std::mutex> lock(mutex);
    return script;
}

void set_script(const std::string &s) {
    std::lock_guard<std::mutex> lock(mutex);
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
void write_cb(char* data, uint16_t len) {
    if(data && len > 2 * strlen(SEND_SCRIPT_FLAG)) {
        printf("Received data: %s\n", data);
        errorHandler.triggerStopExecution();

        set_script(std::string(data + strlen(SEND_SCRIPT_FLAG), len - strlen(SEND_SCRIPT_FLAG) * 2));

        errorHandler.resetStopExecution();

        send_string(SENT_SCRIPT_FLAG);
    }    
}

extern "C" void app_main(void) {

    ble_init(write_cb);

    radio_init();

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
