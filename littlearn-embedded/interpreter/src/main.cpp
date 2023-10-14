#include <iostream>
#include <thread>
#include "tokenizer.hpp"
#include "ast.hpp"
#include "interpreter.hpp"

int main()
{
    // Sample code for the Collatz Conjecture
    // Premise: Pick any positive integer n. If n is even, divide it by 2, otherwise multiply it by 3 and add 1.
    // Repeat this process until n is 1, and print out the number of steps it took to reach 1.
    std::string sourceCode = 
    "{"
        "int n = 343; // Chosen number \n"
        "int count = 0;"
        "print(n);"
        "while (n > 1) {"
            "count = count + 1;"
            "int temp = n % 2;"
            "// If n is even, divide it by 2, otherwise multiply it by 3 and add 1 \n"
            "if (temp - 1) {"
                "n = n / 2;"
            "}"
            "if (temp) {"
                "n = 3 * n;"
                "n = n + 1;"
            "}"
            "print(n);"
            // "wait(500); // Wait for 500 milliseconds between printing each number \n"
        "}"
        "print(count);"
    "}";

    // std::string sourceCode = "{"
    //     "int base = 3;"
    //     "int exponent = 10;"

    //     "int sum = base;"
    //     "int result = 1;"
    //     "while(exponent > 0) {"
    //         "if (exponent % 2) {"
    //             "result = sum * result;"
    //             "exponent = exponent - 1;"
    //         "}"
    //         "sum = sum * sum;"
    //         "exponent = exponent / 2;"
    //     "}"

    //     "print(result);"


    // "}";

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

