#include <iostream>
#include <thread>
#include "tokenizer.hpp"
#include "ast.hpp"
#include "interpreter.hpp"

int main()
{
    // Input source code
    std::string sourceCode = "{ "
        
                                "//Print the Fibonacci numbers from 0 to 100 \n"
                                "int first = 0;"
                                "int second = 1;"
                                "int next = 0;"

                                "while (second < 100) {"
                                    "print(second);"
                                    "next = first + second;"
                                    "first = second;"
                                    "second = next;"
                                    "wait(500); //Wait half a second between printing each number \n"

                                "}"

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

