#include <gtest/gtest.h>
#include "tokenizer.hpp"
#include "ast.hpp"
#include "outputStream.hpp"
#include "error.hpp"

TEST(ASTTest, gatherTokensUntilSimple)
{
    std::string sourceCode = "int sum = 5;";
    Tokenizer tokenizer(sourceCode);
    const std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    std::vector<const Token *> until = parser.gatherTokensUntil(TokenType::SEMICOLON);

    // We just gathered all tokens up to and including the semicolon, so we should have 5 tokens
    ASSERT_EQ(until.size(), 5);

    // Check that the last one is indeed the semicolon
    EXPECT_EQ(until[4]->type, TokenType::SEMICOLON);
    EXPECT_EQ(until[4]->lexeme, ";");
}

TEST(ASTTest, gatherTokensUntilSlice)
{
    std::string sourceCode = "int sum = 5; float pi = 3.14; //this is a comment\n while (sum < 5) {sum = sum + 1;};";
    Tokenizer tokenizer(sourceCode);
    const std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    std::vector<const Token *> until = parser.gatherTokensUntil(TokenType::SEMICOLON);

    // We just gathered all tokens up to and including the semicolon, so we should have 5 tokens
    ASSERT_EQ(until.size(), 5);

    // Check that the last one is indeed the semicolon
    EXPECT_EQ(until[4]->type, TokenType::SEMICOLON);
    EXPECT_EQ(until[4]->lexeme, ";");
}

TEST(ASTTest, gatherTokensUntilSemicolon)
{
    std::string sourceCode = "{ sum = 20;} float w = 5.3;";
    Tokenizer tokenizer(sourceCode);
    const std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    std::vector<const Token *> until = parser.gatherTokensUntil(TokenType::SEMICOLON);

    // We just gathered all tokens up to and including the semicolon, so we should have 4 tokens
    ASSERT_EQ(until.size(), 5);
}

TEST(ASTTest, gatherTokensUntilComplicated)
{
    std::string sourceCode = "(x == 10)) {x = x + 2} \n //this is my annoying comment {()(){}{} \nfloat z = y + 2.0;}";
    Tokenizer tokenizer(sourceCode);
    const std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    std::vector<const Token *> until = parser.gatherTokensUntil(TokenType::RIGHT_PARENTHESIS);

    // We just gathered all tokens up to and including the parenthesis, so we should have 6 tokens
    // This is 23 tokens
    ASSERT_EQ(until.size(), 6);

    // The first and last should be parentheses
    EXPECT_EQ(until[0]->type, TokenType::LEFT_PARENTHESIS);
    EXPECT_EQ(until[0]->lexeme, "(");
    EXPECT_EQ(until[5]->type, TokenType::RIGHT_PARENTHESIS);
    EXPECT_EQ(until[5]->lexeme, ")");

    // The second is an identifier
    EXPECT_EQ(until[1]->type, TokenType::IDENTIFIER);
    EXPECT_EQ(until[1]->lexeme, "x");

    // The third is an operator
    EXPECT_EQ(until[2]->type, TokenType::OPERATOR);
    EXPECT_EQ(until[2]->lexeme, "==");

    // The fourth is an integer
    EXPECT_EQ(until[3]->type, TokenType::INTEGER);
    EXPECT_EQ(until[3]->lexeme, "10");

}

TEST(ASTTest, parseConstantInt) {
    std::string sourceCode = "5;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    NumberNode* node = parser.parseConstant();

    EXPECT_EQ((*node).getValue(), "5");
    EXPECT_EQ((*node).getType(), TokenType::INTEGER);
}

TEST(ASTTest, parseConstantFloat) {
    std::string sourceCode = "5.43;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    NumberNode* node = parser.parseConstant();

    EXPECT_EQ((*node).getValue(), "5.43");
    EXPECT_EQ((*node).getType(), TokenType::FLOAT);
}

TEST(ASTTest, parseConstantInvalid) {
    std::string sourceCode = "{int x = 5.43.2;}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    BlockNode* node = parser.parseProgram();

    // The error handler should be called
    EXPECT_EQ(errorHandler.shouldStopExecution(), true);

    errorHandler.resetStopExecution();

}

TEST(ASTTest, parseFunction) {
    std::string sourceCode = "{print(5);}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    BlockNode* node = parser.parseProgram();

    EXPECT_EQ(node->toString(), "BLOCK NODE {\nFUNCTION CALL print ( NUMBER 5, )\n}");
}

TEST(ASTTest, parseNestedFunction) {
    std::string sourceCode = "{print(5 + 2);}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    BlockNode* node = parser.parseProgram();

    EXPECT_EQ(node->toString(), "BLOCK NODE {\nFUNCTION CALL print ( BINARY OPERATION (NUMBER 5 + NUMBER 2), )\n}");
}

