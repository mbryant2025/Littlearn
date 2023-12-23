#include <iostream>
#include <thread>

#include "ast.hpp"
#include "error.hpp"
#include "interpreter.hpp"
#include "outputStream.hpp"
#include "tokenizer.hpp"

int main() {
    std::string sourceCode = "{int w = x -5;}";

    // Create an OutputStream object for errors and print statements
    OutputStream* outputStream = new StandardOutputStream;
    ErrorHandler* errorHandler = new ErrorHandler(outputStream);

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
    Parser parser(tokens, *outputStream, *errorHandler);

    // std::cout << "Parsing program..." << std::endl;

    BlockNode* block = parser.parseProgram();

    // Print the AST
    if(block != nullptr)
        std::cout << block->toString() << std::endl;

    // // Create an Interpreter object
    // Interpreter interpreter(block, outputStream, errorHandler);

    // // // Interpret the AST
    // interpreter.interpret();

    std::cout << std::endl;


    if(errorHandler->shouldStopExecution()) {
        std::cout << "Error" << std::endl;
    } else {
        std::cout << "No error" << std::endl;
    }

    // Free memory
    delete errorHandler;
    delete outputStream;

    if (block != nullptr) delete block;

    // block is freed by the interpreter

    std::cout << "\nDone" << std::endl;

    return 0;
}
