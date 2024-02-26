#include "interpreter.hpp"

#include <math.h>
#include <thread>

#include "flags.h"
#include "error.hpp"
#include "tokenizer.hpp"

// Return null pointer if there is an error
// Signified by this macro
#define ERROR_EXIT nullptr

#define BIND_FUNCTION(func) std::bind(&Interpreter::func, this, std::placeholders::_1, std::placeholders::_2)

StackFrame::StackFrame(StackFrame *parent, OutputStream &outputStream, ErrorHandler &errorHandler)
    : parent(parent), outputStream(outputStream), errorHandler(errorHandler) {}

StackFrame::~StackFrame() {
    // Functions should not be deleted here because they are stored in the AST
}

void StackFrame::allocateFloatVariable(std::string &name, float value) {
    if (isAllocated(name)) {
        errorHandler.handleError("Runtime Error: Identifier " + name + " already exists in this scope");
    } else {
        float_variables[name] = value;
    }
}

void StackFrame::allocateIntVariable(std::string &name, int value) {
    if (isAllocated(name)) {
        errorHandler.handleError("Runtime Error: Identifier " + name + " already exists in this scope");
    } else {
        int_variables[name] = value;
    }
}

void StackFrame::allocateFunction(std::string &name, FunctionDeclarationNode *function) {
    if (isAllocated(name)) {
        errorHandler.handleError("Runtime Error: Identifier " + name + " already exists in this scope");
    } else {
        functions[name] = function;
    }
}

void StackFrame::setFloatVariable(std::string &name, float value) {
    // Check if the variable exists in the float map
    if (float_variables.find(name) != float_variables.end()) {
        float_variables[name] = value;
    } else if (parent != nullptr) {
        parent->setFloatVariable(name, value);
    } else {
        errorHandler.handleError("Runtime Error: Variable " + name + " does not exist in this scope");
    }
}

void StackFrame::setIntVariable(std::string &name, int value) {
    // Check if the variable exists in the int map
    if (int_variables.find(name) != int_variables.end()) {
        int_variables[name] = value;
    } else if (parent != nullptr) {
        parent->setIntVariable(name, value);
    } else {
        errorHandler.handleError("Runtime Error: Variable " + name + " does not exist in this scope");
    }
}

float StackFrame::getFloatVariable(std::string &name) {
    // Check if the variable exists in the float map
    if (float_variables.find(name) != float_variables.end()) {
        return float_variables[name];
    } else if (parent != nullptr) {
        return parent->getFloatVariable(name);
    } else {
        errorHandler.handleError("Runtime Error: Variable " + name + " does not exist in this scope");
        return 0.0;
    }
}

int StackFrame::getIntVariable(std::string &name) {
    // Check if the variable exists in the int map
    if (int_variables.find(name) != int_variables.end()) {
        return int_variables[name];
    } else if (parent != nullptr) {
        return parent->getIntVariable(name);
    } else {
        errorHandler.handleError("Runtime Error: Variable " + name + " does not exist in this scope");
        return 0;
    }
}

FunctionDeclarationNode *StackFrame::getFunction(std::string &name) {
    // Check if the variable exists in the function map
    if (functions.find(name) != functions.end()) {
        return functions[name];
    } else if (parent != nullptr) {
        return parent->getFunction(name);
    } else {
        errorHandler.handleError("Runtime Error: Function " + name + " does not exist in this scope");
        return nullptr;
    }
}

ValueType StackFrame::getType(std::string &name) {
    // Check if the variable exists in the float map
    if (float_variables.find(name) != float_variables.end()) {
        return ValueType::FLOAT;
    } else if (int_variables.find(name) != int_variables.end()) {
        return ValueType::INTEGER;
    } else if (functions.find(name) != functions.end()) {
        return ValueType::FUNCTION;
    } else if (parent != nullptr) {
        return parent->getType(name);
    } else {
        errorHandler.handleError("Runtime Error: Variable " + name + " does not exist in this scope");
        return ValueType::INTEGER;  // Bogus value does not matter because the error handler will stop execution
    }
}

bool StackFrame::isAllocated(std::string &name) {
    if (float_variables.find(name) != float_variables.end()) {
        return true;
    } else if (int_variables.find(name) != int_variables.end()) {
        return true;
    } else if (functions.find(name) != functions.end()) {
        return true;
    } else if (parent != nullptr) {
        return parent->isAllocated(name);
    } else {
        return false;
    }
}

std::map<std::string, FunctionDeclarationNode *> &StackFrame::getFunctions() {
    return functions;
}

void Interpreter::initBuiltInFunctions() {
    // Fill out the function map
    functionMap["print"] = BIND_FUNCTION(_print);
    functionMap["wait"] = BIND_FUNCTION(_wait);
    functionMap["rand"] = BIND_FUNCTION(_rand);
    functionMap["float_to_int"] = BIND_FUNCTION(_float_to_int);
    functionMap["int_to_float"] = BIND_FUNCTION(_int_to_float);
    functionMap["runtime"] = BIND_FUNCTION(_runtime);
    functionMap["pow"] = BIND_FUNCTION(_pow);
    functionMap["pi"] = BIND_FUNCTION(_pi);
    functionMap["exp"] = BIND_FUNCTION(_exp);
    functionMap["sin"] = BIND_FUNCTION(_sin);
    functionMap["cos"] = BIND_FUNCTION(_cos);
    functionMap["tan"] = BIND_FUNCTION(_tan);
    functionMap["asin"] = BIND_FUNCTION(_asin);
    functionMap["acos"] = BIND_FUNCTION(_acos);
    functionMap["atan"] = BIND_FUNCTION(_atan);
    functionMap["atan2"] = BIND_FUNCTION(_atan2);
    functionMap["sqrt"] = BIND_FUNCTION(_sqrt);
    functionMap["abs"] = BIND_FUNCTION(_abs);
    functionMap["floor"] = BIND_FUNCTION(_floor);
    functionMap["ceil"] = BIND_FUNCTION(_ceil);
    functionMap["min"] = BIND_FUNCTION(_min);
    functionMap["max"] = BIND_FUNCTION(_max);
    functionMap["log"] = BIND_FUNCTION(_log);
    functionMap["log10"] = BIND_FUNCTION(_log10);
    functionMap["log2"] = BIND_FUNCTION(_log2);
    functionMap["round"] = BIND_FUNCTION(_round);
    functionMap["send_bool"] = BIND_FUNCTION(_sendBool);
}


