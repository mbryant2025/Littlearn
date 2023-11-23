#include "interpreter.hpp"
#include "tokenizer.hpp"
#include <thread>
#include <math.h>
#include "error.hpp"

#if __EMBEDDED__
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <RTClib.h>
#include "Adafruit_LEDBackpack.h"
#define DISPLAY_ADDRESS 0x70
Adafruit_7segment segDisplay = Adafruit_7segment();
#endif // __EMBEDDED__

StackFrame::StackFrame(StackFrame *parent) : parent(parent)
{
}

StackFrame::~StackFrame() {}

void StackFrame::allocateFloatVariable(std::string name, float value)
{

    // Check if the variable already exists in either the int or float maps
    if (float_variables.find(name) != float_variables.end() || int_variables.find(name) != int_variables.end())
    {
        handleError("Runtime Error: Variable " + name + " already exists in this scope");
    }

    float_variables[name] = value;
}

void StackFrame::allocateIntVariable(std::string name, int value)
{

    // Check if the variable already exists in either the int or float maps
    if (float_variables.find(name) != float_variables.end() || int_variables.find(name) != int_variables.end())
    {
        handleError("Runtime Error: Variable " + name + " already exists in this scope");
    }

    int_variables[name] = value;
}

void StackFrame::setFloatVariable(std::string name, float value)
{

    // Check if the variable exists in the float map
    if (float_variables.find(name) != float_variables.end())
    {
        float_variables[name] = value;
    }
    else if (parent != nullptr)
    {
        parent->setFloatVariable(name, value);
    }
    else
    {
        handleError("Runtime Error: Variable " + name + " does not exist in this scope");
    }
}

void StackFrame::setIntVariable(std::string name, int value)
{

    // Check if the variable exists in the int map
    if (int_variables.find(name) != int_variables.end())
    {
        int_variables[name] = value;
    }
    else if (parent != nullptr)
    {
        parent->setIntVariable(name, value);
    }
    else
    {
        handleError("Runtime Error: Variable " + name + " does not exist in this scope");
    }
}

float StackFrame::getFloatVariable(std::string name)
{

    // Check if the variable exists in the float map
    if (float_variables.find(name) != float_variables.end())
    {
        return float_variables[name];
    }
    else if (parent != nullptr)
    {
        return parent->getFloatVariable(name);
    }
    else
    {
        handleError("Runtime Error: Variable " + name + " does not exist in this scope");
        return 0.0;
    }
}

int StackFrame::getIntVariable(std::string name)
{

    // Check if the variable exists in the int map
    if (int_variables.find(name) != int_variables.end())
    {
        return int_variables[name];
    }
    else if (parent != nullptr)
    {
        return parent->getIntVariable(name);
    }
    else
    {
        handleError("Runtime Error: Variable " + name + " does not exist in this scope");
        return 0;
    }
}

ReturnableType StackFrame::getType(std::string name)
{

    // Check if the variable exists in the float map
    if (float_variables.find(name) != float_variables.end())
    {
        return ReturnableType::FLOAT;
    }
    else if (int_variables.find(name) != int_variables.end())
    {
        return ReturnableType::INTEGER;
    }
    else if (parent != nullptr)
    {
        return parent->getType(name);
    }
    else
    {
        handleError("Runtime Error: Variable " + name + " does not exist in this scope");
        return ReturnableType::INTEGER; //Default to int
    }
}

Interpreter::Interpreter(BlockNode *ast) : ast(ast)
{

    // For embedded mode, initialize the 7 segment
#if __EMBEDDED__
    segDisplay.begin(DISPLAY_ADDRESS);
#endif
}

void Interpreter::interpret()
{

    // Check if we should stop execution
    if (shouldStopExecution())
    {
        return;
    }

    // Create a stack frame for the global scope
    StackFrame *globalScope = new StackFrame(nullptr);

    // Create a vector of stack frames
    std::vector<StackFrame *> stack;
    stack.push_back(globalScope);

    interpretBlock(ast, stack);

    delete globalScope;
    delete ast;
}

Interpreter::~Interpreter() {}

