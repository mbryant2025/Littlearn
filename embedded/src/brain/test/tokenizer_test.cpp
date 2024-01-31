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

TEST(TokenizerTest, SimpleAssignmentWithFloat) {
    std::string sourceCode = "float sum = 5.0;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    // Check that we have the correct number of tokens
    ASSERT_EQ(tokens.size(), 5);

    // Check that the tokens are correct
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].lexeme, "float");
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "sum");
    EXPECT_EQ(tokens[2].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[2].lexeme, "=");
    EXPECT_EQ(tokens[3].type, TokenType::FLOAT);
    EXPECT_EQ(tokens[3].lexeme, "5.0");
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[4].lexeme, ";");
}

TEST(TokenizerTest, MultipleAssignments) {
    std::string sourceCode = "int sum = 5;\nfloat y = 3;\nfloat z = y + 2.0;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    // Check that we have the correct number of tokens
    ASSERT_EQ(tokens.size(), 17);

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
    EXPECT_EQ(tokens[5].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[5].lexeme, "float");
    EXPECT_EQ(tokens[6].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[6].lexeme, "y");
    EXPECT_EQ(tokens[7].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[7].lexeme, "=");
    EXPECT_EQ(tokens[8].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[8].lexeme, "3");
    EXPECT_EQ(tokens[9].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[9].lexeme, ";");
    EXPECT_EQ(tokens[10].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[10].lexeme, "float");
    EXPECT_EQ(tokens[11].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[11].lexeme, "z");
    EXPECT_EQ(tokens[12].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[12].lexeme, "=");
    EXPECT_EQ(tokens[13].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[13].lexeme, "y");
    EXPECT_EQ(tokens[14].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[14].lexeme, "+");
    EXPECT_EQ(tokens[15].type, TokenType::FLOAT);
    EXPECT_EQ(tokens[15].lexeme, "2.0");
    EXPECT_EQ(tokens[16].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[16].lexeme, ";");
}

TEST(TokenizerTest, WhileLoop) {
    std::string sourceCode = "while (x < 10) {\n    x = x + 1;\n}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    // Check that we have the correct number of tokens
    ASSERT_EQ(tokens.size(), 14);

    // Check that the tokens are correct
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].lexeme, "while");
    EXPECT_EQ(tokens[1].type, TokenType::LEFT_PARENTHESIS);
    EXPECT_EQ(tokens[1].lexeme, "(");
    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].lexeme, "x");
    EXPECT_EQ(tokens[3].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[3].lexeme, "<");
    EXPECT_EQ(tokens[4].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[4].lexeme, "10");
    EXPECT_EQ(tokens[5].type, TokenType::RIGHT_PARENTHESIS);
    EXPECT_EQ(tokens[5].lexeme, ")");
    EXPECT_EQ(tokens[6].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[6].lexeme, "{");
    EXPECT_EQ(tokens[7].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[7].lexeme, "x");
    EXPECT_EQ(tokens[8].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[8].lexeme, "=");
    EXPECT_EQ(tokens[9].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[9].lexeme, "x");
    EXPECT_EQ(tokens[10].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[10].lexeme, "+");
    EXPECT_EQ(tokens[11].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[11].lexeme, "1");
    EXPECT_EQ(tokens[12].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[12].lexeme, ";");
    EXPECT_EQ(tokens[13].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[13].lexeme, "}");
}

TEST(TokenizerTest, TestComment) {
    std::string sourceCode = "int sum = 0; float pi = 3.14; //this is a comment\n while (sum < 5) {sum = sum + 1;}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    // Check that we have the correct number of tokens
    ASSERT_EQ(tokens.size(), 24);

    // Check that the tokens are correct
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].lexeme, "int");
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "sum");
    EXPECT_EQ(tokens[2].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[2].lexeme, "=");
    EXPECT_EQ(tokens[3].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[3].lexeme, "0");
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[4].lexeme, ";");
    EXPECT_EQ(tokens[5].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[5].lexeme, "float");
    EXPECT_EQ(tokens[6].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[6].lexeme, "pi");
    EXPECT_EQ(tokens[7].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[7].lexeme, "=");
    EXPECT_EQ(tokens[8].type, TokenType::FLOAT);
    EXPECT_EQ(tokens[8].lexeme, "3.14");
    EXPECT_EQ(tokens[9].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[9].lexeme, ";");
    EXPECT_EQ(tokens[10].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[10].lexeme, "while");
    EXPECT_EQ(tokens[11].type, TokenType::LEFT_PARENTHESIS);
    EXPECT_EQ(tokens[11].lexeme, "(");
    EXPECT_EQ(tokens[12].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[12].lexeme, "sum");
    EXPECT_EQ(tokens[13].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[13].lexeme, "<");
    EXPECT_EQ(tokens[14].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[14].lexeme, "5");
    EXPECT_EQ(tokens[15].type, TokenType::RIGHT_PARENTHESIS);
    EXPECT_EQ(tokens[15].lexeme, ")");
    EXPECT_EQ(tokens[16].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[16].lexeme, "{");
    EXPECT_EQ(tokens[17].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[17].lexeme, "sum");
    EXPECT_EQ(tokens[18].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[18].lexeme, "=");
    EXPECT_EQ(tokens[19].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[19].lexeme, "sum");
    EXPECT_EQ(tokens[20].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[20].lexeme, "+");
    EXPECT_EQ(tokens[21].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[21].lexeme, "1");
    EXPECT_EQ(tokens[22].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[22].lexeme, ";");
    EXPECT_EQ(tokens[23].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[23].lexeme, "}");
}