Interpreter::Interpreter(BlockNode &ast, OutputStream &outputStream, ErrorHandler &errorHandler) : ast(ast), outputStream(outputStream), errorHandler(errorHandler), radioFormatter(nullptr) {
   initBuiltInFunctions();
}

Interpreter::Interpreter(BlockNode& ast, OutputStream& outputStream, ErrorHandler& errorHandler, RadioFormatter& radioFormatter) : ast(ast), outputStream(outputStream), errorHandler(errorHandler), radioFormatter(&radioFormatter) {
    initBuiltInFunctions();
}

Interpreter::~Interpreter() {
}

void Interpreter::runtimeError(const std::string &message) const {
    errorHandler.handleError("Runtime Error: " + message);
}

void Interpreter::interpret() {
    if (errorHandler.shouldStopExecution()) {
        return;
    }

    // Create a stack frame for the global scope
    StackFrame *globalScope = new StackFrame(nullptr, outputStream, errorHandler);

    // Add the built-in functions to the global scope for the sake of throwing errors if they are redefined by the user
    // The functions themselves are not stored in the AST
    for (const auto &function : functionMap) {
        std::string name = function.first;  // Do it in two steps to create references
        globalScope->allocateFunction(name, nullptr);
    }

    // Create a vector of stack frames
    std::vector<StackFrame *> stack;
    stack.push_back(globalScope);

    // Interpret the block
    ExitingObject *ret = interpretBlock(&ast, stack);

    if (ret != ERROR_EXIT)
        delete ret;

    delete globalScope;
}

ExitingObject *Interpreter::interpretBlock(BlockNode *block, std::vector<StackFrame *> &stack) {
    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    // Create a new stack frame
    StackFrame *frame = new StackFrame(stack.back(), outputStream, errorHandler);

    // Push the new stack frame onto the stack
    stack.push_back(frame);

    // Interpret each statement in the block
    for (ASTNode *statement : block->getStatements()) {
        ExitingObject *ret = interpretStatement(statement, stack);

        // Check if we should stop execution
        if (errorHandler.shouldStopExecution()) {
            delete ret;
            stack.pop_back();
            delete frame;
            return ERROR_EXIT;
        }

        switch (ret->getType()) {
            case ExitingType::BREAK:
                delete ret;
                stack.pop_back();
                delete frame;
                return new ExitingBreak();
            case ExitingType::CONTINUE:
                delete ret;
                stack.pop_back();
                delete frame;
                return new ExitingContinue();
            case ExitingType::RETURN:
                stack.pop_back();
                delete frame;
                return ret;
            case ExitingType::NONE:
                delete ret;
                // Do nothing
        }
    }

    // Pop the stack frame off the stack
    stack.pop_back();

    // Delete the stack frame
    delete frame;

    return new ExitingNone();
}

ExitingObject *Interpreter::interpretStatement(ASTNode *statement, std::vector<StackFrame *> &stack) {
    // These are the statements that represent "start points"
    // In other words, statements that do not return a ValueType

    // Avoids RTTI by using virtual function to getNodeType

    switch (statement->getNodeType()) {
        case ASTNodeType::VARIABLE_DECLARATION_NODE:
            interpretVariableDeclaration((VariableDeclarationNode *)statement, stack);
            return new ExitingNone();

        case ASTNodeType::ASSIGNMENT_NODE:
            interpretAssignment((AssignmentNode *)statement, stack);
            return new ExitingNone();

        case ASTNodeType::FUNCTION_DECLARATION_NODE:
            interpretFunctionDeclaration((FunctionDeclarationNode *)statement, stack);
            return new ExitingNone();

        case ASTNodeType::IF_NODE:
            return interpretIf((IfNode *)statement, stack);

        case ASTNodeType::WHILE_NODE:
            return interpretWhile((WhileNode *)statement, stack);

        case ASTNodeType::FOR_NODE:
            return interpretFor((ForNode *)statement, stack);

        case ASTNodeType::BREAK_NODE:
            return new ExitingBreak();

        case ASTNodeType::CONTINUE_NODE:
            return new ExitingContinue();

        case ASTNodeType::RETURN_NODE:
            return interpretReturn((ReturnNode *)statement, stack);

        case ASTNodeType::FUNCTION_CALL_NODE:
            delete interpretFunctionCall((FunctionCallNode *)statement, stack);
            return new ExitingNone();

        default:
            runtimeError("Unknown statement type " + statement->toString());
            return ERROR_EXIT;
    }
}

