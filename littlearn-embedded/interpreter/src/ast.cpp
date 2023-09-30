#include "ast.hpp"

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), currentTokenIndex(0) {
}

void Parser::syntaxError(const std::string& message) {
    std::cerr << "Syntax error: " << message << std::endl;
}

 // gathers tokens until a certain type is reached
std::vector<Token> Parser::tokensUntil(TokenType type) {
    std::vector<Token> tokensUntil;
    while (tokens[currentTokenIndex].type != type) {
        tokensUntil.push_back(tokens[currentTokenIndex]);
        currentTokenIndex++;
    }

    // Also push back the token of the given type
    tokensUntil.push_back(tokens[currentTokenIndex]);

    return tokensUntil;
}


// Function to parse a primary expression (e.g., literals, identifiers, parentheses)
ASTNode* Parser::parsePrimaryExpression() {
    if (currentTokenIndex >= tokens.size()) {
        // Handle the case where there are no more tokens (end of input)
        // Throw a syntax error or handle it gracefully

        return nullptr;
    }

    const Token& currentToken = tokens[currentTokenIndex];
    
    if (currentToken.type == TokenType::INTEGER) {
        // Handle integer literals
        currentTokenIndex++;
        // Create and return an AST node for the integer literal
        return new IntegerLiteralNode(std::stoi(currentToken.lexeme));
    } else if (currentToken.type == TokenType::FLOAT) {
        // Handle floating-point literals
        currentTokenIndex++;
        // Create and return an AST node for the floating-point literal
        return new FloatLiteralNode(std::stof(currentToken.lexeme));
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        // Handle variable access (identifier)
        std::string identifier = currentToken.lexeme;
        currentTokenIndex++;
        // Create and return an AST node for variable access
        return new VariableAccessNode(identifier);
    } else if (currentToken.type == TokenType::LEFT_PARENTHESIS) {
        // Handle parentheses for grouping expressions
        currentTokenIndex++;
        // Recursively parse the expression within the parentheses
        ASTNode* expression = parseExpression();
        
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::RIGHT_PARENTHESIS) {
            currentTokenIndex++; // Consume the closing parenthesis
            // Return the parsed expression within parentheses
            return expression;
        } else {
            // Handle a missing closing parenthesis (report an error or handle it gracefully)
            syntaxError("Missing closing parenthesis");
            return nullptr;
        }
    } else {
        // Handle other unexpected tokens or report an error
        return nullptr;
    }
}


// For when we have detected a keyword such as int or float
// In this case, we should create a new VariableDeclarationNode, with immediate access to the type and identifier
// The initializer will be used to create a new node, and this will be everything until the semicolon
VariableDeclarationNode* Parser::parseVariableDeclaration() {

    // Ex. int x = 5;
    // Ex. int y = 5 + x;

    // Get the type
    // Should only be called when the current token is a keyword, specifically of a data type
    std::string type = tokens[currentTokenIndex].lexeme;
    currentTokenIndex++;

    // Get the identifier
    std::string identifier = tokens[currentTokenIndex].lexeme;
    currentTokenIndex++;

    // Skip the equals sign
    currentTokenIndex++;

    // Get the initializer
    std::vector<Token> initializer = tokensUntil(TokenType::SEMICOLON);

    // Skip the semicolon
    currentTokenIndex++;

    // Now handle intializer
    // Want to go from vector of tokens to ASTNode*
    // This will be done recursively
    //TODO

    ASTNode* initializerNode = nullptr;


    return new VariableDeclarationNode(identifier, type, initializerNode);
}

// Function to parse a block of code
BlockNode* Parser::parseProgram() {
    std::vector<ASTNode*> statements;

    while (currentTokenIndex < tokens.size()) {

        switch (tokens[currentTokenIndex].type) {
            case TokenType::KEYWORD:
                // Only handle ints and floats for now
                if (tokens[currentTokenIndex].lexeme == "int" || tokens[currentTokenIndex].lexeme == "float") {
                    VariableDeclarationNode* declaration = parseVariableDeclaration();
                    statements.push_back(declaration);
                }
                else if (tokens[currentTokenIndex].lexeme == "if") {
                    // IfNode* ifStatement = parseIfStatement();
                    // statements.push_back(ifStatement);
                }
                else if (tokens[currentTokenIndex].lexeme == "while") {
                    // WhileNode* whileLoop = parseWhileLoop();
                    // statements.push_back(whileLoop);
                }
                break;
            case TokenType::IDENTIFIER:
                // AssignmentNode* assignment = parseAssignment();
                // statements.push_back(assignment);
                break;
            case TokenType::LEFT_BRACE:
                // BlockNode* block = parseBlock();
                // statements.push_back(block);
                break;
            default:
                // Throw an error
                break;
        }


    }

    // Once you've parsed all statements in the program, create a BlockNode to represent the program's block
    return new BlockNode(statements);
}

// Implement other parsing functions as needed for variable declarations, assignments, expressions, etc.

// Define destructors for AST node classes
// e.g., BlockNode destructor deletes statements, VariableDeclarationNode deletes initializer, etc.

// Destructor for BlockNode
BlockNode::~BlockNode() {
    for (ASTNode* statement : statements) {
        delete statement;
    }
}

// Destructor for VariableDeclarationNode
VariableDeclarationNode::~VariableDeclarationNode() {
    delete initializer;
}

// Destructor for AssignmentNode
AssignmentNode::~AssignmentNode() {
    delete expression;
}

FloatLiteralNode::FloatLiteralNode(float value) : value(value) {
}

IntegerLiteralNode::IntegerLiteralNode(int value) : value(value) {
}

FloatLiteralNode::~FloatLiteralNode() {
}

IntegerLiteralNode::~IntegerLiteralNode() {
}