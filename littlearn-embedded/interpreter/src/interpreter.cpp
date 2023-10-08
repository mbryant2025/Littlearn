#include "interpreter.hpp"
#include "tokenizer.hpp"


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
    for (ASTNode* statement : (*block).getStatements()) {
        interpretStatement(statement, stack);
    }

    // Pop the stack frame off the stack
    stack.pop_back();

    // Delete the stack frame
    delete frame;
}

void Interpreter::interpretStatement(ASTNode* statement, std::vector<StackFrame*>& stack) {

    // These are the statements that represent "start points"
    // In other words, statements that do not return a value

    // Check if the statement is a variable declaration
    if (dynamic_cast<VariableDeclarationNode*>(statement) != nullptr) {
        interpretVariableDeclaration(dynamic_cast<VariableDeclarationNode*>(statement), stack);
    } else if (dynamic_cast<AssignmentNode*>(statement) != nullptr) {
        interpretAssignment(dynamic_cast<AssignmentNode*>(statement), stack);
    } else if (dynamic_cast<BlockNode*>(statement) != nullptr) {
        interpretBlock(dynamic_cast<BlockNode*>(statement), stack);
    } else if (dynamic_cast<PrintNode*>(statement) != nullptr) {
        interpretPrint(dynamic_cast<PrintNode*>(statement), stack);
    } else if (dynamic_cast<BinaryOperationNode*>(statement) != nullptr) {
        interpretBinaryOperation(dynamic_cast<BinaryOperationNode*>(statement), stack);
    } else if (dynamic_cast<IfNode*>(statement) != nullptr) {
        interpretIf(dynamic_cast<IfNode*>(statement), stack);
    } else {
        throw std::runtime_error("Unknown statement type " + statement->toString());
    }
}

ReturnableObject* Interpreter::interpretExpression(ASTNode* expression, std::vector<StackFrame*>& stack) {
    
        // These are the expressions that return a value, such as a variable access or a binary operation
    
        // Check if the expression is a variable access
        if (dynamic_cast<VariableAccessNode*>(expression) != nullptr) {
            return interpretVariableAccess(dynamic_cast<VariableAccessNode*>(expression), stack);
        } else if (dynamic_cast<BinaryOperationNode*>(expression) != nullptr) {
            return interpretBinaryOperation(dynamic_cast<BinaryOperationNode*>(expression), stack);
        } else if (dynamic_cast<NumberNode*>(expression) != nullptr) {
            return interpretNumber(dynamic_cast<NumberNode*>(expression), stack);
        } else {
            throw std::runtime_error("Unknown expression type " + expression->toString());
        }
}

ReturnableObject* Interpreter::interpretNumber(NumberNode* number, std::vector<StackFrame*>& stack) {
        
    // Get the type of the number
    std::string type = number->getType() == TokenType::INTEGER ? "int" : "float";

    // Get the value of the number, cast from string to either int or float
    float value = std::stof(number->getValue());

    if (type == "int") {
        // Create a new returnable int
        ReturnableInt* returnableInt = new ReturnableInt((int)value);

        // Return the returnable int
        return returnableInt;
    } else if (type == "float") {
        // Create a new returnable float
        ReturnableFloat* returnableFloat = new ReturnableFloat(value);

        // Return the returnable float
        return returnableFloat;
    } else {
        throw std::runtime_error("Unknown number type " + type);
    }
}

ReturnableObject* Interpreter::interpretVariableAccess(VariableAccessNode* variableAccess, std::vector<StackFrame*>& stack) {
    
        // Get the identifier
        std::string identifier = variableAccess->getIdentifier();
    
        // Get the type of the variable
        std::string type = stack.back()->getType(identifier);
    
        if (type == "int") {
            // Get the int variable
            int value = stack.back()->getIntVariable(identifier);
    
            // Create a new returnable int
            ReturnableInt* returnableInt = new ReturnableInt(value);
    
            // Return the returnable int
            return returnableInt;
        } else if (type == "float") {
            // Get the float variable
            float value = stack.back()->getFloatVariable(identifier);
    
            // Create a new returnable float
            ReturnableFloat* returnableFloat = new ReturnableFloat(value);
    
            // Return the returnable float
            return returnableFloat;
        } else {
            throw std::runtime_error("Unknown variable type " + type);
        }
}

