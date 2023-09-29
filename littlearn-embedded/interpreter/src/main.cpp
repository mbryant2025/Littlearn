#include <iostream>
#include "tokenizer.hpp" // Include the header file for the Tokenizer class

int main() {
    // Input source code
    std::string sourceCode = "int sum = 0; float pi = 3.14;";

    // Create a Tokenizer object
    Tokenizer tokenizer(sourceCode);

    // Tokenize the source code
    std::vector<Token> tokens = tokenizer.tokenize();

    // Print the tokens and their types
    for (const Token& token : tokens) {
        std::cout << "Token Type: ";
        switch (token.type) {
            case TokenType::KEYWORD:
                std::cout << "KEYWORD";
                break;
            case TokenType::IDENTIFIER:
                std::cout << "IDENTIFIER";
                break;
            case TokenType::INTEGER:
                std::cout << "INTEGER";
                break;
            case TokenType::FLOAT:
                std::cout << "FLOAT";
                break;
            case TokenType::OPERATOR:
                std::cout << "OPERATOR";
                break;
            case TokenType::UNKNOWN:
                std::cout << "UNKNOWN";
                break;
        }
        std::cout << ", Lexeme: " << token.lexeme << std::endl;
    }

    return 0;
}