ReturnableType Interpreter::interpretBlock(BlockNode *block, std::vector<StackFrame *> &stack)
{

    // Check if we should stop execution
    if (shouldStopExecution())
    {
        return ReturnableType::NONE;
    }

    // Create a new stack frame
    StackFrame *frame = new StackFrame(stack.back());

    // Push the new stack frame onto the stack
    stack.push_back(frame);

    // Interpret each statement in the block
    for (ASTNode *statement : (*block).getStatements())
    {
        
        // Check for break or continue
        if(statement->getNodeType() == ASTNodeType::BREAK_NODE)
        {
            // Pop the stack frame off the stack
            stack.pop_back();

            // Delete the stack frame
            delete frame;

            // Return a break
            return ReturnableType::BREAK;
        }
        else if(statement->getNodeType() == ASTNodeType::CONTINUE_NODE)
        {
            // Pop the stack frame off the stack
            stack.pop_back();

            // Delete the stack frame
            delete frame;

            // Return a continue
            return ReturnableType::CONTINUE;
        }
        
        interpretStatement(statement, stack);
    }

    // Pop the stack frame off the stack
    stack.pop_back();

    // Delete the stack frame
    delete frame;

    return ReturnableType::NONE;
}

void Interpreter::interpretStatement(ASTNode *statement, std::vector<StackFrame *> &stack)
{

    // Check if we should stop execution
    if (shouldStopExecution())
    {
        return;
    }

    // These are the statements that represent "start points"
    // In other words, statements that do not return a value

    // Avoids RTTI by using virtual function to getNodeType

    switch (statement->getNodeType())
    {
    case ASTNodeType::VARIABLE_DECLARATION_NODE:
        interpretVariableDeclaration((VariableDeclarationNode *)statement, stack);
        break;

    case ASTNodeType::ASSIGNMENT_NODE:
        interpretAssignment((AssignmentNode *)statement, stack);
        break;

    case ASTNodeType::BLOCK_NODE:
        interpretBlock((BlockNode *)statement, stack);
        break;

    case ASTNodeType::PRINT_NODE:
        interpretPrint((PrintNode *)statement, stack);
        break;

    case ASTNodeType::BINARY_OPERATION_NODE:
        interpretBinaryOperation((BinaryOperationNode *)statement, stack);
        break;

    case ASTNodeType::IF_NODE:
        interpretIf((IfNode *)statement, stack);
        break;

    case ASTNodeType::WHILE_NODE:
        interpretWhile((WhileNode *)statement, stack);
        break;

    case ASTNodeType::WAIT_NODE:
        interpretWait((WaitNode *)statement, stack);
        break;

    case ASTNodeType::SEVEN_SEGMENT_NODE:
        interpretPrintSevenSegment((SevenSegmentNode *)statement, stack);
        break;

    case ASTNodeType::WRITE_PORT_NODE:
        interpretWritePort((WritePortNode *)statement, stack);
        break;

    default:
        handleError("Unknown statement type " + statement->toString());
    }
}

ReturnableObject *Interpreter::interpretExpression(ASTNode *expression, std::vector<StackFrame *> &stack)
{

    // These are the expressions that return a value, such as a variable access or a binary operation

    switch (expression->getNodeType())
    {
    case ASTNodeType::VARIABLE_ACCESS_NODE:
        return interpretVariableAccess((VariableAccessNode *)expression, stack);

    case ASTNodeType::BINARY_OPERATION_NODE:
        return interpretBinaryOperation((BinaryOperationNode *)expression, stack);

    case ASTNodeType::NUMBER_NODE:
        return interpretNumber((NumberNode *)expression, stack);

    case ASTNodeType::READ_PORT_NODE:
        return interpretReadPort((ReadPortNode *)expression, stack);

    default:
        handleError("Unknown expression type " + expression->toString());
    }

    // Not reached
    return nullptr;
}

ReturnableObject *Interpreter::interpretNumber(NumberNode *number, std::vector<StackFrame *> &stack)
{

    // Get the type of the number
    std::string type = number->getType() == TokenType::INTEGER ? "int" : "float";

    // Get the value of the number, cast from string to either int or float
    float value = std::stof(number->getValue());

    if (type == "int")
    {
        // Create a new returnable int
        ReturnableInt *returnableInt = new ReturnableInt((int)value);

        // Return the returnable int
        return returnableInt;
    }
    else if (type == "float")
    {
        // Create a new returnable float
        ReturnableFloat *returnableFloat = new ReturnableFloat(value);

        // Return the returnable float
        return returnableFloat;
    }
    else
    {
        handleError("Unknown number type " + type);
    }

    // Not reached
    return nullptr;
}

