#include <iostream>
#include <thread>
#include "tokenizer.hpp"
#include "ast.hpp"
#include "interpreter.hpp"
#include "error.hpp"

void poll() {
    std::cout << "polling" << std::endl;
    // triggerStopExecution();
}

int main()
{

    // while(1) {

    // Sample code for the Collatz Conjecture
    // Premise: Pick any positive integer n. If n is even, divide it by 2, otherwise multiply it by 3 and add 1.
    // Repeat this process until n is 1, and print out the number of steps it took to reach 1.
    std::string sourceCode = //"{int port = 2; int val = 30; write_port(port, val);}";
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
            "wait(5); // Wait for 5 milliseconds between printing each number \n"
        "}"
        "print(count);"
        "int x = read_port(1);"
        "write_port(2, x);"
        "print_seven_segment(10);"
        // "print(w);"
    "}";


    // std::string sourceCode = "{int x = 5;}";

    // Create a Tokenizer object
    Tokenizer tokenizer(sourceCode);

    // Tokenize the source code
    std::vector<Token> tokens = tokenizer.tokenize();

    //print token
    for (auto token : tokens)
    {
        std::cout << Tokenizer::tokenTypeToString(token.type) << " " << token.lexeme << std::endl;
    }

    // Create a Parser object
    Parser parser(tokens);

    BlockNode* block = parser.parseProgram();

    //Print the AST
    std::cout << block->toString() << std::endl;

    // Create an Interpreter object
    Interpreter interpreter(block, poll);

    // Interpret the AST
    interpreter.interpret();

    // }

    return 0;
}

