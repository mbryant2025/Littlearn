#include <iostream>
#include "tokenizer.hpp"
#include "ast.hpp"

int main()
{
    // Input source code
    // std::string sourceCode = "int sum = 0; float pi = 3.14; //this is a comment\n while (sum < 5) {sum = sum + 1;};";
    std::string sourceCode = "{int sum = 5; float x = sum; if(x) {x = 4;}}";

    // Create a Tokenizer object
    Tokenizer tokenizer(sourceCode);

    // Tokenize the source code
    std::vector<Token> tokens = tokenizer.tokenize();

    // Create a Parser object
    Parser parser(tokens);

    BlockNode* block = parser.parseProgram();

    std::cout << "AST: " << std::endl;

    std::cout << block->toString() << std::endl;

    return 0;
}