ReturnableObject *Interpreter::interpretVariableAccess(VariableAccessNode *variableAccess, std::vector<StackFrame *> &stack)
{

    // Get the identifier
    std::string identifier = variableAccess->getIdentifier();

    // Get the type of the variable
    ReturnableType type = stack.back()->getType(identifier);

    if (type == ReturnableType::INTEGER)
    {
        // Get the int variable
        int value = stack.back()->getIntVariable(identifier);

        // Create a new returnable int
        ReturnableInt *returnableInt = new ReturnableInt(value);

        // Return the returnable int
        return returnableInt;
    }
    else if (type == ReturnableType::FLOAT)
    {
        // Get the float variable
        float value = stack.back()->getFloatVariable(identifier);

        // Create a new returnable float
        ReturnableFloat *returnableFloat = new ReturnableFloat(value);

        // Return the returnable float
        return returnableFloat;
    }
    else
    {
        handleError("Unknown variable type " + identifier);
    }

    // Not reached
    return nullptr;
}

ReturnableObject *Interpreter::interpretBinaryOperation(BinaryOperationNode *binaryExpression, std::vector<StackFrame *> &stack)
{

    // Get the left and right expressions
    ASTNode *leftExpression = binaryExpression->getLeftExpression();
    ASTNode *rightExpression = binaryExpression->getRightExpression();

    // Evaluate the left and right expressions
    ReturnableObject *left = interpretExpression(leftExpression, stack);
    ReturnableObject *right = interpretExpression(rightExpression, stack);

    // Perform the binary operation

    // First check if either are floats -- if so then we need to convert both to floats
    if (left->getType() == ReturnableType::FLOAT || right->getType() == ReturnableType::FLOAT)
    {
        // Convert the left and right to floats
        float leftFloat = 0.0;
        float rightFloat = 0.0;

        if (left->getType() == ReturnableType::FLOAT)
        {
            leftFloat = ((ReturnableFloat *)left)->getValue();
        }
        else
        {
            leftFloat = ((ReturnableFloat *)left)->getValue();
        }

        if (right->getType() == ReturnableType::FLOAT)
        {
            rightFloat = ((ReturnableFloat *)right)->getValue();
        }
        else
        {
            rightFloat = ((ReturnableInt *)right)->getValue();
        }

        delete left;
        delete right;

        // Get the operator
        std::string op = binaryExpression->getOperator();

        // Check if the operator is +
        if (op == "+")
        {
            // Create a new float node with the sum of the left and right floats
            ReturnableFloat *sum = new ReturnableFloat(leftFloat + rightFloat);

            return sum;
        }
        else if (op == "-")
        {
            // Create a new float node with the difference of the left and right floats
            ReturnableFloat *difference = new ReturnableFloat(leftFloat - rightFloat);

            return difference;
        }
        else if (op == "*")
        {
            // Create a new float node with the product of the left and right floats
            ReturnableFloat *product = new ReturnableFloat(leftFloat * rightFloat);

            return product;
        }
        else if (op == "/")
        {
            // Create a new float node with the quotient of the left and right floats
            ReturnableFloat *quotient = new ReturnableFloat(leftFloat / rightFloat);

            return quotient;
        }
        else if (op == ">")
        {
            // Create a new int node with the result of the left and right floats
            ReturnableInt *result = new ReturnableInt(leftFloat > rightFloat);

            return result;
        }
        else if (op == "<")
        {
            // Create a new int node with the result of the left and right floats
            ReturnableInt *result = new ReturnableInt(leftFloat < rightFloat);

            return result;
        }
        else if (op == "%")
        {
            // Create a new int node with the result of the left and right floats
            ReturnableInt *result = new ReturnableInt(std::fmod(leftFloat, rightFloat));

            return result;
        }
        else
        {
            handleError("Unknown operator " + op);
        }
    }
    else
    {
        // Convert the left and right to ints
        int leftInt = 0;
        int rightInt = 0;

        if (left->getType() == ReturnableType::INTEGER)
        {
            leftInt = ((ReturnableInt *)left)->getValue();
        }
        else
        {
            leftInt = ((ReturnableFloat *)left)->getValue();
        }

        if (right->getType() == ReturnableType::INTEGER)
        {
            rightInt = ((ReturnableInt *)right)->getValue();
        }
        else
        {
            rightInt = ((ReturnableFloat *)right)->getValue();
        }

        delete left;
        delete right;

        // Get the operator
        std::string op = binaryExpression->getOperator();

        // Check if the operator is +
        if (op == "+")
        {
            // Create a new int node with the sum of the left and right ints
            ReturnableInt *sum = new ReturnableInt(leftInt + rightInt);

            return sum;
        }
        else if (op == "-")
        {
            // Create a new int node with the difference of the left and right ints
            ReturnableInt *difference = new ReturnableInt(leftInt - rightInt);

            return difference;
        }
        else if (op == "*")
        {
            // Create a new int node with the product of the left and right ints
            ReturnableInt *product = new ReturnableInt(leftInt * rightInt);

            return product;
        }
        else if (op == "/")
        {
            // Create a new int node with the quotient of the left and right ints
            ReturnableInt *quotient = new ReturnableInt(leftInt / rightInt);

            return quotient;
        }
        else if (op == ">")
        {
            // Create a new int node with the result of the left and right ints
            ReturnableInt *result = new ReturnableInt(leftInt > rightInt);

            return result;
        }
        else if (op == "<")
        {
            // Create a new int node with the result of the left and right ints
            ReturnableInt *result = new ReturnableInt(leftInt < rightInt);

            return result;
        }
        else if (op == "%")
        {
            // Create a new int node with the result of the left and right ints
            ReturnableInt *result = new ReturnableInt(leftInt % rightInt);

            return result;
        }
        else
        {
            handleError("Unknown operator " + op);
        }
    }

    // Not reached
    return nullptr;
}

