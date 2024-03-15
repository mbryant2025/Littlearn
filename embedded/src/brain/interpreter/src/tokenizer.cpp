#include "tokenizer.hpp"

#include <cctype>


const std::unordered_set<std::string> Tokenizer::keywords = {
    "int", "float", "string", "if",
    "while", "for", "break", "continue",
    "else", "return", "void"};

const std::unordered_set<std::string> Tokenizer::doubleCharOperators = {
    ">=", "<=", "==", "!=", "&&", "||"};

const std::unordered_set<char> Tokenizer::singleCharOperators = {
    '+', '-', '*', '/', '=', '>', '<', '%', '!'};

Tokenizer::Tokenizer(const std::string& sourceCode)
    : sourceCode(sourceCode), currentPosition(0) {}

char Tokenizer::peek() const {
    if (isAtEnd())
        return '\0';
    return sourceCode[currentPosition];
}

char Tokenizer::peek(int offset) const {
    if (offset < 0) {
        for (int i = currentPosition + offset; i >= 0; --i) {
            if (sourceCode[i] != ' ') 
                return sourceCode[i];
        }
        return '\0';
    }
    if (currentPosition + offset >= sourceCode.length())
        return '\0';
    return sourceCode[currentPosition + offset];
}


char Tokenizer::advance() {
    if (!isAtEnd())
        currentPosition++;
    return sourceCode[currentPosition - 1];
}

bool Tokenizer::isAtEnd() const {
    return currentPosition >= sourceCode.length();
}

bool Tokenizer::match(char expected) {
    if (isAtEnd())
        return false;
    if (sourceCode[currentPosition] != expected)
        return false;
    currentPosition++;
    return true;
}

void Tokenizer::skipWhitespace() {
    while (std::isspace(peek()))
        advance();
}

void Tokenizer::skipComment() {
    if (peek() == '/' && peek(1) == '/') {
        // Single-line comment
        while (peek() != '\n' && !isAtEnd())
            advance();
    }
}

Token Tokenizer::parseToken() {
    skipWhitespace();
    skipComment();

    if (isAtEnd())
        return {TokenType::UNKNOWN, ""};

    char currentChar = peek();
    if (std::isalpha(currentChar))
        return parseKeywordOrIdentifier();

    // Handle negative literals
    // Except when the previous token is a number or decimal point or variable or closing parenthesis
    char previousChar = peek(-1);
    if (currentChar == '-' && (std::isdigit(peek(1)) || (peek(1) == '.' && std::isdigit(peek(2)))) && !std::isdigit(previousChar) && previousChar != '.' && !std::isalpha(previousChar) && previousChar != ')') {
        advance();  // Consume '-'
        return parseNumber(true);
    }

    if (std::isdigit(currentChar) || (currentChar == '.' && std::isdigit(peek(1))))
        return parseNumber(false);

    // Check for operators

    // Handle dual character operators

    if (doubleCharOperators.find(std::string(1, currentChar) + peek(1)) != doubleCharOperators.end())
        return parseOperator();
    if (singleCharOperators.find(currentChar) != singleCharOperators.end())
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

    if (currentChar == ',') {
        advance();
        return {TokenType::COMMA, ","};
    }

    advance();  // Consume unrecognized character
    return {TokenType::UNKNOWN, std::string(1, currentChar)};
}

Token Tokenizer::parseKeywordOrIdentifier() {
    std::string lexeme;
    while (std::isalnum(peek()) || peek() == '_') {
        lexeme += advance();
    }

    // Check if lexeme is a keyword
    if (keywords.find(lexeme) != keywords.end()) {
        return {TokenType::KEYWORD, lexeme};
    }

    return {TokenType::IDENTIFIER, lexeme};
}

Token Tokenizer::parseNumber(bool isNegative) {
    std::string lexeme;
    while (std::isdigit(peek())) {
        lexeme += advance();
    }

    if (peek() == '.' && std::isdigit(peek(1))) {
        lexeme += advance();  // Consume '.'
        while (std::isdigit(peek())) {
            lexeme += advance();
        }
        if (isNegative) {
            return {TokenType::FLOAT, "-" + lexeme};
        }
        return {TokenType::FLOAT, lexeme};
    }

    if (isNegative) {
        return {TokenType::INTEGER, "-" + lexeme};
    }

    return {TokenType::INTEGER, lexeme};
}

Token Tokenizer::parseOperator() {
    std::string lexeme;

    if (doubleCharOperators.find(std::string(1, peek()) + peek(1)) != doubleCharOperators.end()) {
        lexeme += advance();
        lexeme += advance();
    }

    else if (singleCharOperators.find(peek()) != singleCharOperators.end()) {
        lexeme += advance();
    }

    else if (peek() == '(' || peek() == ')') {
        lexeme += advance();
        return {TokenType::OPERATOR, lexeme};
    }

    else if (peek() == '{' || peek() == '}') {
        lexeme += advance();
        return {TokenType::OPERATOR, lexeme};
    }

    // If nothing is in lexeme, return unknown
    if (lexeme == "") {
        return {TokenType::UNKNOWN, lexeme};
    }

    return {TokenType::OPERATOR, lexeme};
}

Token Tokenizer::parseUnknown() {
    // Consume and return the unknown character
    return {TokenType::UNKNOWN, std::string(1, advance())};
}

std::vector<Token> Tokenizer::tokenize() {
    if (sourceCode == "") {
        return {};
    }

    std::vector<Token> tokens;

    while (!isAtEnd()) {
        Token token = parseToken();
        if (token.type != TokenType::UNKNOWN) {
            tokens.push_back(token);
        }
    }

    return tokens;
}

std::string Tokenizer::tokenTypeToString(TokenType tokenType) {
    switch (tokenType) {
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
        case TokenType::COMMA:
            return "COMMA";
        case TokenType::UNKNOWN:
            return "UNKNOWN";
        default:
            return "UNKNOWN";
    }
}