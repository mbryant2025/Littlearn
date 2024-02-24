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


std::string sourceCode = "{int x = 420 + 69; print(cos(x)); print(x, 2*x);}";

class BLEOutputStream : public OutputStream {
   public:
    void write(const std::string &message) override {
        printf("Received data: %s\n", message.c_str());
        send_string(message.c_str());
    }
};

BLEOutputStream outputStream;
ErrorHandler errorHandler(outputStream);

// Callback for when a client writes to the characteristic
void write_cb(char* data, uint16_t len) {
    printf("Received data: %s\n", data);
}

extern "C" void app_main(void) {

    ble_init(write_cb);

    radio_init();

    while(1) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        
        send_string("__PRINT__Hello, BLE!__PRINT__");

        Tokenizer tokenizer(sourceCode);

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

        errorHandler.resetStopExecution();

    }
}
