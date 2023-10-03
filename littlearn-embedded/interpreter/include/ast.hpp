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
class VariableAccessNode;
class IntegerLiteralNode;
class FloatLiteralNode;
class AdditionNode;
class SubtractionNode;
class MultiplicationNode;
class DivisionNode;

class Parser {
public:
    Parser(const std::vector<Token>& tokens);

    BlockNode* parseProgram(); // Entry point for parsing a program into an AST

    // Making these public to make testing easier

    // The functions to parse each type of AST node
    BlockNode* parseBlock();
    VariableDeclarationNode* parseVariableDeclaration();
    AssignmentNode* parseAssignment();
    VariableAccessNode* parseVariableAccess();
    IntegerLiteralNode* parseIntegerLiteral();
    FloatLiteralNode* parseFloatLiteral();
    AdditionNode* parseAddition();
    SubtractionNode* parseSubtraction();
    MultiplicationNode* parseMultiplication();
    DivisionNode* parseDivision();

    std::vector<const Token*> gatherTokensUntil(TokenType endTokenType, bool expected);

    ASTNode* parseStatement();
    ASTNode* parseExpression(); // Should result in a single AST node for a constant or variable access
    ASTNode* parseParenthesizedExpression();
    ASTNode* parseConstant();
    ASTNode* parseBinaryExpression();

    void eatToken(TokenType expectedTokenType);

    void syntaxError(const std::string& message);

    
private:
    const std::vector<Token>& tokens;
    size_t currentTokenIndex;

    
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

    ~BlockNode();

private:
    std::vector<ASTNode*> statements;

};

// Define a class for variable declarations
class VariableDeclarationNode : public ASTNode {
public:
    VariableDeclarationNode(const std::string& identifier, const std::string& type, ASTNode* initializer);

    ~VariableDeclarationNode();

private:
    std::string identifier;
    std::string type;
    ASTNode* initializer;

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

    ~VariableAccessNode();

private:
    std::string identifier;

};

// Define a class for integer literals
class IntegerLiteralNode : public ASTNode {
public:
    IntegerLiteralNode(int value);

    ~IntegerLiteralNode();

private:
    int value;

};

// Define a class for float literals
class FloatLiteralNode : public ASTNode {
public:
    FloatLiteralNode(float value);

    ~FloatLiteralNode();

private:
    float value;

};

class AdditionNode : public ASTNode {
public:
    AdditionNode(ASTNode* left, ASTNode* right);

    ~AdditionNode();

private:
    ASTNode* left;
    ASTNode* right;

};

class SubtractionNode : public ASTNode {
public:
    SubtractionNode(ASTNode* left, ASTNode* right);

    ~SubtractionNode();

private:
    ASTNode* left;
    ASTNode* right;

};

class MultiplicationNode : public ASTNode {
public:
    MultiplicationNode(ASTNode* left, ASTNode* right);

    ~MultiplicationNode();

private:
    ASTNode* left;
    ASTNode* right;

};

class DivisionNode : public ASTNode {
public:
    DivisionNode(ASTNode* left, ASTNode* right);

    ~DivisionNode();

private:
    ASTNode* left;
    ASTNode* right;

};

#endif // AST_HPP