TEST(TokenizerTest, NegativeLiteral) {
    std::string sourceCode = "int x = -5;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    // Check that we have the correct number of tokens
    ASSERT_EQ(tokens.size(), 5);

    // Check that the tokens are correct
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].lexeme, "int");
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "x");
    EXPECT_EQ(tokens[2].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[2].lexeme, "=");
    EXPECT_EQ(tokens[3].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[3].lexeme, "-5");
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[4].lexeme, ";");
}

TEST(TokenizerTest, NegativeLiteralSubtraction1) {
    std::string sourceCode = "int x = 10--5;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    // Check that we have the correct number of tokens
    ASSERT_EQ(tokens.size(), 7);

    // Check that the tokens are correct
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].lexeme, "int");
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "x");
    EXPECT_EQ(tokens[2].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[2].lexeme, "=");
    EXPECT_EQ(tokens[3].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[3].lexeme, "10");
    EXPECT_EQ(tokens[4].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[4].lexeme, "-");
    EXPECT_EQ(tokens[5].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[5].lexeme, "-5");
    EXPECT_EQ(tokens[6].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[6].lexeme, ";");
}

TEST(TokenizerTest, NegativeLiteralSubtraction2) {
    std::string sourceCode = "int x = 10+-5;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    // Check that we have the correct number of tokens
    ASSERT_EQ(tokens.size(), 7);

    // Check that the tokens are correct
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].lexeme, "int");
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "x");
    EXPECT_EQ(tokens[2].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[2].lexeme, "=");
    EXPECT_EQ(tokens[3].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[3].lexeme, "10");
    EXPECT_EQ(tokens[4].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[4].lexeme, "+");
    EXPECT_EQ(tokens[5].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[5].lexeme, "-5");
    EXPECT_EQ(tokens[6].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[6].lexeme, ";");
}

TEST(TokenizerTest, NegativeLiteralSubtraction3) {
    std::string sourceCode = "int x = -10+5;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    // Check that we have the correct number of tokens
    ASSERT_EQ(tokens.size(), 7);

    // Check that the tokens are correct
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].lexeme, "int");
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "x");
    EXPECT_EQ(tokens[2].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[2].lexeme, "=");
    EXPECT_EQ(tokens[3].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[3].lexeme, "-10");
    EXPECT_EQ(tokens[4].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[4].lexeme, "+");
    EXPECT_EQ(tokens[5].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[5].lexeme, "5");
    EXPECT_EQ(tokens[6].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[6].lexeme, ";");
}

TEST(TokenizerTest, NegativeLiteralExpression) {
    std::string sourceCode = "int x = 2; if(x-2) {x = -10+5;}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    // Check that we have the correct number of tokens
    ASSERT_EQ(tokens.size(), 19);

    // Check that the tokens are correct
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].lexeme, "int");
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "x");
    EXPECT_EQ(tokens[2].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[2].lexeme, "=");
    EXPECT_EQ(tokens[3].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[3].lexeme, "2");
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[4].lexeme, ";");
    EXPECT_EQ(tokens[5].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[5].lexeme, "if");
    EXPECT_EQ(tokens[6].type, TokenType::LEFT_PARENTHESIS);
    EXPECT_EQ(tokens[6].lexeme, "(");
    EXPECT_EQ(tokens[7].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[7].lexeme, "x");
    EXPECT_EQ(tokens[8].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[8].lexeme, "-");
    EXPECT_EQ(tokens[9].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[9].lexeme, "2");
    EXPECT_EQ(tokens[10].type, TokenType::RIGHT_PARENTHESIS);
    EXPECT_EQ(tokens[10].lexeme, ")");
    EXPECT_EQ(tokens[11].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[11].lexeme, "{");
    EXPECT_EQ(tokens[12].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[12].lexeme, "x");
    EXPECT_EQ(tokens[13].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[13].lexeme, "=");
    EXPECT_EQ(tokens[14].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[14].lexeme, "-10");
    EXPECT_EQ(tokens[15].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[15].lexeme, "+");
    EXPECT_EQ(tokens[16].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[16].lexeme, "5");
    EXPECT_EQ(tokens[17].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[17].lexeme, ";");
    EXPECT_EQ(tokens[18].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[18].lexeme, "}");
}