ReturnableObject *Interpreter::interpretExpression(ASTNode *expression, std::vector<StackFrame *> &stack) {
    // Check if we should stop execution
    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    // These are the expressions that return a value, such as a variable access or a binary operation
    switch (expression->getNodeType()) {
        case ASTNodeType::VARIABLE_ACCESS_NODE:
            return interpretVariableAccess((VariableAccessNode *)expression, stack);

        case ASTNodeType::BINARY_OPERATION_NODE:
            return interpretBinaryOperation((BinaryOperationNode *)expression, stack);

        case ASTNodeType::NUMBER_NODE:
            return interpretNumber((NumberNode *)expression, stack);

        case ASTNodeType::FUNCTION_CALL_NODE:
            return interpretFunctionCall((FunctionCallNode *)expression, stack);

        default:
            runtimeError("Unknown expression type " + expression->toString());
            return ERROR_EXIT;
    }
}

ReturnableObject *Interpreter::interpretNumber(NumberNode *number, std::vector<StackFrame *> &stack) {
    // Get the type of the number
    std::string type = number->getType() == TokenType::INTEGER ? "int" : "float";

    // Get the value of the number, cast from string to either int or float
    float value = std::stof(number->getValue());

    if (type == "int") {
        return new ReturnableInt((int)value);
    } else if (type == "float") {
        // Create a new returnable float
        return new ReturnableFloat(value);
    } else {
        runtimeError("Unknown number type " + type);
        return ERROR_EXIT;
    }
}

ReturnableObject *Interpreter::interpretVariableAccess(VariableAccessNode *variableAccess, std::vector<StackFrame *> &stack) {
    // Get the identifier
    std::string identifier = variableAccess->getIdentifier();

    // Get the type of the variable
    ValueType type = stack.back()->getType(identifier);

    // Check if the variable exists -- if not then throw an error
    if (type == ValueType::INTEGER) {
        // Get the int variable
        int value = stack.back()->getIntVariable(identifier);

        return new ReturnableInt(value);

    } else if (type == ValueType::FLOAT) {
        // Get the float variable
        float value = stack.back()->getFloatVariable(identifier);

        return new ReturnableFloat(value);

    } else {
        runtimeError("Unknown variable type " + identifier);
        return ERROR_EXIT;
    }
}

ReturnableObject *Interpreter::interpretBinaryOperation(BinaryOperationNode *binaryExpression, std::vector<StackFrame *> &stack) {
    ASTNode *leftExpression = binaryExpression->getLeftExpression();
    ASTNode *rightExpression = binaryExpression->getRightExpression();

    ReturnableObject *left = interpretExpression(leftExpression, stack);

    if (errorHandler.shouldStopExecution()) {
        delete left;
        return ERROR_EXIT;
    }

    ReturnableObject *right = interpretExpression(rightExpression, stack);

    if (errorHandler.shouldStopExecution()) {
        delete left;
        delete right;
        return ERROR_EXIT;
    }

    ValueType leftType = left->getType();
    ValueType rightType = right->getType();

    if (leftType == ValueType::FLOAT || rightType == ValueType::FLOAT) {
        float leftFloat = (leftType == ValueType::INTEGER) ? ((ReturnableInt *)left)->getValue() : ((ReturnableFloat *)left)->getValue();
        float rightFloat = (rightType == ValueType::INTEGER) ? ((ReturnableInt *)right)->getValue() : ((ReturnableFloat *)right)->getValue();
        std::string op = binaryExpression->getOperator();

        if (op == "/" && rightFloat == 0) {
            runtimeError("Division by zero");
            delete left;
            delete right;
            return ERROR_EXIT;
        }

        if (op == "+" || op == "-" || op == "*" || op == "/") {
            delete left;
            delete right;
            return new ReturnableFloat(
                (op == "+")   ? leftFloat + rightFloat
                : (op == "-") ? leftFloat - rightFloat
                : (op == "*") ? leftFloat * rightFloat
                              : leftFloat / rightFloat);
        } else {
            delete left;
            delete right;
            return new ReturnableInt(
                (op == ">")    ? leftFloat > rightFloat
                : (op == "<")  ? leftFloat < rightFloat
                : (op == "%")  ? (int)leftFloat % (int)rightFloat
                : (op == ">=") ? leftFloat >= rightFloat
                : (op == "<=") ? leftFloat <= rightFloat
                : (op == "==") ? leftFloat == rightFloat
                : (op == "!=") ? leftFloat != rightFloat
                : (op == "&&") ? leftFloat && rightFloat
                               : leftFloat || rightFloat);
        }
    } else {
        int leftInt = (leftType == ValueType::INTEGER) ? ((ReturnableInt *)left)->getValue() : ((ReturnableFloat *)left)->getValue();
        int rightInt = (rightType == ValueType::INTEGER) ? ((ReturnableInt *)right)->getValue() : ((ReturnableFloat *)right)->getValue();
        std::string op = binaryExpression->getOperator();

        if (op == "/" && rightInt == 0) {
            runtimeError("Division by zero");
            delete left;
            delete right;
            return ERROR_EXIT;
        }

        delete left;
        delete right;

        return new ReturnableInt(
            (op == "+")    ? leftInt + rightInt
            : (op == "-")  ? leftInt - rightInt
            : (op == "*")  ? leftInt * rightInt
            : (op == "/")  ? leftInt / rightInt
            : (op == ">")  ? leftInt > rightInt
            : (op == "<")  ? leftInt < rightInt
            : (op == "%")  ? leftInt % rightInt
            : (op == ">=") ? leftInt >= rightInt
            : (op == "<=") ? leftInt <= rightInt
            : (op == "==") ? leftInt == rightInt
            : (op == "!=") ? leftInt != rightInt
            : (op == "&&") ? leftInt && rightInt
                           : leftInt || rightInt);
    }
}

