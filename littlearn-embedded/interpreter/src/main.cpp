#include <iostream>
#include <thread>
#include "tokenizer.hpp"
#include "ast.hpp"
#include "interpreter.hpp"

int main()
{

    // while(1) {

    // Sample code for the Collatz Conjecture
    // Premise: Pick any positive integer n. If n is even, divide it by 2, otherwise multiply it by 3 and add 1.
    // Repeat this process until n is 1, and print out the number of steps it took to reach 1.
    std::string sourceCode = 
    "{"
        // "int n = 343; // Chosen number \n"
        // "int count = 0;"
        // "print(n);"
        // "while (n > 1) {"
        //     "count = count + 1;"
        //     "int temp = n % 2;"
        //     "// If n is even, divide it by 2, otherwise multiply it by 3 and add 1 \n"
        //     "if (temp - 1) {"
        //         "n = n / 2;"
        //     "}"
        //     "if (temp) {"
        //         "n = 3 * n;"
        //         "n = n + 1;"
        //     "}"
            // "print(n);"
            // "wait(500); // Wait for 500 milliseconds between printing each number \n"
        // "}"
        // "print(count);"
    "}";

    // Create a Tokenizer object
    Tokenizer tokenizer(sourceCode);

    // Tokenize the source code
    std::vector<Token> tokens = tokenizer.tokenize();

    // Create a Parser object
    Parser parser(tokens);

    BlockNode* block = parser.parseProgram();

    // Create an Interpreter object
    Interpreter interpreter(block);

    // Interpret the AST
    // interpreter.interpret();

    delete block;

    // }

    return 0;
}