TEST(TokenizerTest, ParseKeywordOrIdentifier) {
    std::string sourceCode = "int x = 2; if(x-2) {x = -10+5;}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].lexeme, "int");
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].lexeme, "x");
}

TEST(TokenizerTest, ParseNumber) {
    std::string sourceCode = "int x = 123; float y = 45.67;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    EXPECT_EQ(tokens[3].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[3].lexeme, "123");
    EXPECT_EQ(tokens[8].type, TokenType::FLOAT);
    EXPECT_EQ(tokens[8].lexeme, "45.67");
}

TEST(TokenizerTest, ParseOperator) {
    std::string sourceCode = "int x = 2; if(x-2) {x = -10+5;}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    EXPECT_EQ(tokens[2].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[2].lexeme, "=");
    EXPECT_EQ(tokens[8].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[8].lexeme, "-");
    EXPECT_EQ(tokens[15].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[15].lexeme, "+");
}

TEST(TokenizerTest, ParseNegativeNumber) {
    std::string sourceCode = "int x = -123; float y = -45.67;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    EXPECT_EQ(tokens[3].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[3].lexeme, "-123");
    EXPECT_EQ(tokens[8].type, TokenType::FLOAT);
    EXPECT_EQ(tokens[8].lexeme, "-45.67");
}

TEST(TokenizerTest, ParseNegativeNumberAfterFunction) {
    std::string sourceCode = "int x = function_call(54)-3;";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    EXPECT_EQ(tokens[3].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[3].lexeme, "function_call");
    EXPECT_EQ(tokens[4].type, TokenType::LEFT_PARENTHESIS);
    EXPECT_EQ(tokens[4].lexeme, "(");
    EXPECT_EQ(tokens[5].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[5].lexeme, "54");
    EXPECT_EQ(tokens[6].type, TokenType::RIGHT_PARENTHESIS);
    EXPECT_EQ(tokens[6].lexeme, ")");
    EXPECT_EQ(tokens[7].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[7].lexeme, "-");
    EXPECT_EQ(tokens[8].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[8].lexeme, "3");
}

TEST(TokenizerTest, ParseDualCharacterOperators) {
    std::string sourceCode = "if (x >= 2 && y <= 3) {x = x < 1; y = y > 1;}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    EXPECT_EQ(tokens[3].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[3].lexeme, ">=");
    EXPECT_EQ(tokens[5].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[5].lexeme, "&&");
    EXPECT_EQ(tokens[7].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[7].lexeme, "<=");
    EXPECT_EQ(tokens[14].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[14].lexeme, "<");
    EXPECT_EQ(tokens[20].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[20].lexeme, ">");
}

TEST(TokenizerTest, ParseComplexOperatorCombinations) {
    std::string sourceCode = "int x = 2; if((x-2)*3/4 && y || z) {x = -10+5*3/2; y = y > 1 < 2; z = !z;}";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    EXPECT_EQ(tokens[2].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[2].lexeme, "=");
    EXPECT_EQ(tokens[9].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[9].lexeme, "-");
    EXPECT_EQ(tokens[12].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[12].lexeme, "*");
    EXPECT_EQ(tokens[14].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[14].lexeme, "/");
    EXPECT_EQ(tokens[16].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[16].lexeme, "&&");
    EXPECT_EQ(tokens[18].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[18].lexeme, "||");
    EXPECT_EQ(tokens[23].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[23].lexeme, "=");
    EXPECT_EQ(tokens[25].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[25].lexeme, "+");
    EXPECT_EQ(tokens[27].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[27].lexeme, "*");
    EXPECT_EQ(tokens[29].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[29].lexeme, "/");
    EXPECT_EQ(tokens[33].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[33].lexeme, "=");
    EXPECT_EQ(tokens[35].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[35].lexeme, ">");
    EXPECT_EQ(tokens[37].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[37].lexeme, "<");
    EXPECT_EQ(tokens[41].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[41].lexeme, "=");
    EXPECT_EQ(tokens[42].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[42].lexeme, "!");
}