TEST(ASTTest, parseNestedFunction2) {
    std::string sourceCode = "{print(5 + print(x));}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    BlockNode* node = parser.parseProgram();

    EXPECT_EQ(node->toString(), "BLOCK NODE {\nFUNCTION CALL print ( BINARY OPERATION (NUMBER 5 + FUNCTION CALL print ( VARIABLE ACCESS x, )), )\n}");
}

TEST(ASTTest, parseNestedFunction3) {
    std::string sourceCode = "{print(5 + print(x + 2));}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    BlockNode* node = parser.parseProgram();

    EXPECT_EQ(node->toString(), "BLOCK NODE {\nFUNCTION CALL print ( BINARY OPERATION (NUMBER 5 + FUNCTION CALL print ( BINARY OPERATION (VARIABLE ACCESS x + NUMBER 2), )), )\n}");
}

TEST(ASTTest, parseMultipleParameterFunction) {
    std::string sourceCode = "{print(5, 2);}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    BlockNode* node = parser.parseProgram();

    EXPECT_EQ(node->toString(), "BLOCK NODE {\nFUNCTION CALL print ( NUMBER 5, NUMBER 2, )\n}");
}

TEST(ASTTest, parseMultipleParameterFunction2) {
    std::string sourceCode = "{print(5, 2, 3, 4, print(3), 6, 7, 8, 9, 10);}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    BlockNode* node = parser.parseProgram();

    EXPECT_EQ(node->toString(), "BLOCK NODE {\nFUNCTION CALL print ( NUMBER 5, NUMBER 2, NUMBER 3, NUMBER 4, FUNCTION CALL print ( NUMBER 3, ), NUMBER 6, NUMBER 7, NUMBER 8, NUMBER 9, NUMBER 10, )\n}");
}

TEST(ASTTest, parseMultipleParameterFunction3) {
    std::string sourceCode = "{float w = print(5, 2, 3, 4, print(3), 6, 7, 8, 9, 10);}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);

    BlockNode* node = parser.parseProgram();

    EXPECT_EQ(node->toString(), "BLOCK NODE {\nVARIABLE DECLARATION float w = FUNCTION CALL print ( NUMBER 5, NUMBER 2, NUMBER 3, NUMBER 4, FUNCTION CALL print ( NUMBER 3, ), NUMBER 6, NUMBER 7, NUMBER 8, NUMBER 9, NUMBER 10, )\n}");
}

TEST(ASTTest, parseSimpleNesting) {
    std::string sourceCode = "{int x = (5);}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);
    
    Parser parser(tokens, outputStream, errorHandler);

    BlockNode* node = parser.parseProgram();

    EXPECT_EQ(node->toString(), "BLOCK NODE {\nVARIABLE DECLARATION int x = NUMBER 5\n}");
}

TEST(ASTTest, moreNesting) {
    std::string sourceCode = "{int x = (5 + 2) + print((((((3))))));}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);
    
    Parser parser(tokens, outputStream, errorHandler);

    BlockNode* node = parser.parseProgram();

    EXPECT_EQ(node->toString(), "BLOCK NODE {\nVARIABLE DECLARATION int x = BINARY OPERATION (BINARY OPERATION (NUMBER 5 + NUMBER 2) + FUNCTION CALL print ( NUMBER 3, ))\n}");
}

TEST(ASTTest, parseAnnoyingNesting) {
    std::string sourceCode = "{print(5, 2, 3, 4, (print(3)), 6, 7, 8, 9, 10 + print(5, 6, 7, 8, 9, 10));}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);

    Parser parser(tokens, outputStream, errorHandler);
    BlockNode* node = parser.parseProgram();

    EXPECT_EQ(node->toString(), "BLOCK NODE {\nFUNCTION CALL print ( NUMBER 5, NUMBER 2, NUMBER 3, NUMBER 4, FUNCTION CALL print ( NUMBER 3, ), NUMBER 6, NUMBER 7, NUMBER 8, NUMBER 9, BINARY OPERATION (NUMBER 10 + FUNCTION CALL print ( NUMBER 5, NUMBER 6, NUMBER 7, NUMBER 8, NUMBER 9, NUMBER 10, )), )\n}");
}

TEST(ASTTest, missedSemicolon) {
    std::string sourceCode = "{int x = 5}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    StandardOutputStream outputStream;
    ErrorHandler errorHandler(outputStream);
    
    Parser parser(tokens, outputStream, errorHandler);

    BlockNode* node = parser.parseProgram();

    EXPECT_EQ(errorHandler.shouldStopExecution(), true);

    errorHandler.resetStopExecution();
}
