// tokenizer_test.cpp
#include <gtest/gtest.h>
#include "tokenizer.hpp"


TEST(TokenizerTest, SimpleAssignment) {
    std::string sourceCode = "int sum = 5;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    // Check that we have the correct number of tokens
    ASSERT_EQ(tokens.size(), 5);

    // Check that the tokens are correct
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].lexeme, "int");
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "sum");
    EXPECT_EQ(tokens[2].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[2].lexeme, "=");
    EXPECT_EQ(tokens[3].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[3].lexeme, "5");
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[4].lexeme, ";");
}
