#include <gtest/gtest.h>
#include "tokenizer.hpp"
#include "ast.hpp"

// TEST(ASTTest, gatherTokensUntilSimple)
// {
//     std::string sourceCode = "int sum = 5;";
//     Tokenizer tokenizer(sourceCode);
//     std::vector<Token> tokens = tokenizer.tokenize();

//     Parser parser(tokens);

//     std::vector<const Token *> until = parser.gatherTokensUntil(TokenType::SEMICOLON, true);

//     // We just gathered all tokens up to and including the semicolon, so we should have 5 tokens
//     ASSERT_EQ(until.size(), 5);

//     // Check that the last one is indeed the semicolon
//     EXPECT_EQ(until[4]->type, TokenType::SEMICOLON);
//     EXPECT_EQ(until[4]->lexeme, ";");
// }

// TEST(ASTTest, gatherTokensUntilSlice)
// {
//     std::string sourceCode = "int sum = 5; float pi = 3.14; //this is a comment\n while (sum < 5) {sum = sum + 1;};";
//     Tokenizer tokenizer(sourceCode);
//     std::vector<Token> tokens = tokenizer.tokenize();

//     Parser parser(tokens);

//     std::vector<const Token *> until = parser.gatherTokensUntil(TokenType::SEMICOLON, true);

//     // We just gathered all tokens up to and including the semicolon, so we should have 5 tokens
//     ASSERT_EQ(until.size(), 5);

//     // Check that the last one is indeed the semicolon
//     EXPECT_EQ(until[4]->type, TokenType::SEMICOLON);
//     EXPECT_EQ(until[4]->lexeme, ";");
// }

// TEST(ASTTest, gatherTokensUntilBrace)
// {
//     std::string sourceCode = "{ sum = 20;} float w = 5.3;";
//     Tokenizer tokenizer(sourceCode);
//     std::vector<Token> tokens = tokenizer.tokenize();

//     Parser parser(tokens);

//     // We want to eat that left brace
//     parser.eatToken(TokenType::LEFT_BRACE);

//     std::vector<const Token *> until = parser.gatherTokensUntil(TokenType::RIGHT_BRACE, true);

//     // We just gathered all tokens up to and including the brace
//     // This is 5 tokens
//     ASSERT_EQ(until.size(), 5);

//     // Check that the last one is indeed the brace
//     EXPECT_EQ(until[4]->type, TokenType::RIGHT_BRACE);
//     EXPECT_EQ(until[4]->lexeme, "}");
// }

// TEST(ASTTest, gatherTokensUntilComplicated)
// {
//     std::string sourceCode = "{int sum = 5; float y = 3; if(x > 5){sum = 6;}} \n //this is my annoying comment {()(){}{} \nfloat z = y + 2.0;";
//     Tokenizer tokenizer(sourceCode);
//     std::vector<Token> tokens = tokenizer.tokenize();

//     Parser parser(tokens);

//     // We want to eat that first left brace
//     parser.eatToken(TokenType::LEFT_BRACE);

//     std::vector<const Token *> until = parser.gatherTokensUntil(TokenType::RIGHT_BRACE, true);

//     // std::cout << "Printing tokens in until vector" << std::endl;
//     // for(const Token* token : until) {
//     //     std::cout << token->lexeme << std::endl;
//     // }

//     // We just gathered all tokens up to and including the brace
//     // // This is 23 tokens
//     ASSERT_EQ(until.size(), 23);

//     // Check that the last one is indeed the brace
//     EXPECT_EQ(until[22]->type, TokenType::RIGHT_BRACE);

//     // Check that the second to last one is also a brace
//     EXPECT_EQ(until[21]->type, TokenType::RIGHT_BRACE);

//     // Check that the one before that is a semicolon
//     EXPECT_EQ(until[20]->type, TokenType::SEMICOLON);
// }

// TEST(ASTTest, gatherTokensUntilMismatchedBraces)
// {