void Interpreter::interpretVariableDeclaration(VariableDeclarationNode *variableDeclaration, std::vector<StackFrame *> &stack)
{

    if (shouldStopExecution())
    {
        return;
    }

    ReturnableObject *val = interpretExpression(variableDeclaration->getInitializer(), stack);

    std::string type = variableDeclaration->getType();

    // Handle both types as a float and cast to the appropriate type later
    float value = val->getType() == ReturnableType::INTEGER ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    delete val;

    if (type == "int")
    {
        // Allocate the int variable
        stack.back()->allocateIntVariable(variableDeclaration->getIdentifier(), (int)value);
    }
    else if (type == "float")
    {
        // Allocate the float variable
        stack.back()->allocateFloatVariable(variableDeclaration->getIdentifier(), (float)value);
    }
    else
    {
        handleError("Unknown variable type " + type);
    }
}

void Interpreter::interpretAssignment(AssignmentNode *assignment, std::vector<StackFrame *> &stack)
{

    if (shouldStopExecution())
    {
        return;
    }

    ReturnableObject *val = interpretExpression(assignment->getExpression(), stack);

    ReturnableType type = stack.back()->getType(assignment->getIdentifier());

    // Handle both types as a float and cast to the appropriate type later
    float value = val->getType() == ReturnableType::INTEGER ? ((ReturnableInt *)val)->getValue() : ((ReturnableFloat *)val)->getValue();

    if (type == ReturnableType::INTEGER)
    {
        // Set the int variable
        stack.back()->setIntVariable(assignment->getIdentifier(), (int)value);
    }
    else if (type == ReturnableType::FLOAT)
    {
        // Set the float variable
        stack.back()->setFloatVariable(assignment->getIdentifier(), (float)value);
    }
    else
    {
        delete val;
        handleError("Unknown variable type for " + assignment->getIdentifier());
    }

    delete val;
}

void Interpreter::interpretPrint(ASTNode *expression, std::vector<StackFrame *> &stack)
{

    if (shouldStopExecution())
    {
        return;
    }

    // Cast to a print node
    PrintNode *printNode = (PrintNode *)expression;

    if (printNode == nullptr)
    {
        handleError("Unknown expression type " + expression->toString());
    }

    // We need to evaluate the expression
    ReturnableObject *returnableObject = interpretExpression(printNode->getExpression(), stack);

    // At this point we know the type of the returnable object to be either an int or a float
    if (returnableObject->getType() == ReturnableType::INTEGER)
    {
// Print the int
#if __EMBEDDED__
        // TODO transition to Bluetooth
        Serial.println(((ReturnableInt *)returnableObject)->getValue());
#else
        std::cout << ((ReturnableInt *)returnableObject)->getValue() << "\n";
#endif
    }
    else if (returnableObject->getType() == ReturnableType::FLOAT)
    {
// Print the float
#if __EMBEDDED__
        // TODO transition to Bluetooth
        Serial.println(((ReturnableFloat *)returnableObject)->getValue());
#else
        std::cout << ((ReturnableFloat *)returnableObject)->getValue() << "\n";
#endif
    }
    else
    {
        delete returnableObject;
        handleError("Unknown returnable object type for print call");
    }

    delete returnableObject;
}

