#include "ast.hpp"
#include <map>
#include <vector>

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

std::string getType(std::string name);

private:
    std::map<std::string, float> float_variables;
    std::map<std::string, int> int_variables;
    StackFrame* parent;
};

class ReturnableObject {
public:
    virtual std::string getType() = 0;
    virtual ~ReturnableObject() = default;
};

class ReturnableFloat : public ReturnableObject {
public:
    ReturnableFloat(float value);
    std::string getType() override;
    float getValue();
    ~ReturnableFloat();

private:
    float value;
};

class ReturnableInt : public ReturnableObject {
public:
    ReturnableInt(int value);
    std::string getType() override;
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

    bool interpretTruthiness(ReturnableObject* condition, std::vector<StackFrame*>& stack);

    ReturnableObject* interpretExpression(ASTNode* expression, std::vector<StackFrame*>& stack);
    ReturnableObject* interpretVariableAccess(VariableAccessNode* variableAccess, std::vector<StackFrame*>& stack);
    ReturnableObject* interpretBinaryOperation(BinaryOperationNode* binaryExpression, std::vector<StackFrame*>& stack);
    ReturnableObject* interpretNumber(NumberNode* number, std::vector<StackFrame*>& stack);

};