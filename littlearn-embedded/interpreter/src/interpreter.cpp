#include "interpreter.hpp"


StackFrame::StackFrame(StackFrame* parent) : parent(parent) {}

StackFrame::~StackFrame() {}

void StackFrame::allocateFloatVariable(std::string name, float value) {

    // Check if the variable already exists in either the int or float maps
    if (float_variables.find(name) != float_variables.end() || int_variables.find(name) != int_variables.end()) {
        throw std::runtime_error("Variable " + name + " already exists in this scope");
    }

    float_variables[name] = value;
}

void StackFrame::allocateIntVariable(std::string name, int value) {

    // Check if the variable already exists in either the int or float maps
    if (float_variables.find(name) != float_variables.end() || int_variables.find(name) != int_variables.end()) {
        throw std::runtime_error("Variable " + name + " already exists in this scope");
    }

    int_variables[name] = value;
}

void StackFrame::setFloatVariable(std::string name, float value) {

    // Check if the variable exists in the float map
    if (float_variables.find(name) != float_variables.end()) {
        float_variables[name] = value;
    } else if (parent != nullptr) {
        parent->setFloatVariable(name, value);
    } else {
        throw std::runtime_error("Variable " + name + " does not exist in this scope");
    }
}

void StackFrame::setIntVariable(std::string name, int value) {

    // Check if the variable exists in the int map
    if (int_variables.find(name) != int_variables.end()) {
        int_variables[name] = value;
    } else if (parent != nullptr) {
        parent->setIntVariable(name, value);
    } else {
        throw std::runtime_error("Variable " + name + " does not exist in this scope");
    }
}

float StackFrame::getFloatVariable(std::string name) {

    // Check if the variable exists in the float map
    if (float_variables.find(name) != float_variables.end()) {
        return float_variables[name];
    } else if (parent != nullptr) {
        return parent->getFloatVariable(name);
    } else {
        throw std::runtime_error("Variable " + name + " does not exist in this scope");
    }
}

int StackFrame::getIntVariable(std::string name) {

    // Check if the variable exists in the int map
    if (int_variables.find(name) != int_variables.end()) {
        return int_variables[name];
    } else if (parent != nullptr) {
        return parent->getIntVariable(name);
    } else {
        throw std::runtime_error("Variable " + name + " does not exist in this scope");
    }
}

std::string StackFrame::getType(std::string name) {

    // Check if the variable exists in the float map
    if (float_variables.find(name) != float_variables.end()) {
        return "float";
    } else if (int_variables.find(name) != int_variables.end()) {
        return "int";
    } else if (parent != nullptr) {
        return parent->getType(name);
    } else {
        throw std::runtime_error("Variable " + name + " does not exist in this scope");
    }
}

Interpreter::Interpreter(BlockNode* ast) : ast(ast) {}

void Interpreter::interpret() {
    
    // Create a stack frame for the global scope
    StackFrame* globalScope = new StackFrame(nullptr);

    // Create a vector of stack frames
    std::vector<StackFrame*> stack;
    stack.push_back(globalScope);

    interpretBlock(ast, stack);
}

Interpreter::~Interpreter() {}

void Interpreter::interpretBlock(BlockNode* block, std::vector<StackFrame*>& stack) {

    // Create a new stack frame
    StackFrame* frame = new StackFrame(stack.back());

    // Push the new stack frame onto the stack
    stack.push_back(frame);

    // Interpret each statement in the block
    for (ASTNode* statement : (*block).getStatements()) { // TODO bad_alloc
        interpretStatement(statement, stack);
    }

    // Pop the stack frame off the stack
    stack.pop_back();

    // Delete the stack frame
    delete frame;
}

void Interpreter::interpretStatement(ASTNode* statement, std::vector<StackFrame*>& stack) {

    // Check if the statement is a variable declaration
    if (dynamic_cast<VariableDeclarationNode*>(statement) != nullptr) {
        interpretVariableDeclaration(dynamic_cast<VariableDeclarationNode*>(statement), stack);
    } else if (dynamic_cast<AssignmentNode*>(statement) != nullptr) {
        // interpretAssignment(dynamic_cast<AssignmentNode*>(statement), stack);
    } else if (dynamic_cast<BlockNode*>(statement) != nullptr) {
        interpretBlock(dynamic_cast<BlockNode*>(statement), stack);
    } else if (dynamic_cast<PrintNode*>(statement) != nullptr) {
        interpretPrint(dynamic_cast<PrintNode*>(statement), stack);
    // } else if (dynamic_cast<IfStatementNode*>(statement) != nullptr) {
        // interpretIfStatement(dynamic_cast<IfStatementNode*>(statement), stack);
    } else {
        throw std::runtime_error("Unknown statement type " + statement->toString());
    }
}

void Interpreter::interpretVariableDeclaration(VariableDeclarationNode* variableDeclaration, std::vector<StackFrame*>& stack) {

    // TODO evaluate the initializer expression

    // Check if the variable is an int
    if (variableDeclaration->getType() == "int") {
        // Allocate an int variable in the current stack frame
        stack.back()->allocateIntVariable(variableDeclaration->getIdentifier(), 0);
    } else if (variableDeclaration->getType() == "float") {
        // Allocate a float variable in the current stack frame
        stack.back()->allocateFloatVariable(variableDeclaration->getIdentifier(), 0.0);
    } else {
        throw std::runtime_error("Unknown variable type " + variableDeclaration->getType());
    }
}

void Interpreter::interpretAssignment(AssignmentNode* assignment, std::vector<StackFrame*>& stack) {

    // TODO evaluate the expression

    std::string type = stack.back()->getType(assignment->getIdentifier());

    if (type == "int") {
        // Set the int variable
        stack.back()->setIntVariable(assignment->getIdentifier(), (int)0);
    } else if (type == "float") {
        // Set the float variable
        stack.back()->setFloatVariable(assignment->getIdentifier(), (float)0.0);
    } else {
        throw std::runtime_error("Unknown variable type " + type);
    }
}

void Interpreter::interpretPrint(ASTNode* expression, std::vector<StackFrame*>& stack) {

    // TODO evaluate the expression

    // Print the expression
    std::cout << expression->toString() << std::endl;
}