#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "error.hpp"
#include "outputStream.hpp"
#include "tokenizer.hpp"

// enum for AST node types
enum class ASTNodeType {
    BLOCK_NODE,
    VARIABLE_DECLARATION_NODE,
    ASSIGNMENT_NODE,
    VARIABLE_ACCESS_NODE,
    NUMBER_NODE,
    BINARY_OPERATION_NODE,
    IF_NODE,
    WHILE_NODE,
    FOR_NODE,
    BREAK_NODE,
    CONTINUE_NODE,
    FUNCTION_DECLARATION_NODE,
    FUNCTION_CALL_NODE,
    RETURN_NODE,
    ERROR_NODE
};

// Forward declarations of AST node classes
class ASTNode;    // virtual base class
class BlockNode;  // block node for scope (e.g. function body, if statement body, while loop body)
class VariableDeclarationNode;
class AssignmentNode;
class VariableAccessNode;
class NumberNode;
class BinaryOperationNode;
class IfNode;
class WhileNode;
class ForNode;
class BreakNode;
class ContinueNode;
class FunctionDeclarationNode;
class FunctionCallNode;
class ReturnNode;

class Parser {
   public:
    Parser(const std::vector<Token>& tokens, OutputStream& outputStream, ErrorHandler& errorHandler);
    ~Parser();

    BlockNode* parseProgram();  // Entry point for parsing a program into an AST

    // The functions to parse each type of AST node
    BlockNode* parseBlock();
    VariableDeclarationNode* parseVariableDeclaration();
    AssignmentNode* parseAssignment();
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

    ASTNode* parseExpression(const std::vector<const Token*>& expressionTokens);  // Should result in a single AST node for an expression, constant or variable access
    int getPrecedence(const std::string& lexeme);

    void eatToken(TokenType expectedTokenType);

    void syntaxError(const std::string& message) const;

   private:
    const std::vector<Token>& tokens;
    OutputStream& outputStream;
    ErrorHandler& errorHandler;
    size_t currentTokenIndex;
};

// Base class for all nodes
class ASTNode {
   public:
    virtual std::string toString() const = 0; // Should not be called when an error node is encountered
    virtual ~ASTNode() = default;
    virtual ASTNodeType getNodeType() const = 0;
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
    ~BinaryOperationNode();

   private:
    ASTNode* left;
    std::string op;
    ASTNode* right;
};

class IfNode : public ASTNode {
   public:
    IfNode(ASTNode* expression, BlockNode* body);
    IfNode(ASTNode* expression, BlockNode* body, BlockNode* elseBody);
    std::string toString() const override;
    ASTNode* getExpression() const;
    BlockNode* getBody() const;
    BlockNode* getElseBody() const;
    ASTNodeType getNodeType() const override;
    ~IfNode();

   private:
    ASTNode* expression;
    BlockNode* body;
    BlockNode* elseBody;
};

class WhileNode : public ASTNode {
   public:
    WhileNode(ASTNode* expression, BlockNode* body);
    std::string toString() const override;
    ASTNode* getExpression() const;
    BlockNode* getBody() const;
    ASTNodeType getNodeType() const override;
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
    ~BreakNode();
};

class ContinueNode : public ASTNode {
   public:
    ContinueNode();
    std::string toString() const override;
    ASTNodeType getNodeType() const override;
    ~ContinueNode();
};

class FunctionDeclarationNode : public ASTNode {
   public:
    FunctionDeclarationNode(const std::string& name, const std::vector<std::string>& parameters, const std::vector<TokenType>& parameterTypes, BlockNode* body);
    std::string toString() const override;
    std::string getName() const;
    std::vector<std::string> getParameters() const;
    std::vector<TokenType> getParameterTypes() const;
    BlockNode* getBody() const;
    ASTNodeType getNodeType() const override;
    ~FunctionDeclarationNode();

   private:
    std::string name;
    std::vector<std::string> parameters;
    std::vector<TokenType> parameterTypes;
    BlockNode* body;
};

class FunctionCallNode : public ASTNode {
   public:
    FunctionCallNode(const std::string& name, const std::vector<ASTNode*>& arguments);
    std::string toString() const override;
    std::string getName() const;
    std::vector<ASTNode*> getArguments() const;
    ASTNodeType getNodeType() const override;
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
    ~ReturnNode();

   private:
    ASTNode* expression;
};

#endif  // AST_HPP
