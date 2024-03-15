#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "error.hpp"
#include "outputStream.hpp"
#include "tokenizer.hpp"

/**
 * @brief Enum for the different types of AST nodes
 * 
 */
enum class ASTNodeType {
    BLOCK_NODE,
    VARIABLE_DECLARATION_NODE,
    ASSIGNMENT_NODE,
    VARIABLE_ACCESS_NODE,
    NUMBER_NODE,
    BINARY_OPERATION_NODE,
    MONO_OPERATION_NODE,
    IF_NODE,
    WHILE_NODE,
    FOR_NODE,
    BREAK_NODE,
    CONTINUE_NODE,
    FUNCTION_DECLARATION_NODE,
    FUNCTION_CALL_NODE,
    RETURN_NODE,
    EMPTY_EXPRESSION_NODE
};

// Forward declarations of AST node classes

/**
 * @brief Virtual base class for all AST nodes
 * 
 */
class ASTNode;

/**
 * @brief Block node for scope (e.g. function body, if statement body, while loop body)
 * 
 * Ex. { ... }
 * 
 */
class BlockNode;

/**
 * @brief Node for variable declaration, including type, identifier, and initializer
 * 
 * Ex. int x = 5;
 * 
 */
class VariableDeclarationNode;


class AssignmentNode;
class VariableAccessNode;
class NumberNode;
class BinaryOperationNode;
class MonoOperationNode;
class IfNode;
class WhileNode;
class ForNode;
class BreakNode;
class ContinueNode;
class FunctionDeclarationNode;
class FunctionCallNode;
class ReturnNode;
class EmptyExpressionNode;

class Parser {
   public:
    Parser(const std::vector<Token>& tokens, OutputStream& outputStream, ErrorHandler& errorHandler);
    ~Parser();

    BlockNode* parseProgram();  // Entry point for parsing a program into an AST

    // The functions to parse each type of AST node
    BlockNode* parseBlock();
    VariableDeclarationNode* parseVariableDeclaration();
    AssignmentNode* parseAssignment(TokenType terminator); // terminator is the token that terminates the expression (e.g. semicolon in most use cases)
    VariableAccessNode* parseVariableAccess();
    NumberNode* parseConstant();
    IfNode* parseIfStatement();
    WhileNode* parseWhile();
    ForNode* parseFor();
    BreakNode* parseBreak();
    ContinueNode* parseContinue();
    FunctionDeclarationNode* parseFunctionDeclaration();
    FunctionCallNode* parseFunctionCall();
    ReturnNode* parseReturn();

    // Helper functions

    // Gather tokens until a token of the specified type is encountered
    // Does not consume the token of the specified type
    // Is used to gather tokens for expressions (we need all tokens in order to perform order of operations)
    std::vector<const Token*> gatherTokensUntil(TokenType endTokenType);

    ASTNode* parseExpression(const std::vector<const Token*>& expressionTokens, bool canBeEmpty);  // Should result in a single AST node for an expression, constant or variable access
    int getPrecedence(const std::string& lexeme);

    void eatToken(TokenType expectedTokenType);

    void syntaxError(const std::string& message) const;

   private:

    std::string genNewIdentifier(); // Generate a new identifier for use in obfuscation or optimization

    const std::vector<Token>& tokens;
    OutputStream& outputStream;
    ErrorHandler& errorHandler;
    size_t currentTokenIndex;
    std::vector<std::string> userIdentifiers; // Identifiers that have been declared -- should not be used when optimizing/obfuscating
};

// Base class for all nodes
class ASTNode {
   public:
    virtual std::string toString() const = 0; // Should not be called when an error node is encountered
    virtual ~ASTNode() = default;
    virtual ASTNodeType getNodeType() const = 0;
    virtual void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) = 0;
};

// Define a class for housing a block of code
// (e.g. function body, if statement body, while loop body)
// Variables declared in a block are scoped to that block
class BlockNode : public ASTNode {
   public:
    BlockNode(const std::vector<ASTNode*>& statements);
    std::string toString() const override;
    std::vector<ASTNode*> getStatements() const;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~BlockNode();

   private:
    std::vector<ASTNode*> statements;
};

class VariableDeclarationNode : public ASTNode {
   public:
    VariableDeclarationNode(const std::string& identifier, const std::string& type, ASTNode* initializer);
    std::string toString() const override;
    std::string getIdentifier() const;
    std::string getType() const;
    ASTNode* getInitializer() const;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~VariableDeclarationNode();

   private:
    std::string identifier;
    std::string type;
    ASTNode* initializer;
};