ReturnableObject* Interpreter::interpretBinaryOperation(BinaryOperationNode* binaryExpression, std::vector<StackFrame*>& stack) {
    
    // Get the left and right expressions
    ASTNode* leftExpression = binaryExpression->getLeftExpression();
    ASTNode* rightExpression = binaryExpression->getRightExpression();

    // Evaluate the left and right expressions
    ReturnableObject* left = interpretExpression(leftExpression, stack);
    ReturnableObject* right = interpretExpression(rightExpression, stack);

    // Perform the binary operation
    
    // First check if either are floats -- if so then we need to convert both to floats
    if (left->getType() == "float" || right->getType() == "float") {
        // Convert the left and right to floats
        float leftFloat = 0.0;
        float rightFloat = 0.0;

        if (left->getType() == "float") {
            leftFloat = dynamic_cast<ReturnableFloat*>(left)->getValue();
        } else {
            leftFloat = dynamic_cast<ReturnableInt*>(left)->getValue();
        }

        if (right->getType() == "float") {
            rightFloat = dynamic_cast<ReturnableFloat*>(right)->getValue();
        } else {
            rightFloat = dynamic_cast<ReturnableInt*>(right)->getValue();
        }

        // Get the operator
        std::string op = binaryExpression->getOperator();

        // Check if the operator is +
        if (op == "+") {
            // Create a new float node with the sum of the left and right floats
            ReturnableFloat* sum = new ReturnableFloat(leftFloat + rightFloat);

            delete binaryExpression;

            return sum;
        } else if (op == "-") {
            // Create a new float node with the difference of the left and right floats
            ReturnableFloat* difference = new ReturnableFloat(leftFloat - rightFloat);

            delete binaryExpression;

            return difference;
        } else if (op == "*") {
            // Create a new float node with the product of the left and right floats
            ReturnableFloat* product = new ReturnableFloat(leftFloat * rightFloat);

            delete binaryExpression;

            return product;
        } else if (op == "/") {
            // Create a new float node with the quotient of the left and right floats
            ReturnableFloat* quotient = new ReturnableFloat(leftFloat / rightFloat);

            delete binaryExpression;

            return quotient;
        } else {
            throw std::runtime_error("Unknown operator " + op);
        }
    } else {
        // Convert the left and right to ints
        int leftInt = 0;
        int rightInt = 0;

        if (left->getType() == "int") {
            leftInt = dynamic_cast<ReturnableInt*>(left)->getValue();
        } else {
            leftInt = dynamic_cast<ReturnableFloat*>(left)->getValue();
        }

        if (right->getType() == "int") {
            rightInt = dynamic_cast<ReturnableInt*>(right)->getValue();
        } else {
            rightInt = dynamic_cast<ReturnableFloat*>(right)->getValue();
        }

        // Get the operator
        std::string op = binaryExpression->getOperator();

        // Check if the operator is +
        if (op == "+") {
            // Create a new int node with the sum of the left and right ints
            ReturnableInt* sum = new ReturnableInt(leftInt + rightInt);

            delete binaryExpression;

            return sum;
        } else if (op == "-") {
            // Create a new int node with the difference of the left and right ints
            ReturnableInt* difference = new ReturnableInt(leftInt - rightInt);

            delete binaryExpression;

            return difference;
        } else if (op == "*") {
            // Create a new int node with the product of the left and right ints
            ReturnableInt* product = new ReturnableInt(leftInt * rightInt);

            delete binaryExpression;

            return product;
        } else if (op == "/") {
            // Create a new int node with the quotient of the left and right ints
            ReturnableInt* quotient = new ReturnableInt(leftInt / rightInt);

            delete binaryExpression;

            return quotient;
        } else {
            throw std::runtime_error("Unknown operator " + op);
        }
    }
}