ReturnableObject *Interpreter::interpretFunctionCall(FunctionCallNode *functionCall, std::vector<StackFrame *> &stack) {
    // Get the identifier
    std::string identifier = functionCall->getName();

    // Get the arguments
    std::vector<ASTNode *> arguments = functionCall->getArguments();

    // Check if the function exists as a built-in function
    // If so, use the map to call the function
    if (functionMap.find(identifier) != functionMap.end()) {
        return functionMap[identifier](arguments, stack);
    }

    // Get the function from the stack
    FunctionDeclarationNode *function = stack.back()->getFunction(identifier);

    if (function == nullptr) {
        return ERROR_EXIT;
    }

    // Get the parameters
    std::vector<std::string> parameters = function->getParameters();

    // Get the parameter types
    std::vector<std::string> parameterTypes = function->getParameterTypes();

    // Check if the number of arguments matches the number of parameters
    // Or if there are 0 arguments in the function and 1 is given and it is an EmptyExpressionNode
    if (arguments.size() != parameters.size() && !(arguments.size() == 1 && arguments[0]->getNodeType() == ASTNodeType::EMPTY_EXPRESSION_NODE)) {
        runtimeError("Function " + identifier + " takes " + std::to_string(parameters.size()) + " arguments, but " + std::to_string(arguments.size()) + " were given");
        return ERROR_EXIT;
    }

    // Create a new stack frame independent of the current stack frame
    StackFrame *newFrame = new StackFrame(nullptr, outputStream, errorHandler);

    // Gather all functions from the current stack frame
    std::map<std::string, FunctionDeclarationNode *> functions;
    for (StackFrame *frame : stack) {
        std::map<std::string, FunctionDeclarationNode *> frameFunctions = frame->getFunctions();
        functions.insert(frameFunctions.begin(), frameFunctions.end());
    }

    // Add the functions to the new stack frame
    for (std::pair<std::string, FunctionDeclarationNode *> function : functions) {
        newFrame->allocateFunction(function.first, function.second);
    }

    // Note that the above is done per function rather than having a global map of functions
    // This is because we can scope functions in the same way as variables

    // Create a new stack for the new stack frame
    std::vector<StackFrame *> newStack;
    newStack.push_back(newFrame);

    // Interpret each argument
    for (int i = 0; i < parameters.size(); i++) {
        // Get the argument
        ASTNode *argument = arguments[i];

        // Get the parameter
        std::string parameter = parameters[i];

        // Get the parameter type
        std::string parameterType = parameterTypes[i];

        // Interpret the argument
        ReturnableObject *value = interpretExpression(argument, stack);

        if (errorHandler.shouldStopExecution()) {
            delete value;
            delete newFrame;
            return ERROR_EXIT;
        }

        // Allocate the parameter
        if (parameterType == "int") {
            if (value->getType() == ValueType::INTEGER) {
                newFrame->allocateIntVariable(parameter, ((ReturnableInt *)value)->getValue());
            } else {
                newFrame->allocateIntVariable(parameter, (int)((ReturnableFloat *)value)->getValue());
            }
        } else if (parameterType == "float") {
            if (value->getType() == ValueType::INTEGER) {
                newFrame->allocateFloatVariable(parameter, (float)((ReturnableInt *)value)->getValue());
            } else {
                newFrame->allocateFloatVariable(parameter, ((ReturnableFloat *)value)->getValue());
            }
        } else {
            runtimeError("Unknown parameter type " + parameterType);
            delete value;
            delete newFrame;
            return ERROR_EXIT;
        }

        delete value;
    }

    // Interpret the function body
    ExitingObject *ret = interpretBlock(function->getBody(), newStack);

    if (errorHandler.shouldStopExecution()) {
        delete newFrame;
        return ERROR_EXIT;
    }

    // Delete the new stack frame
    delete newFrame;

    // If ret is a return, return the value, otherwise return 0
    if (ret->getType() == ExitingType::RETURN) {
        ReturnableObject *value = ((ExitingReturn *)ret)->getValue();
        delete ret;
        return value;
    } else {
        delete ret;
        return new ReturnableInt(0);
    }
}

void Interpreter::interpretVariableDeclaration(VariableDeclarationNode *variableDeclaration, std::vector<StackFrame *> &stack) {
    ReturnableObject *val = interpretExpression(variableDeclaration->getInitializer(), stack);

    if (errorHandler.shouldStopExecution()) {
        return;
    }

    std::string type = variableDeclaration->getType();

    // Handle both types as a float and cast to the appropriate type later
    float value = val->getType() == ValueType::INTEGER ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    delete val;

    std::string identifier = variableDeclaration->getIdentifier();

    if (type == "int") {
        // Allocate the int variable
        stack.back()->allocateIntVariable(identifier, (int)value);
    } else if (type == "float") {
        // Allocate the float variable
        stack.back()->allocateFloatVariable(identifier, (float)value);
    } else {
        runtimeError("Unknown variable type " + type);
    }
}

void Interpreter::interpretAssignment(AssignmentNode *assignment, std::vector<StackFrame *> &stack) {
    ReturnableObject *val = interpretExpression(assignment->getExpression(), stack);

    if (errorHandler.shouldStopExecution()) {
        return;
    }

    std::string identifier = assignment->getIdentifier();

    ValueType type = stack.back()->getType(identifier);

    if (errorHandler.shouldStopExecution()) {
        delete val;
        return;
    }

    // Handle both types as a float and cast to the appropriate type later
    float value = val->getType() == ValueType::INTEGER ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    if (type == ValueType::INTEGER) {
        // Set the int variable
        stack.back()->setIntVariable(identifier, (int)value);
    } else if (type == ValueType::FLOAT) {
        // Set the float variable
        stack.back()->setFloatVariable(identifier, (float)value);
    } else {
        delete val;
        runtimeError("Unknown variable type for " + identifier);
    }

    delete val;
}

