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

class Interpreter {
public:
    Interpreter(BlockNode* ast);
    void interpret();
    ~Interpreter();

private:
    BlockNode* ast;

    void interpretBlock(BlockNode* block, std::vector<StackFrame*>& stack);
    void interpretStatement(ASTNode* statement, std::vector<StackFrame*>& stack);
    // void interpretExpression(ASTNode* expression);
    void interpretAssignment(AssignmentNode* assignment, std::vector<StackFrame*>& stack);
    void interpretVariableDeclaration(VariableDeclarationNode* variableDeclaration, std::vector<StackFrame*>& stack);

};