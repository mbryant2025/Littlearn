#include <iostream>
#include "tokenizer.hpp" // Include the header file for the Tokenizer class

int main()
{
    // Input source code
    std::string sourceCode = "int sum = 0; float pi = 3.14; //this is a comment\n while (sum < 5) {sum = sum + 1;};";

    // Create a Tokenizer object
    Tokenizer tokenizer(sourceCode);

    // Tokenize the source code
    std::vector<Token> tokens = tokenizer.tokenize();

    // Print the tokens and their types
    for (const Token &token : tokens)
    {
        std::cout << "Token Type: ";
        switch (token.type)
        {
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
        case TokenType::LEFT_BRACE:
            std::cout << "LEFT_BRACE";
            break;
        case TokenType::RIGHT_BRACE:
            std::cout << "RIGHT_BRACE";
            break;
        case TokenType::LEFT_PARENTHESIS:
            std::cout << "LEFT_PARENTHESIS";
            break;
        case TokenType::RIGHT_PARENTHESIS:
            std::cout << "RIGHT_PARENTHESIS";
            break;
        case TokenType::UNKNOWN:
            std::cout << "UNKNOWN";
            break;
        }
        std::cout << ", Lexeme: " << token.lexeme << std::endl;
    }

    return 0;
}