void Interpreter::interpretFunctionDeclaration(FunctionDeclarationNode *functionDeclaration, std::vector<StackFrame *> &stack) {
    // Get the identifier
    std::string identifier = functionDeclaration->getName();

    // Allocate the function
    stack.back()->allocateFunction(identifier, functionDeclaration);
}

bool Interpreter::interpretTruthiness(ReturnableObject *condition, std::vector<StackFrame *> &stack) {
    float conditionVal = 0; // Default to false

    if (condition->getType() == ValueType::INTEGER) {
        conditionVal = ((ReturnableInt *)condition)->getValue();
    } else if (condition->getType() == ValueType::FLOAT) {
        conditionVal = ((ReturnableFloat *)condition)->getValue();
    } else {
        runtimeError("Unknown condition type for interpret truthiness");
    }

    // Check if the condition is true
    // Truthy is value != 0
    if (conditionVal != 0) {
        return true;
    } else {
        return false;
    }
}

ExitingObject *Interpreter::interpretIf(IfNode *ifStatement, std::vector<StackFrame *> &stack) {
    std::vector<ASTNode *> expressions = ifStatement->getExpressions();
    std::vector<BlockNode *> bodies = ifStatement->getBodies();

    // Interpret each expression and once one is true, interpret the corresponding body
    // If none are true, interpret the else body if it exists

    int blockNum = -1;

    for (int i = 0; i < expressions.size(); i++) {
        // Evaluate the condition
        ReturnableObject *condition = interpretExpression(expressions[i], stack);

        if (errorHandler.shouldStopExecution()) {
            return ERROR_EXIT;
        }

        // Check if the condition is true
        if (interpretTruthiness(condition, stack)) {
            blockNum = i;
            delete condition;
            break;
        }

        delete condition;
    }

    // Interpret the block
    // If blockNum is -1, then the else body should be interpreted if it exists

    // If no conditions evaluated to true and there is no else body, then do nothing
    if (blockNum == -1 && expressions.size() == bodies.size()) {
        return new ExitingNone();
    }

    // If no conditions evaluated to true and there is an else body, then interpret the else body
    if (blockNum == -1 && expressions.size() == bodies.size() - 1) {
        return interpretBlock(bodies.back(), stack);
    }

    // If a condition evaluated to true, then interpret the corresponding body
    return interpretBlock(bodies[blockNum], stack);
}

