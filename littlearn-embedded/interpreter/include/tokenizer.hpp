#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <vector>

// Define token types
enum class TokenType
{
    KEYWORD,           // ex int, float, if, while, print, wait, print_seven_segment, read_port, write_port
    IDENTIFIER,        // ex variable names
    INTEGER,           // ex 1, 2, 3, 4, 5
    FLOAT,             // ex 1.0, 2.0, 3.0, 4.0, 5.0
    OPERATOR,          // ex +, -, *, /, =, %
    LEFT_BRACE,        // {
    RIGHT_BRACE,       // }
    LEFT_PARENTHESIS,  // (
    RIGHT_PARENTHESIS, // )
    SEMICOLON,         // ;
    COMMA,             // ,
    UNKNOWN
};

// Define a struct to represent tokens
struct Token
{
    TokenType type;
    std::string lexeme;
};

class Tokenizer
{
public:
    Tokenizer(const std::string &sourceCode);
    std::vector<Token> tokenize();

    static std::string tokenTypeToString(TokenType tokenType);

private:
    std::string sourceCode;
    std::size_t currentPosition;

    char peek(); // Peek at the current character
    char peek(int offset); // Peek at the character at the given offset (default: 1)
    char advance(); // Advance to the next character
    bool isAtEnd(); // Check if we're at the end of the source code
    bool match(char expected);
    void skipWhitespace(); 
    void skipComment();

    Token parseToken();
    Token parseKeywordOrIdentifier();
    Token parseNumber(bool isNegative);
    Token parseOperator();
    Token parseUnknown();
};

#endif // TOKENIZER_HPP