class AssignmentNode : public ASTNode {
   public:
    AssignmentNode(const std::string& identifier, ASTNode* expression);
    ASTNodeType getNodeType() const override;
    std::string toString() const override;
    std::string getIdentifier() const;
    ASTNode* getExpression() const;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~AssignmentNode();

   private:
    std::string identifier;
    ASTNode* expression;
};

class VariableAccessNode : public ASTNode {
   public:
    VariableAccessNode(const std::string& identifier);
    std::string toString() const override;
    std::string getIdentifier() const;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~VariableAccessNode();

   private:
    std::string identifier;
};

class NumberNode : public ASTNode {
   public:
    NumberNode(std::string val, TokenType type);
    std::string toString() const override;
    ASTNodeType getNodeType() const override;
    TokenType getType() const;
    std::string getValue() const;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~NumberNode();

   private:
    std::string value;
    TokenType type;
};

class BinaryOperationNode : public ASTNode {
   public:
    BinaryOperationNode(ASTNode* left, std::string op, ASTNode* right);
    std::string toString() const override;
    ASTNode* getLeftExpression() const;
    ASTNode* getRightExpression() const;
    std::string getOperator() const;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~BinaryOperationNode();

   private:
    ASTNode* left;
    std::string op;
    ASTNode* right;
};

class MonoOperationNode : public ASTNode {
   public:
    MonoOperationNode(std::string op, ASTNode* expression);
    std::string toString() const override;
    std::string getOperator() const;
    ASTNode* getExpression() const;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~MonoOperationNode();

   private:
    std::string op;
    ASTNode* expression;
};

class IfNode : public ASTNode {
   public:
    IfNode(std::vector<ASTNode*> expressions, std::vector<BlockNode*> bodies);
    std::string toString() const override;
    std::vector<ASTNode*> getExpressions() const;
    std::vector<BlockNode*> getBodies() const;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~IfNode();

   private:
    std::vector<ASTNode*> expressions; // expressions.size() == bodies.size() - 1 if there is an else clause
    std::vector<BlockNode*> bodies;
};

class WhileNode : public ASTNode {
   public:
    WhileNode(ASTNode* expression, BlockNode* body);
    std::string toString() const override;
    ASTNode* getExpression() const;
    BlockNode* getBody() const;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~WhileNode();

   private:
    ASTNode* expression;
    BlockNode* body;
};

class ForNode : public ASTNode {
   public:
    ForNode(ASTNode* initializer, ASTNode* condition, ASTNode* increment, BlockNode* body);
    std::string toString() const override;
    ASTNode* getInitializer() const;
    ASTNode* getCondition() const;
    ASTNode* getIncrement() const;
    BlockNode* getBody() const;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~ForNode();

   private:
    ASTNode* initializer;
    ASTNode* condition;
    ASTNode* increment;
    BlockNode* body;
};

class BreakNode : public ASTNode {
   public:
    BreakNode();
    std::string toString() const override;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~BreakNode();
};

class ContinueNode : public ASTNode {
   public:
    ContinueNode();
    std::string toString() const override;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~ContinueNode();
};

class FunctionDeclarationNode : public ASTNode {
   public:
    FunctionDeclarationNode(const std::string& type, const std::string& name, const std::vector<std::string>& parameters, const std::vector<std::string>& parameterTypes, BlockNode* body);
    std::string toString() const override;
    std::string getType() const;
    std::string getName() const;
    std::vector<std::string> getParameters() const;
    std::vector<std::string> getParameterTypes() const;
    BlockNode* getBody() const;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~FunctionDeclarationNode();

   private:
    std::string type;
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::string> parameterTypes;
    BlockNode* body;
};

class FunctionCallNode : public ASTNode {
   public:
    FunctionCallNode(const std::string& name, const std::vector<ASTNode*>& arguments);
    std::string toString() const override;
    std::string getName() const;
    std::vector<ASTNode*> getArguments() const;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~FunctionCallNode();

   private:
    std::string name;
    std::vector<ASTNode*> arguments;
};

class ReturnNode : public ASTNode {
   public:
    ReturnNode(ASTNode* expression);
    ReturnNode();
    std::string toString() const override;
    ASTNode* getExpression() const;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~ReturnNode();

   private:
    ASTNode* expression;
};

class EmptyExpressionNode : public ASTNode {
   public:
    EmptyExpressionNode();
    std::string toString() const override;
    ASTNodeType getNodeType() const override;
    void replaceIdentifier(const std::string& oldIdentifier, const std::string& newIdentifier) override;
    ~EmptyExpressionNode();
};

#endif  // AST_HPP
