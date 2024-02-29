#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <functional>
#include <map>
#include <unordered_map>
#include <vector>

#include "ast.hpp"
#include "error.hpp"
#include "outputStream.hpp"
#include "radioFormatter.hpp"

#define PI 3.14159265358979323846

#define MAX_RECURSION_DEPTH 4

// Tags for the various exiting types
enum class ExitingType {
    BREAK,     // break out of the current loop
    CONTINUE,  // continue to the next iteration of the current loop
    RETURN,    // return a value from a function
    NONE       // no return value (e.g. for print statements or an if statement without break or continue)
};

// Tags for the various types of values that can be stored in a variable
enum class ValueType {
    INTEGER,
    FLOAT,
    FUNCTION
};

class StackFrame {
   public:
    StackFrame(StackFrame* parent, OutputStream& outputStream, ErrorHandler& errorHandler);
    ~StackFrame();

    void allocateFloatVariable(std::string& name, float value);
    void allocateIntVariable(std::string& name, int value);
    void allocateFunction(std::string& name, FunctionDeclarationNode* function);

    void setFloatVariable(std::string& name, float value);
    void setIntVariable(std::string& name, int value);
    // No setFunction

    float getFloatVariable(std::string& name);
    int getIntVariable(std::string& name);
    FunctionDeclarationNode* getFunction(std::string& name);

    // What type is stored in the variable
    ValueType getType(std::string& name);

    // Check if a variable is allocated in any of the stack frames
    bool isAllocated(std::string& name);

    std::map<std::string, FunctionDeclarationNode*>& getFunctions();

   private:
    std::map<std::string, float> float_variables;
    std::map<std::string, int> int_variables;
    std::map<std::string, FunctionDeclarationNode*> functions;
    StackFrame* parent;
    OutputStream& outputStream;
    ErrorHandler& errorHandler;
};

// The objects returned by expressions or statements or functions
class ReturnableObject {
   public:
    virtual ValueType getType() = 0;
    virtual ~ReturnableObject() = default;
};

class ReturnableFloat : public ReturnableObject {
   public:
    ReturnableFloat(float value);
    ValueType getType() override;
    float getValue();
    ~ReturnableFloat();

   private:
    float value;
};

class ReturnableInt : public ReturnableObject {
   public:
    ReturnableInt(int value);
    ValueType getType() override;
    int getValue();
    ~ReturnableInt();

   private:
    int value;
};

// The objects returned by scopes and blocks, carried through the likes of if, while, for, function calls
// Ex break, continue, return
class ExitingObject {
   public:
    virtual ExitingType getType() = 0;
    virtual ~ExitingObject() = default;
};

class ExitingBreak : public ExitingObject {
   public:
    ExitingBreak();
    ExitingType getType() override;
    ~ExitingBreak();
};

class ExitingContinue : public ExitingObject {
   public:
    ExitingContinue();
    ExitingType getType() override;
    ~ExitingContinue();
};

class ExitingReturn : public ExitingObject {
   public:
    ExitingReturn();
    ExitingReturn(ReturnableObject* value);
    ExitingType getType() override;
    ReturnableObject* getValue();
    ~ExitingReturn();

   private:
    ReturnableObject* value;
};

class ExitingNone : public ExitingObject {
   public:
    ExitingNone();
    ExitingType getType() override;
    ~ExitingNone();
};

class Interpreter {
   public:
    Interpreter(BlockNode& ast, OutputStream& outputStream, ErrorHandler& errorHandler);
    Interpreter(BlockNode& ast, OutputStream& outputStream, ErrorHandler& errorHandler, RadioFormatter& radioFormatter);
    void interpret();
    ~Interpreter();

   private:
    BlockNode& ast;
    OutputStream& outputStream;
    ErrorHandler& errorHandler;
    RadioFormatter* radioFormatter;
    uint8_t recursionDepth;

    using FunctionPtr = std::function<ReturnableObject*(std::vector<ASTNode*>&, std::vector<StackFrame*>&)>;
    std::unordered_map<std::string, FunctionPtr> functionMap;

    void initBuiltInFunctions();

    void runtimeError(const std::string& message) const;