ExitingObject *Interpreter::interpretWhile(WhileNode *whileStatement, std::vector<StackFrame *> &stack) {
    // Evaluate the condition
    ReturnableObject *condition = interpretExpression(whileStatement->getExpression(), stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    // Check if the condition is true
    while (interpretTruthiness(condition, stack)) {
        // Interpret the while block
        // Gather the return type to check for break or continue
        ExitingObject *returnType = interpretBlock(whileStatement->getBody(), stack);

        if (errorHandler.shouldStopExecution()) {
            delete condition;
            return ERROR_EXIT;
        }

        switch (returnType->getType()) {
            case ExitingType::BREAK:
                delete condition;
                delete returnType;
                return new ExitingNone();
            case ExitingType::RETURN:
                delete condition;
                return returnType;
            case ExitingType::CONTINUE:
            case ExitingType::NONE:
                delete returnType;
                // Do nothing
                break;
        }

        // Re-evaluate the condition
        delete condition;
        condition = interpretExpression(whileStatement->getExpression(), stack);
    }

    delete condition;
    return new ExitingNone();
}

ExitingObject *Interpreter::interpretFor(ForNode *forStatement, std::vector<StackFrame *> &stack) {
    // Evaluate the initializer
    interpretVariableDeclaration((VariableDeclarationNode *)forStatement->getInitializer(), stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    // Evaluate the condition
    ReturnableObject *condition = interpretExpression(forStatement->getCondition(), stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    // Check if the condition is true
    while (interpretTruthiness(condition, stack)) {
        // Interpret the for block
        // Gather the return type to check for break or continue
        ExitingObject *returnType = interpretBlock(forStatement->getBody(), stack);

        if (errorHandler.shouldStopExecution()) {
            delete condition;
            return ERROR_EXIT;
        }

        switch (returnType->getType()) {
            case ExitingType::BREAK:
                delete condition;
                delete returnType;
                return new ExitingNone();
            case ExitingType::CONTINUE:
            case ExitingType::NONE:
                // Continue to the next iteration
                break;
            case ExitingType::RETURN:
                delete condition;
                return returnType;
        }

        delete returnType;

        // Evaluate the increment
        interpretAssignment((AssignmentNode *)forStatement->getIncrement(), stack);

        if (errorHandler.shouldStopExecution()) {
            delete condition;
            return ERROR_EXIT;
        }

        // Re-evaluate the condition
        delete condition;
        condition = interpretExpression(forStatement->getCondition(), stack);

        if (errorHandler.shouldStopExecution()) {
            delete condition;
            return ERROR_EXIT;
        }
    }

    delete condition;
    return new ExitingNone();
}

ExitingObject *Interpreter::interpretReturn(ReturnNode *returnStatement, std::vector<StackFrame *> &stack) {
    // Check if it is a return statement with no expression
    if (returnStatement->getExpression() == nullptr) {
        return new ExitingReturn();
    }

    ReturnableObject *value = interpretExpression(returnStatement->getExpression(), stack);

    if (errorHandler.shouldStopExecution()) {
        delete value;
        return ERROR_EXIT;
    }

    return new ExitingReturn(value);
}

// Builtin functions

ReturnableObject *Interpreter::_print(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("print() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() == ValueType::INTEGER)
        outputStream.write(PRINT_FLAG + std::to_string(((ReturnableInt *)val)->getValue()) + "\n" + PRINT_FLAG);
    else 
        outputStream.write(PRINT_FLAG + std::to_string(((ReturnableFloat *)val)->getValue()) + "\n" + PRINT_FLAG);

    delete val;

    return new ReturnableInt(0);
}

ReturnableObject *Interpreter::_wait(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("wait() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::INTEGER) {
        runtimeError("wait() takes an integer argument");
        delete val;
        return ERROR_EXIT;
    }

    int value = ((ReturnableInt *)val)->getValue();

    delete val;

    std::this_thread::sleep_for(std::chrono::milliseconds(value));

    return new ReturnableInt(0);
}

ReturnableObject *Interpreter::_rand(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument -- that being an EmptyExpressionNode
    if (arguments.size() != 1 || arguments[0]->getNodeType() != ASTNodeType::EMPTY_EXPRESSION_NODE) {
        runtimeError("rand() takes exactly 0 arguments");
        return ERROR_EXIT;
    }

    // Generate a random number between 0 and 1
    float value = (float)rand() / RAND_MAX;

    return new ReturnableFloat(value);
}

ReturnableObject *Interpreter::_float_to_int(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("int() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::FLOAT) {
        runtimeError("int() takes a float argument");
        delete val;
        return ERROR_EXIT;
    }

    float value = ((ReturnableFloat *)val)->getValue();

    delete val;

    return new ReturnableInt((int)value);
}

ReturnableObject *Interpreter::_int_to_float(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("float() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::INTEGER) {
        runtimeError("float() takes an integer argument");
        delete val;
        return ERROR_EXIT;
    }

    int value = ((ReturnableInt *)val)->getValue();

    delete val;

    return new ReturnableFloat((float)value);
}

ReturnableObject *Interpreter::_runtime(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument -- that being an EmptyExpressionNode
    if (arguments.size() != 1 || arguments[0]->getNodeType() != ASTNodeType::EMPTY_EXPRESSION_NODE) {
        runtimeError("runtime() takes exactly 0 arguments");
        return ERROR_EXIT;
    }

#if __EMBEDDED__
    // return new ReturnableInt((int)round(millis())); //TODO ensure this is good
    return new ReturnableInt((int)round((double)clock() / CLOCKS_PER_SEC * 1000));
#else
    return new ReturnableInt((int)round((double)clock() / CLOCKS_PER_SEC * 1000));
#endif
}

ReturnableObject *Interpreter::_pow(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there are exactly two arguments
    if (arguments.size() != 2) {
        runtimeError("pow() takes exactly two arguments");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val1 = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    // Get the second argument
    ReturnableObject *val2 = interpretExpression(arguments[1], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val1->getType() != ValueType::INTEGER && val1->getType() != ValueType::FLOAT) {
        runtimeError("pow() takes a float or integer argument");
        delete val1;
        delete val2;
        return ERROR_EXIT;
    }

    if (val2->getType() != ValueType::INTEGER && val2->getType() != ValueType::FLOAT) {
        runtimeError("pow() takes a float or integer argument");
        delete val1;
        delete val2;
        return ERROR_EXIT;
    }

    float value1 = (val1->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val1)->getValue() : ((ReturnableFloat *)val1)->getValue();
    float value2 = (val2->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val2)->getValue() : ((ReturnableFloat *)val2)->getValue();

    delete val1;
    delete val2;

    return new ReturnableFloat(pow(value1, value2));
}

ReturnableObject *Interpreter::_pi(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument -- that being an EmptyExpressionNode
    if (arguments.size() != 1 || arguments[0]->getNodeType() != ASTNodeType::EMPTY_EXPRESSION_NODE) {
        runtimeError("pi() takes exactly 0 arguments");
        return ERROR_EXIT;
    }

    return new ReturnableFloat(PI);
}

ReturnableObject *Interpreter::_exp(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("exp() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::INTEGER && val->getType() != ValueType::FLOAT) {
        runtimeError("exp() takes a float or integer argument");
        delete val;
        return ERROR_EXIT;
    }

    float value = (val->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    delete val;

    return new ReturnableFloat(exp(value));
}

ReturnableObject *Interpreter::_sin(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("sin() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::INTEGER && val->getType() != ValueType::FLOAT) {
        runtimeError("sin() takes a float or integer argument");
        delete val;
        return ERROR_EXIT;
    }

    float value = (val->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    delete val;

    return new ReturnableFloat(sin(value));
}

ReturnableObject *Interpreter::_cos(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("cos() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::INTEGER && val->getType() != ValueType::FLOAT) {
        runtimeError("cos() takes a float or integer argument");
        delete val;
        return ERROR_EXIT;
    }

    float value = (val->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    delete val;

    return new ReturnableFloat(cos(value));
}

ReturnableObject *Interpreter::_tan(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("tan() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::INTEGER && val->getType() != ValueType::FLOAT) {
        runtimeError("tan() takes a float or integer argument");
        delete val;
        return ERROR_EXIT;
    }

    float value = (val->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    delete val;

    return new ReturnableFloat(tan(value));
}

ReturnableObject *Interpreter::_asin(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("asin() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::FLOAT && val->getType() != ValueType::INTEGER) {
        runtimeError("asin() takes a float or integer argument");
        delete val;
        return ERROR_EXIT;
    }

    float value = (val->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    delete val;

    if (value < -1 || value > 1) {
        runtimeError("asin() takes an argument between -1 and 1");
        return ERROR_EXIT;
    }

    return new ReturnableFloat(asin(value));
}

ReturnableObject *Interpreter::_acos(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("acos() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::FLOAT && val->getType() != ValueType::INTEGER) {
        runtimeError("acos() takes a float or integer argument");
        delete val;
        return ERROR_EXIT;
    }

    float value = (val->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    delete val;

    if (value < -1 || value > 1) {
        runtimeError("acos() takes an argument between -1 and 1");
        return ERROR_EXIT;
    }

    return new ReturnableFloat(acos(value));
}

ReturnableObject *Interpreter::_atan(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("atan() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::FLOAT && val->getType() != ValueType::INTEGER) {
        runtimeError("atan() takes a float or integer argument");
        delete val;
        return ERROR_EXIT;
    }

    float value = (val->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    delete val;

    return new ReturnableFloat(atan(value));
}

ReturnableObject *Interpreter::_atan2(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there are exactly two arguments
    if (arguments.size() != 2) {
        runtimeError("atan2() takes exactly two arguments");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val1 = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val1->getType() != ValueType::FLOAT && val1->getType() != ValueType::INTEGER) {
        runtimeError("atan2() takes a float or integer argument");
        delete val1;
        return ERROR_EXIT;
    }

    // Get the second argument
    ReturnableObject *val2 = interpretExpression(arguments[1], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val2->getType() != ValueType::FLOAT && val2->getType() != ValueType::INTEGER) {
        runtimeError("atan2() takes a float or integer argument");
        delete val1;
        delete val2;
        return ERROR_EXIT;
    }

    float value1 = (val1->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val1)->getValue() : ((ReturnableFloat *)val1)->getValue();
    float value2 = (val2->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val2)->getValue() : ((ReturnableFloat *)val2)->getValue();

    delete val1;
    delete val2;

    return new ReturnableFloat(atan2(value1, value2));
}

ReturnableObject *Interpreter::_sqrt(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("sqrt() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::FLOAT && val->getType() != ValueType::INTEGER) {
        runtimeError("sqrt() takes a float or integer argument");
        delete val;
        return ERROR_EXIT;
    }

    float value = (val->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    if (value < 0) {
        runtimeError("sqrt() takes a positive argument");
        delete val;
        return ERROR_EXIT;
    }

    delete val;

    return new ReturnableFloat(sqrt(value));
}

ReturnableObject *Interpreter::_abs(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("abs() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::FLOAT && val->getType() != ValueType::INTEGER) {
        runtimeError("abs() takes a float or integer argument");
        delete val;
        return ERROR_EXIT;
    }

    float value = (val->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    delete val;

    return new ReturnableFloat(abs(value));
}

ReturnableObject *Interpreter::_floor(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("floor() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::FLOAT && val->getType() != ValueType::INTEGER) {
        runtimeError("floor() takes a float or integer argument");
        delete val;
        return ERROR_EXIT;
    }

    float value = (val->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    delete val;

    return new ReturnableFloat(floor(value));
}

ReturnableObject *Interpreter::_ceil(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("ceil() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::FLOAT && val->getType() != ValueType::INTEGER) {
        runtimeError("ceil() takes a float or integer argument");
        delete val;
        return ERROR_EXIT;
    }

    float value = (val->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    delete val;

    return new ReturnableFloat(ceil(value));
}

ReturnableObject *Interpreter::_min(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there are exactly two arguments
    if (arguments.size() != 2) {
        runtimeError("min() takes exactly two arguments");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val1 = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val1->getType() != ValueType::FLOAT && val1->getType() != ValueType::INTEGER) {
        runtimeError("min() takes a float or integer argument");
        delete val1;
        return ERROR_EXIT;
    }

    // Get the second argument
    ReturnableObject *val2 = interpretExpression(arguments[1], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val2->getType() != ValueType::FLOAT && val2->getType() != ValueType::INTEGER) {
        runtimeError("min() takes a float or integer argument");
        delete val1;
        delete val2;
        return ERROR_EXIT;
    }

    float value1 = (val1->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val1)->getValue() : ((ReturnableFloat *)val1)->getValue();
    float value2 = (val2->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val2)->getValue() : ((ReturnableFloat *)val2)->getValue();

    delete val1;
    delete val2;

    return new ReturnableFloat(std::min(value1, value2));
}

ReturnableObject *Interpreter::_max(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there are exactly two arguments
    if (arguments.size() != 2) {
        runtimeError("max() takes exactly two arguments");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val1 = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val1->getType() != ValueType::FLOAT && val1->getType() != ValueType::INTEGER) {
        runtimeError("max() takes a float or integer argument");
        delete val1;
        return ERROR_EXIT;
    }

    // Get the second argument
    ReturnableObject *val2 = interpretExpression(arguments[1], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val2->getType() != ValueType::FLOAT && val2->getType() != ValueType::INTEGER) {
        runtimeError("max() takes a float or integer argument");
        delete val1;
        delete val2;
        return ERROR_EXIT;
    }

    float value1 = (val1->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val1)->getValue() : ((ReturnableFloat *)val1)->getValue();
    float value2 = (val2->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val2)->getValue() : ((ReturnableFloat *)val2)->getValue();

    delete val1;
    delete val2;

    return new ReturnableFloat(std::max(value1, value2));
}

ReturnableObject *Interpreter::_log(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("log() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val->getType() != ValueType::FLOAT && val->getType() != ValueType::INTEGER) {
        runtimeError("log() takes a float or integer argument");
        delete val;
        return ERROR_EXIT;
    }

    float value = (val->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    delete val;

    if (value < 0) {
        runtimeError("log() takes a positive argument");
        return ERROR_EXIT;
    }

    return new ReturnableFloat(log(value));
}

ReturnableObject *Interpreter::_log10(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("log10() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val1 = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val1->getType() != ValueType::FLOAT && val1->getType() != ValueType::INTEGER) {
        runtimeError("log10() takes a float or integer argument");
        delete val1;
        return ERROR_EXIT;
    }

    float value1 = (val1->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val1)->getValue() : ((ReturnableFloat *)val1)->getValue();

    delete val1;

    if (value1 < 0) {
        runtimeError("log10() takes a positive argument");
        return ERROR_EXIT;
    }

    return new ReturnableFloat(log10(value1));
}

ReturnableObject *Interpreter::_log2(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 1) {
        runtimeError("log2() takes exactly one argument");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val1 = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val1->getType() != ValueType::FLOAT && val1->getType() != ValueType::INTEGER) {
        runtimeError("log2() takes a float or integer argument");
        delete val1;
        return ERROR_EXIT;
    }

    float value1 = (val1->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val1)->getValue() : ((ReturnableFloat *)val1)->getValue();

    delete val1;

    if (value1 < 0) {
        runtimeError("log2() takes a positive argument");
        return ERROR_EXIT;
    }

    return new ReturnableFloat(log2(value1));
}

ReturnableObject *Interpreter::_round(std::vector<ASTNode *> &arguments, std::vector<StackFrame *> &stack) {
    // Check if there is exactly one argument
    if (arguments.size() != 2) {
        runtimeError("round() takes exactly two arguments");
        return ERROR_EXIT;
    }

    // Get the first argument
    ReturnableObject *val1 = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val1->getType() != ValueType::FLOAT && val1->getType() != ValueType::INTEGER) {
        runtimeError("round() takes a float or integer argument");
        delete val1;
        return ERROR_EXIT;
    }

    // Get the second argument
    ReturnableObject *val2 = interpretExpression(arguments[1], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val2->getType() != ValueType::FLOAT && val2->getType() != ValueType::INTEGER) {
        runtimeError("round() takes a float or integer argument");
        delete val1;
        delete val2;
        return ERROR_EXIT;
    }

    float value1 = (val1->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val1)->getValue() : ((ReturnableFloat *)val1)->getValue();
    float value2 = (val2->getType() == ValueType::INTEGER) ? ((ReturnableInt *)val2)->getValue() : ((ReturnableFloat *)val2)->getValue();

    delete val1;
    delete val2;

    float factor = pow(10, value2);

    return new ReturnableFloat(round(value1 * factor) / factor);
}

ReturnableObject* Interpreter::_sendBool(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack) {
    // Check if there are exactly two arguments
    if (arguments.size() != 2) {
        runtimeError("send_bool() takes exactly two arguments");
        return ERROR_EXIT;
    }

    // Get the first argument -- the pin
    ReturnableObject* val1 = interpretExpression(arguments[0], stack);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_EXIT;
    }

    if (val1->getType() != ValueType::INTEGER) {
        runtimeError("send_bool()'s first argument must be an integer");
        delete val1;
        return ERROR_EXIT;
    }

    // Get the second argument -- the value
    ReturnableObject* val2 = interpretExpression(arguments[1], stack);

    if (errorHandler.shouldStopExecution()) {
        delete val1;
        return ERROR_EXIT;
    }

    bool value = interpretTruthiness(val2, stack);

    if (errorHandler.shouldStopExecution()) {
        delete val1;
        delete val2;
        return ERROR_EXIT;
    }

    int tileIdx = ((ReturnableInt*)val1)->getValue();

    // Send the data command over radio
    #if __EMBEDDED__
    radioFormatter->send_bool(tileIdx, value);
    #else
    delete val1;
    delete val2;
    runtimeError("send_bool() is only available in embedded mode");
    #endif

    delete val1;
    delete val2;

    return new ReturnableInt(0);

}

//===================================================

ReturnableFloat::ReturnableFloat(float value) : value(value) {}

ValueType ReturnableFloat::getType() {
    return ValueType::FLOAT;
}

float ReturnableFloat::getValue() {
    return value;
}

ReturnableFloat::~ReturnableFloat() {}

ReturnableInt::ReturnableInt(int value) : value(value) {}

ValueType ReturnableInt::getType() {
    return ValueType::INTEGER;
}

int ReturnableInt::getValue() {
    return value;
}

ReturnableInt::~ReturnableInt() {}

ExitingBreak::ExitingBreak() {}

ExitingType ExitingBreak::getType() {
    return ExitingType::BREAK;
}

ExitingBreak::~ExitingBreak() {}

ExitingContinue::ExitingContinue() {}

ExitingType ExitingContinue::getType() {
    return ExitingType::CONTINUE;
}

ExitingContinue::~ExitingContinue() {}

ExitingReturn::ExitingReturn(ReturnableObject *value) : value(value) {}

ExitingReturn::ExitingReturn() : value(nullptr) {}

ExitingType ExitingReturn::getType() {
    return ExitingType::RETURN;
}

ReturnableObject *ExitingReturn::getValue() {
    return value;
}

ExitingReturn::~ExitingReturn() {
    // Do not delete value because it is needed elsewhere
}

ExitingNone::ExitingNone() {}

ExitingType ExitingNone::getType() {
    return ExitingType::NONE;
}

ExitingNone::~ExitingNone() {}