void Interpreter::interpretWait(ASTNode *expression, std::vector<StackFrame *> &stack)
{

    if (shouldStopExecution())
    {
        return;
    }

    // Cast to a wait node
    WaitNode *waitNode = (WaitNode *)expression;

    if (waitNode == nullptr)
    {
        handleError("Unknown expression type " + expression->toString());
    }

    // We need to evaluate the expression
    ReturnableObject *returnableObject = interpretExpression(waitNode->getExpression(), stack);

    // At this point we know the type of the returnable object to be either an int or a float
    if (returnableObject->getType() == ReturnableType::INTEGER)
    {
        // Wait for the int
        std::this_thread::sleep_for(std::chrono::milliseconds(((ReturnableInt *)returnableObject)->getValue()));
        delete returnableObject;
    }
    else if (returnableObject->getType() == ReturnableType::FLOAT)
    {
        delete returnableObject;
        handleError("Cannot wait for a float. Use an integer number of milliseconds instead.");
    }
    else
    {
        delete returnableObject;
        handleError("Unknown returnable object type for wait call");
    }
}

void Interpreter::interpretPrintSevenSegment(ASTNode *expression, std::vector<StackFrame *> &stack)
{

    if (shouldStopExecution())
    {
        return;
    }

    // Cast to a print seven segment node
    SevenSegmentNode *sevenSegmentNode = (SevenSegmentNode *)expression;

    if (sevenSegmentNode == nullptr)
    {
        handleError("Unknown expression type " + expression->toString());
    }

    // We need to evaluate the expression
    ReturnableObject *returnableObject = interpretExpression(sevenSegmentNode->getExpression(), stack);

    // At this point we know the type of the returnable object to be either an int or a float
    if (returnableObject->getType() == ReturnableType::INTEGER)
    {
// Write the int to the seven segment
#if __EMBEDDED__
        segDisplay.print(((ReturnableInt *)returnableObject)->getValue());
        segDisplay.writeDisplay();
#else
        std::cout << "Cannot print to seven segment in non-embedded mode.\n";
#endif
        delete returnableObject;
    }
    else if (returnableObject->getType() == ReturnableType::FLOAT)
    {
        delete returnableObject;
        handleError("Cannot print to seven segment for a float. Use an integer number instead.");
    }
    else
    {
        delete returnableObject;
        handleError("Unknown returnable object type for print seven segment call.");
    }
}

ReturnableObject *Interpreter::interpretReadPort(ASTNode *expression, std::vector<StackFrame *> &stack)
{

    // Cast to a read port node
    ReadPortNode *readPortNode = (ReadPortNode *)expression;

    if (readPortNode == nullptr)
    {
        handleError("Unknown expression type " + expression->toString());
    }

    // We need to evaluate the expression
    ReturnableObject *returnableObject = interpretExpression(readPortNode->getExpression(), stack);

    // At this point we know the type of the returnable object to be either an int or a float
    if (returnableObject->getType() == ReturnableType::INTEGER)
    {
// Read the int from the port
#if __EMBEDDED__
        int port = ((ReturnableInt *)returnableObject)->getValue();
        int mappedPort = 0;
        switch (port)
        {
        case 1:
            mappedPort = PORT_1;
            break;
        case 2:
            mappedPort = PORT_2;
            break;
        case 3:
            mappedPort = PORT_3;
            break;
        case 4:
            mappedPort = PORT_4;
            break;
        case 5:
            mappedPort = PORT_5;
            break;
        case 6:
            mappedPort = PORT_6;
            break;
        default:
            handleError("Unknown port number " + port);
            delete returnableObject;
        }
        pinMode(mappedPort, INPUT);
        int value = digitalRead(mappedPort);
        ReturnableInt *returnableInt = new ReturnableInt(value);
        delete returnableObject;
        return returnableInt;
#else
        delete returnableObject;
        std::cout << "Cannot read from port in non-embedded mode. Returning 0 from this function call.\n";
        return new ReturnableInt(0);
#endif
    }
    else if (returnableObject->getType() == ReturnableType::FLOAT)
    {
        delete returnableObject;
        handleError("Cannot read from port for a float. Use an integer port number instead.");
    }
    else
    {
        delete returnableObject;
        handleError("Unknown returnable object type for read port call.");
    }

    // Not reached
    return nullptr;
}

