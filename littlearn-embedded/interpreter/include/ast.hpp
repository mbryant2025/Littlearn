#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <string>
#include <vector>

#include "tokenizer.hpp"
#include "outputStream.hpp"
#include "error.hpp"


// enum for AST node types
enum class ASTNodeType {
    BLOCK_NODE,
    VARIABLE_DECLARATION_NODE,
    ASSIGNMENT_NODE,
    VARIABLE_ACCESS_NODE,
    NUMBER_NODE,
    BINARY_OPERATION_NODE,
    IF_NODE,
    PRINT_NODE,
    WHILE_NODE,
    WAIT_NODE,
    SEVEN_SEGMENT_NODE,
    READ_PORT_NODE,
    WRITE_PORT_NODE,
    BREAK_NODE,
    CONTINUE_NODE
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
class PrintNode;
class WhileNode;
class WaitNode;
class SevenSegmentNode;
class ReadPortNode;
class WritePortNode;
class BreakNode;
class ContinueNode;

class Parser {
   public:
    Parser(const std::vector<Token>& tokens, OutputStream* outputStream);

    BlockNode* parseProgram();  // Entry point for parsing a program into an AST

    // Making these public to make testing easier

    // The functions to parse each type of AST node
    BlockNode* parseBlock();
    VariableDeclarationNode* parseVariableDeclaration();
    AssignmentNode* parseAssignment();
    VariableAccessNode* parseVariableAccess();
    NumberNode* parseConstant();
    IfNode* parseIfStatement();
    PrintNode* parsePrint();
    WhileNode* parseWhile();
    WaitNode* parseWait();
    SevenSegmentNode* parseSevenSegment();
    ReadPortNode* parseReadPort();
    WritePortNode* parseWritePort();
    BreakNode* parseBreak();
    ContinueNode* parseContinue();

    std::vector<const Token*> gatherTokensUntil(TokenType endTokenType, bool advanceIndex);

    bool isFunctionHeader(std::string lexeme);
    ASTNode* createFunctionCallNode(std::string name, std::vector<ASTNode*> functionArguments);

    ASTNode* parseExpression(std::vector<const Token*> expressionTokens);  // Should result in a single AST node for an expression, constant or variable access
    int getPrecedence(std::string lexeme);

    void eatToken(TokenType expectedTokenType);

    void syntaxError(const std::string& message);

   private:
    const std::vector<Token>& tokens;
    size_t currentTokenIndex;
    ErrorHandler* errorHandler;
};

// Define a base class for all nodes
class ASTNode {
   public:
    virtual std::string toString() const = 0;
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

// Define a class for variable declarations
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

// Define a class for variable assignments
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

// Define a class for variable access
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

// Define a class for integer literals
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

// Define a class for binary operations
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
    std::string toString() const override;
    ASTNode* getExpression() const;
    BlockNode* getBody() const;
    ASTNodeType getNodeType() const override;
    ~IfNode();

   private:
    ASTNode* expression;
    BlockNode* body;
};

class PrintNode : public ASTNode {
   public:
    PrintNode(ASTNode* expression);
    std::string toString() const override;
    ASTNodeType getNodeType() const override;
    ASTNode* getExpression() const;
    ~PrintNode();

   private:
    ASTNode* expression;
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

class WaitNode : public ASTNode {
   public:
    WaitNode(ASTNode* expression);
    std::string toString() const override;
    ASTNode* getExpression() const;
    ASTNodeType getNodeType() const override;
    ~WaitNode();

   private:
    ASTNode* expression;
};

class SevenSegmentNode : public ASTNode {
   public:
    SevenSegmentNode(ASTNode* expression);
    std::string toString() const override;
    ASTNode* getExpression() const;
    ASTNodeType getNodeType() const override;
    ~SevenSegmentNode();

   private:
    ASTNode* expression;
};

class ReadPortNode : public ASTNode {
   public:
    ReadPortNode(ASTNode* expression);
    std::string toString() const override;
    ASTNode* getExpression() const;
    ASTNodeType getNodeType() const override;
    ~ReadPortNode();

   private:
    ASTNode* expression;
};

class WritePortNode : public ASTNode {
   public:
    WritePortNode(ASTNode* port, ASTNode* value);
    std::string toString() const override;
    ASTNode* getPort() const;
    ASTNode* getValue() const;
    ASTNodeType getNodeType() const override;
    ~WritePortNode();

   private:
    ASTNode* port;
    ASTNode* value;
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

#endif  // AST_HPP
