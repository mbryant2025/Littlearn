#include <iostream>
#include "tokenizer.hpp"
#include "ast.hpp"
#include "interpreter.hpp"

int main()
{
    // Input source code
    std::string sourceCode = "{ "
        
                                "int x = 11;"
                                "int sum = x;"
                                "if (x - 10) {"
                                    "sum = sum + 1;"
                                    "print(x);"
                                "}"
                                "if (x - 11) {"
                                    "sum = sum + 2;"
                                "}"
                                "print(sum * 34);"
                                "print(x / 3.3);"

                             "}";

    // Create a Tokenizer object
    Tokenizer tokenizer(sourceCode);

    // Tokenize the source code
    std::vector<Token> tokens = tokenizer.tokenize();

    // Create a Parser object
    Parser parser(tokens);

    BlockNode* block = parser.parseProgram();

    std::cout << "AST: " << std::endl;

    std::cout << block->toString() << std::endl;

    std::cout << "==========================================================" << std::endl;

    // Create an Interpreter object
    Interpreter interpreter(block);

    // // Interpret the AST
    interpreter.interpret();

    return 0;
}

