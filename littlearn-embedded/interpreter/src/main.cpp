#include <iostream>
#include <thread>
#include "tokenizer.hpp"
#include "ast.hpp"
#include "interpreter.hpp"
#include "error.hpp"
#include "outputStream.hpp"


int main()
{

    // Sample code for the Collatz Conjecture
    // Premise: Pick any positive integer n. If n is even, divide it by 2, otherwise multiply it by 3 and add 1.
    // Repeat this process until n is 1, and print out the number of steps it took to reach 1.
    // std::string sourceCode = //"{int port = 2; int val = 30; write_port(port, val);}";
    // "{"
    //     "int n = 343; // Chosen number \n"
    //     "int count = 0;"
    //     "print(n);"
    //     "while (n > 1) {"
    //         "count = count + 1;"
    //         "// If n is even, divide it by 2, otherwise multiply it by 3 and add 1 \n"
    //         "if (n % 2) {"
    //             "n = 3 * n + 1;"
    //         "} else {"
    //             "n = n / 2;"
    //         "}"
    //         "print(n);"
    //         "continue;"
    //         "wait(5); // Wait for 5 milliseconds between printing each number \n"
    //     "}"
    //     "print(count);"
    //     "int x = read_port(1); print(317 + x);"
    //     "write_port(read_port(10), x*(4+3));"
    //     "print_seven_segment(10);"
    // "}";


    std::string sourceCode = "{float x = 3.14; print(x); }";
    // std::string sourceCode = "{int y = 0;"
    //                          "if(y-2*y) {"
    //                             "print(42);"
    //                          "} else {"
    //                             "print(69);"
    //                          "}}";

    // Create a Tokenizer object
    Tokenizer tokenizer(sourceCode);

    // Tokenize the source code
    std::vector<Token> tokens = tokenizer.tokenize();

    // Print tokens
    // for (auto token : tokens)
    // {
    //     std::cout << Tokenizer::tokenTypeToString(token.type) << " " << token.lexeme << std::endl;
    // }

    // Create an OutputStream object for errors and print statements
    OutputStream* outputStream = new StandardOutputStream;

    // Create a Parser object
    Parser parser(tokens, outputStream);

    BlockNode* block = parser.parseProgram();

    // Print the AST
    // std::cout << block->toString() << std::endl;

    // Create an Interpreter object
    Interpreter interpreter(block, outputStream);

    // Interpret the AST
    interpreter.interpret();

    // Free memory
    delete outputStream;
    // block is freed by the interpreter


    return 0;
}

