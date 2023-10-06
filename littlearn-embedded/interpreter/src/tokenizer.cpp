#include "tokenizer.hpp"
#include <cctype>

Tokenizer::Tokenizer(const std::string &sourceCode)
    : sourceCode(sourceCode), currentPosition(0) {}

char Tokenizer::peek()
{
    if (isAtEnd())
        return '\0';
    return sourceCode[currentPosition];
}

char Tokenizer::peek(int offset)
{
    if (currentPosition + offset >= sourceCode.length())
        return '\0';
    return sourceCode[currentPosition + offset];
}

char Tokenizer::advance()
{
    if (!isAtEnd())
        currentPosition++;
    return sourceCode[currentPosition - 1];
}

bool Tokenizer::isAtEnd()
{
    return currentPosition >= sourceCode.length();
}

bool Tokenizer::match(char expected)
{
    if (isAtEnd())
        return false;
    if (sourceCode[currentPosition] != expected)
        return false;
    currentPosition++;
    return true;
}

void Tokenizer::skipWhitespace()
{
    while (std::isspace(peek()))
        advance();
}

void Tokenizer::skipComment()
{
    if (peek() == '/' && match('/'))
    {
        // Single-line comment
        while (peek() != '\n' && !isAtEnd())
            advance();
    }
    else if (peek() == '/' && match('*'))
    {
        // Multi-line comment
        while (!(peek() == '*' && peek() == '/') && !isAtEnd())
            advance();
        if (!isAtEnd())
        {
            advance(); // Consume '*'
            advance(); // Consume '/'
        }
    }
}

Token Tokenizer::parseToken()
{
    skipWhitespace();
    skipComment();

    if (isAtEnd())
        return {TokenType::UNKNOWN, ""};

    char currentChar = peek();
    if (std::isalpha(currentChar))
        return parseKeywordOrIdentifier();
    if (std::isdigit(currentChar) || currentChar == '.')
        return parseNumber();
    if (currentChar == '+' || currentChar == '-' || currentChar == '*' || currentChar == '/' || currentChar == '=' || currentChar == '>' || currentChar == '<')
        return parseOperator();

    if (currentChar == ';') {
        advance();
        return {TokenType::SEMICOLON, ";"};
    }

    if (currentChar == '(') {
        advance();
        return {TokenType::LEFT_PARENTHESIS, "("};
    }

    if (currentChar == ')') {
        advance();
        return {TokenType::RIGHT_PARENTHESIS, ")"};
    }

    if (currentChar == '{') {
        advance();
        return {TokenType::LEFT_BRACE, "{"};
    }

    if (currentChar == '}') {
        advance();
        return {TokenType::RIGHT_BRACE, "}"};
    }

    advance(); // Consume unrecognized character
    return {TokenType::UNKNOWN, std::string(1, currentChar)};
}

Token Tokenizer::parseKeywordOrIdentifier()
{
    std::string lexeme;
    while (std::isalnum(peek()) || peek() == '_')
    {
        lexeme += advance();
    }

    // Check if it's a keyword
    if (lexeme == "int" || lexeme == "float" || lexeme == "string" ||  lexeme == "if" || lexeme == "while" || lexeme == "print")
    {
        return {TokenType::KEYWORD, lexeme};
    }

    return {TokenType::IDENTIFIER, lexeme};
}

Token Tokenizer::parseNumber()
{
    std::string lexeme;
    while (std::isdigit(peek()))
    {
        lexeme += advance();
    }

    if (peek() == '.' && std::isdigit(peek(1)))
    {
        lexeme += advance(); // Consume '.'
        while (std::isdigit(peek()))
        {
            lexeme += advance();
        }
        return {TokenType::FLOAT, lexeme};
    }

    return {TokenType::INTEGER, lexeme};
}

Token Tokenizer::parseOperator()
{
    std::string lexeme;
    while (peek() == '+' || peek() == '-' || peek() == '*' || peek() == '/' || peek() == '=' || peek() == '>' || peek() == '<')
    {
        lexeme += advance();
    }

    if (peek() == '(' || peek() == ')') {
        lexeme += advance();
        return {TokenType::OPERATOR, lexeme};
    }

    if (peek() == '{' || peek() == '}') {
        lexeme += advance();
        return {TokenType::OPERATOR, lexeme};
    }

    return {TokenType::OPERATOR, lexeme};
}

Token Tokenizer::parseUnknown()
{
    // Consume and return the unknown character
    return {TokenType::UNKNOWN, std::string(1, advance())};
}

std::vector<Token> Tokenizer::tokenize()
{
    std::vector<Token> tokens;

    while (!isAtEnd())
    {
        Token token = parseToken();
        if (token.type != TokenType::UNKNOWN)
        {
            tokens.push_back(token);
        }
    }

    return tokens;
}

std::string Tokenizer::tokenTypeToString(TokenType tokenType)
{
    switch (tokenType)
    {
    case TokenType::KEYWORD:
        return "KEYWORD";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::INTEGER:
        return "INTEGER";
    case TokenType::FLOAT:
        return "FLOAT";
    case TokenType::OPERATOR:
        return "OPERATOR";
    case TokenType::LEFT_BRACE:
        return "LEFT_BRACE";
    case TokenType::RIGHT_BRACE:
        return "RIGHT_BRACE";
    case TokenType::LEFT_PARENTHESIS:
        return "LEFT_PARENTHESIS";
    case TokenType::RIGHT_PARENTHESIS:
        return "RIGHT_PARENTHESIS";
    case TokenType::SEMICOLON:
        return "SEMICOLON";
    case TokenType::UNKNOWN:
        return "UNKNOWN";
    default:
        return "UNKNOWN";
    }
}