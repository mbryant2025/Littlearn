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
class NumberNode;
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
    NumberNode* parseConstant();
    AdditionNode* parseAddition();
    SubtractionNode* parseSubtraction();
    MultiplicationNode* parseMultiplication();
    DivisionNode* parseDivision();

    std::vector<const Token*> gatherTokensUntil(TokenType endTokenType, bool advanceIndex);

    ASTNode* parseStatement();
    ASTNode* parseExpression(std::vector<const Token*> expressionTokens); // Should result in a single AST node for an expression, constant or variable access
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
    virtual std::string toString() const = 0;
    virtual ~ASTNode() = default;
};

// Define a class for housing a block of code
// (e.g. function body, if statement body, while loop body)
// Variables declared in a block are scoped to that block
class BlockNode : public ASTNode {
public:
    BlockNode(const std::vector<ASTNode*>& statements);

    std::string toString() const override;

    ~BlockNode();

private:
    std::vector<ASTNode*> statements;

};

// Define a class for variable declarations
class VariableDeclarationNode : public ASTNode {
public:
    VariableDeclarationNode(const std::string& identifier, const std::string& type, ASTNode* initializer);

    std::string toString() const override;

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

    std::string toString() const override;

    std::string identifier;
    ASTNode* expression;

    ~AssignmentNode();
};

// Define a class for variable access
class VariableAccessNode : public ASTNode {
public:
    VariableAccessNode(const std::string& identifier);

    std::string toString() const override;

    ~VariableAccessNode();

private:
    std::string identifier;

};

// Define a class for integer literals
class NumberNode : public ASTNode {
public:
    NumberNode(std::string val, TokenType type);

    std::string toString() const override;

    ~NumberNode();

    TokenType getType() const;

    std::string getValue() const;

private:
    std::string value;
    TokenType type;

};


class AdditionNode : public ASTNode {
public:
    AdditionNode(ASTNode* left, ASTNode* right);

    std::string toString() const override;

    ~AdditionNode();

private:
    ASTNode* left;
    ASTNode* right;

};

class SubtractionNode : public ASTNode {
public:
    SubtractionNode(ASTNode* left, ASTNode* right);

    std::string toString() const override;

    ~SubtractionNode();

private:
    ASTNode* left;
    ASTNode* right;

};

class MultiplicationNode : public ASTNode {
public:
    MultiplicationNode(ASTNode* left, ASTNode* right);

    std::string toString() const override;

    ~MultiplicationNode();

private:
    ASTNode* left;
    ASTNode* right;

};

class DivisionNode : public ASTNode {
public:
    DivisionNode(ASTNode* left, ASTNode* right);

    std::string toString() const override;

    ~DivisionNode();

private:
    ASTNode* left;
    ASTNode* right;

};

#endif // AST_HPP
