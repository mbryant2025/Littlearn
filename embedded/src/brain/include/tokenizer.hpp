#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <unordered_set>
#include <vector>

/**
 * @brief Enum for token types
 * 
 * Used within the tokenizer to represent the type of a token, but also in AST and execution to represent the type of a node
 * 
 */
enum class TokenType {
    KEYWORD,            // ex int, float, if, while, for, break, continue, else, return, void
    IDENTIFIER,         // ex variable names
    INTEGER,            // ex 1, 2, 3, 4, 5
    FLOAT,              // ex 1.0, 2.0, 3.0, 4.0, 5.6
    OPERATOR,           // +, -, *, /, =, %, !, &&, ||, ==, !=, >=, <=
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_PARENTHESIS,   // (
    RIGHT_PARENTHESIS,  // )
    SEMICOLON,          // ;
    COMMA,              // ,
    UNKNOWN
};

/**
 * @brief Struct for a token
 * 
 * Contains the type of the token and the lexeme (the actual string that the token represents)
 * 
 * @param type The type of the token
 * @param lexeme The lexeme of the token
 * 
 */
struct Token {
    TokenType type;
    std::string lexeme;
};

/**
 * @brief Tokenizer object to convert source code into tokens
 * 
 * The tokenizer takes in a string of source code and tokenizes it into a vector of tokens
 * 
 * @param sourceCode The source code to tokenize
 * 
 */
class Tokenizer {

   public:
    Tokenizer(const std::string &sourceCode);
    std::vector<Token> tokenize();

    static std::string tokenTypeToString(TokenType tokenType);
    static const std::unordered_set<std::string> keywords;
    static const std::unordered_set<std::string> doubleCharOperators;
    static const std::unordered_set<char> singleCharOperators;

   private:
    std::string sourceCode;
    std::size_t currentPosition;

    char peek();            // Peek at the current character
    char peek(int offset);  // Peek at the character at the given offset (default: 1)
    char advance();         // Advance to the next character
    bool isAtEnd();         // Check if we're at the end of the source code
    bool match(char expected);
    void skipWhitespace();
    void skipComment();

    Token parseToken();
    Token parseKeywordOrIdentifier();
    Token parseNumber(bool isNegative);
    Token parseOperator();
    Token parseUnknown();
};

#endif  // TOKENIZER_HPP
