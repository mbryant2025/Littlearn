#include <iostream>
#include <thread>
#include "tokenizer.hpp"
#include "ast.hpp"
#include "interpreter.hpp"

int main()
{
    // Input source code for the Collatz Conjecture
    std::string sourceCode = 
    "{"
        "int n = 77031;"
        "int count = 0;"
        "while (n - 1) {"
            "count = count + 1;"
            "int temp = n % 2;"
            "if (temp - 1) {"
                "n = n / 2;"
            "}"
            "if (temp) {"
                "n = 3 * n;"
                "n = n + 1;"
            "}"
            "print(n);"
        "}"
        "print(count);"
    "}";

    // Create a Tokenizer object
    Tokenizer tokenizer(sourceCode);

    // Tokenize the source code
    std::vector<Token> tokens = tokenizer.tokenize();

    // // Print out the tokens
    // std::cout << "Tokens: " << std::endl;
    // for (const Token& token : tokens)
    // {
    //     std::cout << Tokenizer::tokenTypeToString(token.type) << " " << token.lexeme << std::endl;
    // }

    // Create a Parser object
    Parser parser(tokens);

    BlockNode* block = parser.parseProgram();

    // std::cout << "AST: " << std::endl;

    // std::cout << block->toString() << std::endl;

    // std::cout << "==========================================================" << std::endl;

    // Create an Interpreter object
    Interpreter interpreter(block);

    // // Interpret the AST
    interpreter.interpret();

    return 0;
}

