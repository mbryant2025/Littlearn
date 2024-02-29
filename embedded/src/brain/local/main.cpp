// File for testing locally

#include <iostream>

#include "ast.hpp"
#include "error.hpp"
#include "interpreter.hpp"
#include "outputStream.hpp"
#include "tokenizer.hpp"

int main() {
    // Sample program for mess of various features
    // std::string sourceCode = "{ void x(int p) {  for(int i = 0; i < 2000*p; i = i + runtime()) {float t = rand(); print(t + i); wait(float_to_int(1000*t));} } x(10); print(100); x(6.5);  }";

    // Sample program for recursive fibonacci
    std::string sourceCode = "{" 
        "int fibonacci(int n) {"
             "if(n == 0) { return 0; }"
             "else if(n == 1) { return 1; }"
             "else { return fibonacci(n - 1) + fibonacci(n - 2); }" 
        "}"
        
        "for(int i = 0; i < 3; i = i + 1) {"
            "print(fibonacci(i));"
        "}"
        // "send_bool(0, 1);"
    "}";
    
    
    
    // std::string sourceCode = "{  int MICHAEL= ---!!-!-!-!!!-!5; print(MICHAEL); }";
    // std::string sourceCode = "{  int MICHAEL= --5; print(MICHAEL); }";

    // Create an OutputStream object for errors and print statements
    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    // Create a Tokenizer object
    // No error handling is done here as the tokenizer is not supposed to fail
    // Therefore, we do not need to pass the ErrorHandler object to the Tokenizer constructor
    Tokenizer tokenizer(sourceCode);

    // Tokenize the source code
    const std::vector<Token> tokens = tokenizer.tokenize();

    // Print tokens
    for (auto token : tokens) {
        std::cout << Tokenizer::tokenTypeToString(token.type) << " " << token.lexeme << std::endl;
    }

    std::cout << "Now onto parsing" << std::endl;

    // Create a Parser object
    Parser parser(tokens, outputStream, errorHandler);

    // std::cout << "Parsing program..." << std::endl;

    BlockNode* block = parser.parseProgram();

    // Create an Interpreter object
    if (block != nullptr) {
        std::cout << block->toString() << std::endl;

         Interpreter interpreter(*block, outputStream, errorHandler);

        // Interpret the AST
        interpreter.interpret();

        delete block;
    }
   

    std::cout << std::endl;


    if(errorHandler.shouldStopExecution()) {
        std::cout << "Error" << std::endl;
    } else {
        std::cout << "No error" << std::endl;
    }

    std::cout << "\nDone" << std::endl;

    return 0;
}
