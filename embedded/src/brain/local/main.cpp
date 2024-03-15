// File for testing locally

#include <iostream>

#include "ast.hpp"
#include "error.hpp"
#include "interpreter.hpp"
#include "outputStream.hpp"
#include "tokenizer.hpp"

int main() {
    // Sample program for mess of various features
    std::string sourceCode = "{ void x(int y, int i) { print(y*i); } for(int i=0; i<10; i=i+1) { int y = 69; x(i, y); } }";
    
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