    void interpretVariableDeclaration(VariableDeclarationNode* variableDeclaration, std::vector<StackFrame*>& stack);
    void interpretAssignment(AssignmentNode* assignment, std::vector<StackFrame*>& stack);
    void interpretFunctionDeclaration(FunctionDeclarationNode* functionDeclaration, std::vector<StackFrame*>& stack);

    bool interpretTruthiness(ReturnableObject* condition, std::vector<StackFrame*>& stack);

    // Expressions can return a ReturnableFloat or a ReturnableInt
    ReturnableObject* interpretExpression(ASTNode* expression, std::vector<StackFrame*>& stack);
    ReturnableObject* interpretVariableAccess(VariableAccessNode* variableAccess, std::vector<StackFrame*>& stack);
    ReturnableObject* interpretBinaryOperation(BinaryOperationNode* binaryExpression, std::vector<StackFrame*>& stack);
    ReturnableObject* interpretNumber(NumberNode* number, std::vector<StackFrame*>& stack);
    ReturnableObject* interpretFunctionCall(FunctionCallNode* functionCall, std::vector<StackFrame*>& stack);

    // Statements with block nodes can possibly return a ExitingObject such as ExitingBreak, ExitingContinue, ExitingReturn
    ExitingObject* interpretStatement(ASTNode* statement, std::vector<StackFrame*>& stack);
    ExitingObject* interpretBlock(BlockNode* block, std::vector<StackFrame*>& stack);
    ExitingObject* interpretIf(IfNode* ifStatement, std::vector<StackFrame*>& stack);
    ExitingObject* interpretWhile(WhileNode* whileStatement, std::vector<StackFrame*>& stack);
    ExitingObject* interpretFor(ForNode* forStatement, std::vector<StackFrame*>& stack);
    ExitingObject* interpretReturn(ReturnNode* returnStatement, std::vector<StackFrame*>& stack);
    // continue and break do not need dedicated functions because they don't have any associated values as does return

    // Built-in functions
    ReturnableObject* _print(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);         // print to output stream
    ReturnableObject* _wait(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);          // wait for a given number of milliseconds
    ReturnableObject* _rand(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);          // returns a random number [0, 1)
    ReturnableObject* _float_to_int(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);  // convert a float to an int
    ReturnableObject* _int_to_float(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);  // convert an int to a float
    ReturnableObject* _runtime(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);       // return the time since interpretation start in milliseconds
    ReturnableObject* _pow(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);           // return the first argument raised to the power of the second argument
    ReturnableObject* _pi(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);            // return the value of pi
    ReturnableObject* _exp(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);           // return the value of pi
    ReturnableObject* _sin(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);           // return the sine of the argument
    ReturnableObject* _cos(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);           // return the cosine of the argument
    ReturnableObject* _tan(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);           // return the tangent of the argument
    ReturnableObject* _asin(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);          // return the arcsine of the argument
    ReturnableObject* _acos(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);          // return the arccosine of the argument
    ReturnableObject* _atan(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);          // return the arctangent of the argument
    ReturnableObject* _atan2(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);         // return the atan2 of the two arguments
    ReturnableObject* _sqrt(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);          // return the square root of the argument
    ReturnableObject* _abs(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);           // return the absolute value of the argument
    ReturnableObject* _floor(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);         // return the floor of the argument
    ReturnableObject* _ceil(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);          // return the ceiling of the argument
    ReturnableObject* _min(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);           // return the minimum of the two arguments
    ReturnableObject* _max(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);           // return the maximum of the two arguments
    ReturnableObject* _log(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);           // return the natural logarithm of the argument
    ReturnableObject* _log10(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);         // return the base 10 logarithm of the argument
    ReturnableObject* _log2(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);          // return the base 2 logarithm of the argument
    ReturnableObject* _round(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);         // returns the first argument rounded to the number of decimal places specified by the second argument

    // Built-in tile functions
    ReturnableObject* _sendBool(std::vector<ASTNode*>& arguments, std::vector<StackFrame*>& stack);  // send a boolean value to a tile; argument 1 is the tile index, argument 2 is the value
};

#endif  // INTERPRETER_HPP