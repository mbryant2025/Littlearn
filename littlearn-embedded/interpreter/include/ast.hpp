#ifndef AST_HPP
#define AST_HPP

#include <vector>
#include <string>
#include <iostream>
#include "tokenizer.hpp"

// Forward declarations of AST node classes
class ASTNode; // virtual base class
class BlockNode; // block node for scope (e.g. function body, if statement body, while loop body)
class VariableDeclarationNode;
class AssignmentNode;
class VariableAccessNode; // for accessing a variable's value
class IntegerLiteralNode;
class FloatLiteralNode;

class Parser {
public:
    Parser(const std::vector<Token>& tokens);

    BlockNode* parseProgram(); // Entry point for parsing a program into an AST

private:
    const std::vector<Token>& tokens;
    size_t currentTokenIndex;

    std::vector<Token> tokensUntil(TokenType type); // gathers tokens until a certain type is reached, inclusive
    ASTNode* parsePrimaryExpression(); // handles literals, identifiers, and parenthesized expressions

    VariableDeclarationNode* parseVariableDeclaration();

    void syntaxError(const std::string& message);
};

// Define a base class for all nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Define a class for housing a block of code
// (e.g. function body, if statement body, while loop body)
// Variables declared in a block are scoped to that block
class BlockNode : public ASTNode {
public:
    BlockNode(const std::vector<ASTNode*>& statements);

    std::vector<ASTNode*> statements;

    ~BlockNode();
};

// Define a class for variable declarations
class VariableDeclarationNode : public ASTNode {
public:
    VariableDeclarationNode(const std::string& identifier, const std::string& type, ASTNode* initializer);

    std::string identifier;
    std::string type;
    ASTNode* initializer;

    ~VariableDeclarationNode();
};

// Define a class for variable assignments
class AssignmentNode : public ASTNode {
public:
    AssignmentNode(const std::string& identifier, ASTNode* expression);

    std::string identifier;
    ASTNode* expression;

    ~AssignmentNode();
};

// Define a class for variable access
class VariableAccessNode : public ASTNode {
public:
    VariableAccessNode(const std::string& identifier);

    std::string identifier;

    ~VariableAccessNode();
};

// Define a class for integer literals
class IntegerLiteralNode : public ASTNode {
public:
    IntegerLiteralNode(int value);

    int value;

    ~IntegerLiteralNode();
};

// Define a class for float literals
class FloatLiteralNode : public ASTNode {
public:
    FloatLiteralNode(float value);

    float value;

    ~FloatLiteralNode();
};

#endif // AST_HPP
