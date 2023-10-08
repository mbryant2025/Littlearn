#include <iostream>
#include "tokenizer.hpp"
#include "ast.hpp"
#include "interpreter.hpp"

int main()
{
    // Input source code
    std::string sourceCode = "{ int x = 0; int sum = x; if(x + 1){//Add 3 to x \n x = x + 3; //Make new variable \n int y = 5.4;} print(x + 13.4); print(y);}";

    // Create a Tokenizer object
    Tokenizer tokenizer(sourceCode);

    // Tokenize the source code
    std::vector<Token> tokens = tokenizer.tokenize();

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

