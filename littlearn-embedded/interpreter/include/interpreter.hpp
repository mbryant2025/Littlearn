#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "ast.hpp"
#include <map>
#include <vector>

// Port numbers
#define PORT_1 25
#define PORT_2 33
#define PORT_3 32
#define PORT_4 12
#define PORT_5 26
#define PORT_6 27

// Returnable types
enum class ReturnableType {
    FLOAT,
    INTEGER
};

class StackFrame {

public:
StackFrame(StackFrame* parent);
~StackFrame();

void allocateFloatVariable(std::string name, float value);
void allocateIntVariable(std::string name, int value);

void setFloatVariable(std::string name, float value);
void setIntVariable(std::string name, int value);

float getFloatVariable(std::string name);
int getIntVariable(std::string name);

ReturnableType getType(std::string name);

private:
    std::map<std::string, float> float_variables;
    std::map<std::string, int> int_variables;
    StackFrame* parent;
};

class ReturnableObject {
public:
    virtual ReturnableType getType() = 0;
    virtual ~ReturnableObject() = default;
};

class ReturnableFloat : public ReturnableObject {
public:
    ReturnableFloat(float value);
    ReturnableType getType() override;
    float getValue();
    ~ReturnableFloat();

private:
    float value;
};

class ReturnableInt : public ReturnableObject {
public:
    ReturnableInt(int value);
    ReturnableType getType() override;
    int getValue();
    ~ReturnableInt();

private:
    int value;
};

class Interpreter {
public:
    Interpreter(BlockNode* ast);
    void interpret();
    ~Interpreter();

private:
    BlockNode* ast;

    void interpretBlock(BlockNode* block, std::vector<StackFrame*>& stack);
    void interpretStatement(ASTNode* statement, std::vector<StackFrame*>& stack);
    void interpretAssignment(AssignmentNode* assignment, std::vector<StackFrame*>& stack);
    void interpretVariableDeclaration(VariableDeclarationNode* variableDeclaration, std::vector<StackFrame*>& stack);
    void interpretPrint(ASTNode* expression, std::vector<StackFrame*>& stack);
    void interpretIf(IfNode* ifStatement, std::vector<StackFrame*>& stack);
    void interpretWhile(WhileNode* whileStatement, std::vector<StackFrame*>& stack);
    void interpretWait(ASTNode* expression, std::vector<StackFrame*>& stack);
    void interpretPrintSevenSegment(ASTNode* expression, std::vector<StackFrame*>& stack);
    void interpretWritePort(WritePortNode* writePort, std::vector<StackFrame*>& stack);

    bool interpretTruthiness(ReturnableObject* condition, std::vector<StackFrame*>& stack);

    ReturnableObject* interpretExpression(ASTNode* expression, std::vector<StackFrame*>& stack);
    ReturnableObject* interpretVariableAccess(VariableAccessNode* variableAccess, std::vector<StackFrame*>& stack);
    ReturnableObject* interpretBinaryOperation(BinaryOperationNode* binaryExpression, std::vector<StackFrame*>& stack);
    ReturnableObject* interpretNumber(NumberNode* number, std::vector<StackFrame*>& stack);
    ReturnableObject* interpretReadPort(ASTNode* expression, std::vector<StackFrame*>& stack);

};

#endif // INTERPRETER_HPP