bool Interpreter::interpretTruthiness(ReturnableObject *condition, std::vector<StackFrame *> &stack)
{

    float conditionVal;

    if (condition->getType() == ReturnableType::INTEGER)
    {
        conditionVal = ((ReturnableInt *)condition)->getValue();
    }
    else if (condition->getType() == ReturnableType::FLOAT)
    {
        conditionVal = ((ReturnableFloat *)condition)->getValue();
    }
    else
    {
        handleError("Unknown condition type for interpret truthiness");
    }

    // Check if the condition is true
    // Truthy is value != 0
    if (conditionVal != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Interpreter::interpretIf(IfNode *ifStatement, std::vector<StackFrame *> &stack)
{

    if (shouldStopExecution())
    {
        return;
    }

    // Evaluate the condition
    ReturnableObject *condition = interpretExpression(ifStatement->getExpression(), stack);

    // Check if the condition is true
    if (interpretTruthiness(condition, stack))
    {
        // Interpret the if block
        interpretBlock(ifStatement->getBody(), stack);
    }

    delete condition;
}

void Interpreter::interpretWhile(WhileNode *whileStatement, std::vector<StackFrame *> &stack)
{

    if (shouldStopExecution())
    {
        return;
    }

    // Evaluate the condition
    ReturnableObject *condition = interpretExpression(whileStatement->getExpression(), stack);

    // Check if the condition is true
    while (interpretTruthiness(condition, stack))
    {
        // Interpret the while block
        // Gather the return type to check for break or continue
        ReturnableType returnType = interpretBlock(whileStatement->getBody(), stack);

        delete condition;

        // Check if the return type is a break
        if (returnType == ReturnableType::BREAK)
        {
            // Break out of the while loop

            // We've already deleted the condition, so we can just return
            return;
        }
        else if (returnType == ReturnableType::CONTINUE)
        {
            // Continue the while loop
            // Re-evaluate the condition
            condition = interpretExpression(whileStatement->getExpression(), stack);
            continue;
        }

        // Re-evaluate the condition
        condition = interpretExpression(whileStatement->getExpression(), stack);
    }

    delete condition;
}

void Interpreter::interpretWritePort(WritePortNode *writePort, std::vector<StackFrame *> &stack)
{

    if (shouldStopExecution())
    {
        return;
    }

    if (writePort == nullptr)
    {
        handleError("Unknown expression type " + writePort->toString());
    }

    // Evaluate the expression
    ReturnableObject *port = interpretExpression(writePort->getPort(), stack);
    ReturnableObject *value = interpretExpression(writePort->getValue(), stack);

    // Check if the port and value are ints
    if (port->getType() == ReturnableType::INTEGER && value->getType() == ReturnableType::INTEGER)
    {
// Write the value to the port
#if __EMBEDDED__
        int portNum = ((ReturnableInt *)port)->getValue();
        int mappedPort = 0;
        switch (portNum)
        {
        case 1:
            mappedPort = PORT_1;
            break;
        case 2:
            mappedPort = PORT_2;
            break;
        case 3:
            mappedPort = PORT_3;
            break;
        case 4:
            mappedPort = PORT_4;
            break;
        case 5:
            mappedPort = PORT_5;
            break;
        case 6:
            mappedPort = PORT_6;
            break;
        default:
            handleError("Unknown port number " + portNum);
            delete port;
            delete value;
        }
        pinMode(mappedPort, OUTPUT);
        int valToWrite = interpretTruthiness(value, stack) ? HIGH : LOW;
        digitalWrite(mappedPort, valToWrite);
#else
        std::cout << "Cannot write to port in non-embedded mode.\n";
#endif
        delete port;
        delete value;
    }
    else
    {
        delete port;
        delete value;
        handleError("Cannot write to port for a float. Use an integer port number and value instead.");
    }
}

//===================================================

ReturnableFloat::ReturnableFloat(float value) : value(value) {}

ReturnableType ReturnableFloat::getType()
{
    return ReturnableType::FLOAT;
}

float ReturnableFloat::getValue()
{
    return value;
}

ReturnableFloat::~ReturnableFloat() {}

ReturnableInt::ReturnableInt(int value) : value(value) {}

ReturnableType ReturnableInt::getType()
{
    return ReturnableType::INTEGER;
}

int ReturnableInt::getValue()
{
    return value;
}

ReturnableInt::~ReturnableInt() {}
