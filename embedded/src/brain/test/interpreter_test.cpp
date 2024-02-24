#include <gtest/gtest.h>
#include "tokenizer.hpp"
#include "ast.hpp"
#include "interpreter.hpp"
#include "flags.h"

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
    const std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    BlockNode* block = parser.parseProgram();

    // Redirect stdout to a stringstream to capture the printed output
    std::stringstream capturedOutput;
    std::streambuf* originalStdout = std::cout.rdbuf(capturedOutput.rdbuf());

    if(block != nullptr) {
        Interpreter interpreter(*block, outputStream, errorHandler);

        // Call interpret and restore the original stdout
        interpreter.interpret();
        std::cout.rdbuf(originalStdout);

        // Check that the captured output contains "125"
        EXPECT_EQ(capturedOutput.str(), "__PRINT__125\n__PRINT__");
    } else {
        FAIL();
    }
}

// TEST(InterpreterTest, testExpression1)
// {
//     std::string sourceCode = "{int x = 2 - -5; print(x);}";
//     Tokenizer tokenizer(sourceCode);
//     const std::vector<Token> tokens = tokenizer.tokenize();

//     OutputStream* outputStream = new StandardOutputStream;
//     ErrorHandler* errorHandler = new ErrorHandler(outputStream);

//     Parser parser(tokens, *outputStream, *errorHandler);

//     BlockNode* block = parser.parseProgram();

//     // Redirect stdout to a stringstream to capture the printed output
//     std::stringstream capturedOutput;
//     std::streambuf* originalStdout = std::cout.rdbuf(capturedOutput.rdbuf());

//     if(block != nullptr) {
//         Interpreter interpreter(*block, *outputStream, *errorHandler);

//         // Call interpret and restore the original stdout
//         interpreter.interpret();
//         std::cout.rdbuf(capturedOutput.rdbuf());

//         // Check that the captured output contains "125"
//         EXPECT_EQ(capturedOutput.str(), "7\n");
//     } else {
//         FAIL();
//     }
// }

// TEST(InterpreterTest, testExpression2)
// {
//     std::string sourceCode = "{int x  = (2 - -5); print(x);}";
//     Tokenizer tokenizer(sourceCode);
//     std::vector<Token> tokens = tokenizer.tokenize();

//     OutputStream* outputStream = new StandardOutputStream;
//     Parser parser(tokens, outputStream);

//     BlockNode* block = parser.parseProgram();

//     // Redirect stdout to a stringstream to capture the printed output
//     std::stringstream capturedOutput;
//     std::streambuf* originalStdout = std::cout.rdbuf(capturedOutput.rdbuf());

//     Interpreter interpreter(block, outputStream);

//     // Call interpret and restore the original stdout
//     interpreter.interpret();
//     std::cout.rdbuf(originalStdout);

//     // Check that the captured output contains "125"
//     EXPECT_EQ(capturedOutput.str(), "7\n");

// }

// TEST(InterpreterTest, testExpression3)
// {
//     std::string sourceCode = "{int x  = (2*1 * (2-5*1)); print(x);}";
//     Tokenizer tokenizer(sourceCode);
//     std::vector<Token> tokens = tokenizer.tokenize();

//     OutputStream* outputStream = new StandardOutputStream;
//     Parser parser(tokens, outputStream);

//     BlockNode* block = parser.parseProgram();

//     // Redirect stdout to a stringstream to capture the printed output
//     std::stringstream capturedOutput;
//     std::streambuf* originalStdout = std::cout.rdbuf(capturedOutput.rdbuf());

//     Interpreter interpreter(block, outputStream);

//     // Call interpret and restore the original stdout
//     interpreter.interpret();
//     std::cout.rdbuf(originalStdout);

//     // Check that the captured output contains "125"
//     EXPECT_EQ(capturedOutput.str(), "-6\n");

// }

// TEST(InterpreterTest, testExpression4)
// {
//     std::string sourceCode = "{int x  = (2*1) * (2-5*1)); print(x);}";
//     Tokenizer tokenizer(sourceCode);
//     std::vector<Token> tokens = tokenizer.tokenize();

//     OutputStream* outputStream = new StandardOutputStream;
//     Parser parser(tokens, outputStream);

//     BlockNode* block = parser.parseProgram();

//     // Redirect stdout to a stringstream to capture the printed output
//     std::stringstream capturedOutput;
//     std::streambuf* originalStdout = std::cout.rdbuf(capturedOutput.rdbuf());

//     Interpreter interpreter(block, outputStream);

//     // Call interpret and restore the original stdout
//     interpreter.interpret();
//     std::cout.rdbuf(originalStdout);

//     // Check that the captured output contains "125"
//     EXPECT_EQ(capturedOutput.str(), "-6\n");

// }

// TEST(InterpreterTest, testExpression5)
// {
//     std::string sourceCode = "{int x  = ((2 * 4) + (-12 - -8 /2)); print(x);}";
//     Tokenizer tokenizer(sourceCode);
//     std::vector<Token> tokens = tokenizer.tokenize();

//     OutputStream* outputStream = new StandardOutputStream;
//     Parser parser(tokens, outputStream);

//     BlockNode* block = parser.parseProgram();

//     // Redirect stdout to a stringstream to capture the printed output
//     std::stringstream capturedOutput;
//     std::streambuf* originalStdout = std::cout.rdbuf(capturedOutput.rdbuf());

//     Interpreter interpreter(block, outputStream);

//     // Call interpret and restore the original stdout
//     interpreter.interpret();
//     std::cout.rdbuf(originalStdout);

//     // Check that the captured output contains "125"
//     EXPECT_EQ(capturedOutput.str(), "0\n");

// }