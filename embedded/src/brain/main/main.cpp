#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#include "ast.hpp"
#include "error.hpp"
#include "interpreter.hpp"
#include "outputStream.hpp"
#include "tokenizer.hpp"

#define LED_BUILTIN (gpio_num_t)2

void runCode() {

    // Sample program for recursive fibonacci
    std::string sourceCode = "{" 
        "int fibonacci(int n) {"
             "if(n == 0) { return 0; }"
             "else if(n == 1) { return 1; }"
             "else { return fibonacci(n - 1) + fibonacci(n - 2); }" 
        "}"
        
        "for(int i = 0; i < 10; i = i + 1) {"
            // "print(fibonacci(i));"
        "}"
    "}";

    // Create an OutputStream object for errors and print statements
    OutputStream* outputStream = new StandardOutputStream;
    ErrorHandler* errorHandler = new ErrorHandler(outputStream);

    // Create a Tokenizer object
    // No error handling is done here as the tokenizer is not supposed to fail
    // Therefore, we do not need to pass the ErrorHandler object to the Tokenizer constructor
    Tokenizer tokenizer(sourceCode);

    // Tokenize the source code
    const std::vector<Token> tokens = tokenizer.tokenize();

    // Create a Parser object
    Parser parser(tokens, *outputStream, *errorHandler);

    BlockNode* block = parser.parseProgram();

    // Create an Interpreter object
    if (block != nullptr) {

         Interpreter interpreter(*block, *outputStream, *errorHandler);

        // Interpret the AST
        interpreter.interpret();

        delete block;

        gpio_set_level(LED_BUILTIN, 0);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        gpio_set_level(LED_BUILTIN, 1);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        
    }

    // Free memory
    delete errorHandler;
    delete outputStream;


}

extern "C" void app_main() {

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
        runCode();
    }
}
