#include <gtest/gtest.h>
#include "tokenizer.hpp"
#include "ast.hpp"
#include "interpreter.hpp"

TEST(InterpreterTest, testCollatz)
{
    std::string sourceCode = 
    "{"
        "int n = 343; // Chosen number \n"
        "int count = 0;"
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
        "}"
        "print(count);"
    "}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(tokens);

    BlockNode* block = parser.parseProgram();

    // Redirect stdout to a stringstream to capture the printed output
    std::stringstream capturedOutput;
    std::streambuf* originalStdout = std::cout.rdbuf(capturedOutput.rdbuf());

    Interpreter interpreter(block);

    // Call interpret and restore the original stdout
    interpreter.interpret();
    std::cout.rdbuf(originalStdout);

    // Check that the captured output contains "125"
    EXPECT_EQ(capturedOutput.str(), "125\n");

}