TEST(TokenizerTest, ParseMessyOperatorCombinations) {
    std::string sourceCode = "int x = 2; int z = x * !x + !x > 4 != x + (3 + !!!!!!!!!!!!!!!!!x);";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    EXPECT_EQ(tokens[2].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[2].lexeme, "=");
    EXPECT_EQ(tokens[7].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[7].lexeme, "=");
    EXPECT_EQ(tokens[9].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[9].lexeme, "*");
    EXPECT_EQ(tokens[10].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[10].lexeme, "!");
    EXPECT_EQ(tokens[12].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[12].lexeme, "+");
    EXPECT_EQ(tokens[13].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[13].lexeme, "!");
    EXPECT_EQ(tokens[15].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[15].lexeme, ">");
    EXPECT_EQ(tokens[17].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[17].lexeme, "!=");
    EXPECT_EQ(tokens[19].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[19].lexeme, "+");
    EXPECT_EQ(tokens[22].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[22].lexeme, "+");
    EXPECT_EQ(tokens[23].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[23].lexeme, "!");
    EXPECT_EQ(tokens[24].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[24].lexeme, "!");
    EXPECT_EQ(tokens[25].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[25].lexeme, "!");
    EXPECT_EQ(tokens[26].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[26].lexeme, "!");
    EXPECT_EQ(tokens[27].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[27].lexeme, "!");
    EXPECT_EQ(tokens[28].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[28].lexeme, "!");
    EXPECT_EQ(tokens[29].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[29].lexeme, "!");
    EXPECT_EQ(tokens[30].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[30].lexeme, "!");
    EXPECT_EQ(tokens[31].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[31].lexeme, "!");
    EXPECT_EQ(tokens[32].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[32].lexeme, "!");
    EXPECT_EQ(tokens[33].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[33].lexeme, "!");
    EXPECT_EQ(tokens[34].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[34].lexeme, "!");
    EXPECT_EQ(tokens[35].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[35].lexeme, "!");
    EXPECT_EQ(tokens[36].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[36].lexeme, "!");
    EXPECT_EQ(tokens[37].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[37].lexeme, "!");
    EXPECT_EQ(tokens[38].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[38].lexeme, "!");
    EXPECT_EQ(tokens[39].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[39].lexeme, "!");
}

TEST(TokenizerTest, ParseIfElseStatement) {
    std::string sourceCode = "if (x > 10) { y = 20; } else { y = 30; }";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[0].lexeme, "if");
    EXPECT_EQ(tokens[1].type, TokenType::LEFT_PARENTHESIS);
    EXPECT_EQ(tokens[1].lexeme, "(");
    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].lexeme, "x");
    EXPECT_EQ(tokens[3].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[3].lexeme, ">");
    EXPECT_EQ(tokens[4].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[4].lexeme, "10");
    EXPECT_EQ(tokens[5].type, TokenType::RIGHT_PARENTHESIS);
    EXPECT_EQ(tokens[5].lexeme, ")");
    EXPECT_EQ(tokens[6].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[6].lexeme, "{");
    EXPECT_EQ(tokens[7].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[7].lexeme, "y");
    EXPECT_EQ(tokens[8].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[8].lexeme, "=");
    EXPECT_EQ(tokens[9].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[9].lexeme, "20");
    EXPECT_EQ(tokens[10].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[10].lexeme, ";");
    EXPECT_EQ(tokens[11].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[11].lexeme, "}");
    EXPECT_EQ(tokens[12].type, TokenType::KEYWORD);
    EXPECT_EQ(tokens[12].lexeme, "else");
    EXPECT_EQ(tokens[13].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[13].lexeme, "{");
    EXPECT_EQ(tokens[14].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[14].lexeme, "y");
    EXPECT_EQ(tokens[15].type, TokenType::OPERATOR);
    EXPECT_EQ(tokens[15].lexeme, "=");
    EXPECT_EQ(tokens[16].type, TokenType::INTEGER);
    EXPECT_EQ(tokens[16].lexeme, "30");
    EXPECT_EQ(tokens[17].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[17].lexeme, ";");
    EXPECT_EQ(tokens[18].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[18].lexeme, "}");
}

TEST(TokenizerTest, TestEmptySourceCode) {
    std::string sourceCode = "";
    Tokenizer tokenizer(sourceCode);
    std::vector<Token> tokens = tokenizer.tokenize();

    EXPECT_EQ(tokens.size(), 0);
}