//     std::string sourceCode = "{int sum = 5; float y = 3; if(x > 5){float z = y + 2.0;";
//     Tokenizer tokenizer(sourceCode);
//     std::vector<Token> tokens = tokenizer.tokenize();

//     Parser parser(tokens);

//     // We want to eat that first left brace
//     parser.eatToken(TokenType::LEFT_BRACE);

//     // Check for cerr output
//     // testing::internal::CaptureStderr();

//     // std::vector<const Token*> until = parser.gatherTokensUntil(TokenType::RIGHT_BRACE, true);

//     // Check that the std::cerr output is correct
//     //  EXPECT_EQ(testing::internal::GetCapturedStderr(), "Syntax Error at token z: Unexpected end of file, expected RIGHT_BRACE\n");

//     // Check that the program exits
//     EXPECT_EXIT(parser.gatherTokensUntil(TokenType::RIGHT_BRACE, true), ::testing::ExitedWithCode(1), ".*");
// }

// TEST(ASTTest, gatherTokensUntilMismatchedParentheses)
// {

//     std::string sourceCode = "{int sum = 5; float y = 3; if(x > 5{float z = y + 2.0;}";
//     Tokenizer tokenizer(sourceCode);
//     std::vector<Token> tokens = tokenizer.tokenize();

//     Parser parser(tokens);

//     // We want to eat that first left brace
//     parser.eatToken(TokenType::LEFT_BRACE);

//     // Check for cerr output
//     // testing::internal::CaptureStderr();

//     // std::vector<const Token*> until = parser.gatherTokensUntil(TokenType::RIGHT_BRACE, true);

//     // Check that the std::cerr output is correct
//     //  EXPECT_EQ(testing::internal::GetCapturedStderr(), "Syntax Error at token z: Unexpected end of file, expected RIGHT_BRACE\n");

//     // Check that the program exits
//     EXPECT_EXIT(parser.gatherTokensUntil(TokenType::RIGHT_BRACE, true), ::testing::ExitedWithCode(1), ".*");
// }

// TEST(ASTTest, gatherTokensUntilDisallowedMixed)
// {
//     std::string sourceCode = "{int sum = 5; float y = 3; if(x > 5){float z = y + 2.0;} \\prepare for a programming sin \n {(});";
//     Tokenizer tokenizer(sourceCode);
//     std::vector<Token> tokens = tokenizer.tokenize();

//     Parser parser(tokens);

//     // We want to eat that first left brace
//     parser.eatToken(TokenType::LEFT_BRACE);

//     // Check for cerr output
//     // testing::internal::CaptureStderr();

//     // std::vector<const Token*> until = parser.gatherTokensUntil(TokenType::RIGHT_BRACE, true);

//     //Check that the std::cerr output is correct
//     // EXPECT_EQ(testing::internal::GetCapturedStderr(), "Syntax Error at token z: Unexpected end of file, expected RIGHT_BRACE\n");

//     // Check that the program exits
//     EXPECT_EXIT(parser.gatherTokensUntil(TokenType::RIGHT_BRACE, true), ::testing::ExitedWithCode(1), ".*");

// }

TEST(ASTTest, parseConstantInt) {
    std::string sourceCode = "5;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    OutputStream* outputStream = new StandardOutputStream;
    Parser parser(tokens, outputStream);

    NumberNode* node = parser.parseConstant();

    EXPECT_EQ((*node).getValue(), "5");
    EXPECT_EQ((*node).getType(), TokenType::INTEGER);
}

TEST(ASTTest, parseConstantFloat) {
    std::string sourceCode = "5.43;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    OutputStream* outputStream = new StandardOutputStream;
    Parser parser(tokens, outputStream);

    NumberNode* node = parser.parseConstant();

    EXPECT_EQ((*node).getValue(), "5.43");
    EXPECT_EQ((*node).getType(), TokenType::FLOAT);
}