void Interpreter::interpretVariableDeclaration(VariableDeclarationNode* variableDeclaration, std::vector<StackFrame*>& stack) {

    ReturnableObject* val = interpretExpression(variableDeclaration->getInitializer(), stack);

    std::string type = variableDeclaration->getType();

    // Handle both types as a float and cast to the appropriate type later
    float value = val->getType() == "int" ? dynamic_cast<ReturnableInt*>(val)->getValue() : (float)dynamic_cast<ReturnableFloat*>(val)->getValue();

    if (type == "int") {
        // Allocate the int variable
        stack.back()->allocateIntVariable(variableDeclaration->getIdentifier(), (int)value);
    } else if (type == "float") {
        // Allocate the float variable
        stack.back()->allocateFloatVariable(variableDeclaration->getIdentifier(), (float)value);
    } else {
        throw std::runtime_error("Unknown variable type " + type);
    }
}

void Interpreter::interpretAssignment(AssignmentNode* assignment, std::vector<StackFrame*>& stack) {

    ReturnableObject* val = interpretExpression(assignment->getExpression(), stack);

    std::string type = stack.back()->getType(assignment->getIdentifier());

    // Handle both types as a float and cast to the appropriate type later
    float value = val->getType() == "int" ? dynamic_cast<ReturnableInt*>(val)->getValue() : (float)dynamic_cast<ReturnableFloat*>(val)->getValue();

    if (type == "int") {
        // Set the int variable
        stack.back()->setIntVariable(assignment->getIdentifier(), (int)value);
    } else if (type == "float") {
        // Set the float variable
        stack.back()->setFloatVariable(assignment->getIdentifier(), (float)value);
    } else {
        throw std::runtime_error("Unknown variable type " + type);
    }
}

void Interpreter::interpretPrint(ASTNode* expression, std::vector<StackFrame*>& stack) {

    // Cast to a print node
    PrintNode* printNode = dynamic_cast<PrintNode*>(expression);

    if(printNode == nullptr) {
        throw std::runtime_error("Unknown expression type " + expression->toString());
    }

    // We need to evaluate the expression
    ReturnableObject* returnableObject = interpretExpression(printNode->getExpression(), stack);

    // At this point we know the type of the returnable object to be either an int or a float
    if (returnableObject->getType() == "int") {
        // Print the int
        std::cout << dynamic_cast<ReturnableInt*>(returnableObject)->getValue() << "\n";
    } else if (returnableObject->getType() == "float") {
        // Print the float
        std::cout << dynamic_cast<ReturnableFloat*>(returnableObject)->getValue() << "\n";
    } else {
        throw std::runtime_error("Unknown returnable object type for print call: " + returnableObject->getType());
    }
}

void Interpreter::interpretIf(IfNode* ifStatement, std::vector<StackFrame*>& stack) {
    
    // Evaluate the condition
    ReturnableObject* condition = interpretExpression(ifStatement->getExpression(), stack);

    float conditionVal;

    if (condition->getType() == "int") {
        conditionVal = dynamic_cast<ReturnableInt*>(condition)->getValue();
    } else if (condition->getType() == "float") {
        conditionVal = dynamic_cast<ReturnableFloat*>(condition)->getValue();
    } else {
        throw std::runtime_error("Unknown condition type " + condition->getType());
    }

    // Check if the condition is true
    // Truthy is value != 0
    if (conditionVal != 0) {
        // Interpret the if block
        interpretBlock(ifStatement->getBody(), stack);
    }
}

//===================================================


ReturnableFloat::ReturnableFloat(float value) : value(value) {}

std::string ReturnableFloat::getType() {
    return "float";
}

float ReturnableFloat::getValue() {
    return value;
}

ReturnableFloat::~ReturnableFloat() {}

ReturnableInt::ReturnableInt(int value) : value(value) {}

std::string ReturnableInt::getType() {
    return "int";
}

int ReturnableInt::getValue() {
    return value;
}

ReturnableInt::~ReturnableInt() {}
