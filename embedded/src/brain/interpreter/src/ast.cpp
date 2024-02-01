#include "ast.hpp"
#include "tokenizer.hpp"

// Upon error, return nullptr
// Therefore, all calls expecting an ASTNode* should check for nullptr
// This #define is used to clarify that this is the case

// In general, call the syntaxError function at the source and pass the error
// Therefore, we'll be checking the erroHandler a lot (or equivalently, seeing if we were returned ERROR_NODE)
// Also since we are not calling new, we will only have to delete the nodes that we create when error handling
#define ERROR_NODE nullptr
#define ERROR_VECTOR \
    {}

Parser::Parser(const std::vector<Token>& tokens, OutputStream& outputStream, ErrorHandler& errorHandler) : tokens(tokens), outputStream(outputStream), errorHandler(errorHandler) {
    currentTokenIndex = 0;
}

Parser::~Parser() {
}

void Parser::syntaxError(const std::string& message) const {
    if (currentTokenIndex < tokens.size()) {
        errorHandler.handleError("Syntax Error at token " + std::to_string(currentTokenIndex + 1) + ": " + tokens[currentTokenIndex].lexeme + ": " + message);

    } else {
        errorHandler.handleError("Syntax Error at token " + std::to_string(tokens.size()) + ": " + message);
    }
}

BlockNode* Parser::parseProgram() {
    // The entry point for parsing a program into an AST.
    // Example: Parse a block of code (e.g., the main program)

    // If the program is empty, return an empty block
    if (tokens.size() == 0) {
        return new BlockNode(std::vector<ASTNode*>());
    }

    // If the first and last tokens are not braces, we have an error
    if (tokens[0].type != TokenType::LEFT_BRACE || tokens[tokens.size() - 1].type != TokenType::RIGHT_BRACE) {
        syntaxError("Program must be enclosed in braces.");
        return ERROR_NODE;
    }

    BlockNode* programBlock = parseBlock();

    // Check if there are any remaining tokens; if yes, report an error
    // Avoid this error if we already have an error
    if (currentTokenIndex < tokens.size() && programBlock != ERROR_NODE) {
        syntaxError("Unexpected tokens after the program.");
        delete programBlock;
        return ERROR_NODE;
    }

    // If parseBlock failed, it would be forwarded to here, which is good because we want to return nullptr (ERROR_NODE)
    return programBlock;
}

void Parser::eatToken(TokenType expectedTokenType) {
    // Eat the next token if it matches the expected token type
    // POSTCONDITION Can throw a syntax error if the token types do not match, so checking for errors following is necessary
    if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == expectedTokenType) {
        currentTokenIndex++;
    } else {
        if(currentTokenIndex < tokens.size()) {
            syntaxError("Unexpected token type, expected " + Tokenizer::tokenTypeToString(expectedTokenType) + ", got " + Tokenizer::tokenTypeToString(tokens[currentTokenIndex].type));
        } else {
            syntaxError("Unexpected end of file, expected " + Tokenizer::tokenTypeToString(expectedTokenType));
        }
    }
}

std::vector<const Token*> Parser::gatherTokensUntil(TokenType endTokenType) {
    // Gather tokens until the end token type is reached
    // Precondition: for wrapping syntax, the currentTokenIndex will be
    // immediately after the opening version Ex. If we are looking for } we should
    // be right after the { (which should have been eaten prior to this call)

    // In the case of an error, return an empty vector
    // Therefore, calls to this function should check if the error handler has been tripped

    // If we want a closing parenthesis or brace, we need to keep a counter
    // This counter will go up with every left brace/parenthesis and down with
    // every right brace/parenthesis When the counter reaches 0, we know we have
    // reached the end of the block A syntax error will be thrown if the counter
    // goes below 0 A syntax error will also be thrown if we reach the end of the
    // file without the counter reaching 0 The last case for a syntax error is
    // that of {(})

    int braceParenthesisCounter = 0;

    // This is just a placeholder, it will be overwritten. It also indicates if we
    // are looking for a brace or parenthesis It just keeps track of the opened
    // ones: ( or {
    TokenType lastBraceParenthesis = TokenType::UNKNOWN;

    if (endTokenType == TokenType::RIGHT_BRACE || endTokenType == TokenType::RIGHT_PARENTHESIS) {
        // We want to match, so assume we already ate an opening brace/parenthesis
        braceParenthesisCounter++;
        if (endTokenType == TokenType::RIGHT_BRACE) {
            lastBraceParenthesis = TokenType::LEFT_BRACE;
        } else {
            lastBraceParenthesis = TokenType::LEFT_PARENTHESIS;
        }
    }

    std::vector<const Token*> gatheredTokens;
    // while we can keep going and (we still should go)
    // we should go if (we are not looking for a brace or parenthesis and the
    // current token is not the end token type) OR (we are looking for a brace or
    // parenthesis and the counter is not 0)
    while (currentTokenIndex < tokens.size() && ((lastBraceParenthesis == TokenType::UNKNOWN && tokens[currentTokenIndex].type != endTokenType) || (lastBraceParenthesis != TokenType::UNKNOWN && braceParenthesisCounter != 0))) {
        // If we are looking for a brace or parenthesis, check if the current token
        // is one
        if (lastBraceParenthesis != TokenType::UNKNOWN) {
            TokenType currentTokenType = tokens[currentTokenIndex].type;
            // Increment or decrement the counter
            if (currentTokenType == TokenType::LEFT_BRACE || currentTokenType == TokenType::LEFT_PARENTHESIS) {
                braceParenthesisCounter++;
            } else if (currentTokenType == TokenType::RIGHT_BRACE || currentTokenType == TokenType::RIGHT_PARENTHESIS) {
                braceParenthesisCounter--;

                // If we decremented the counter to 0 and the current token is not the
                // end token type, we have an error
                if (braceParenthesisCounter == 0 && currentTokenType != endTokenType) {
                    syntaxError("Unexpected brace or parenthesis " + tokens[currentTokenIndex].lexeme);
                    return ERROR_VECTOR;
                }

                // If we decremented the counter to -1, we have an error
                if (braceParenthesisCounter < 0) {
                    syntaxError("Unexpected brace or parenthesis " + tokens[currentTokenIndex].lexeme);
                    return ERROR_VECTOR;
                }

                // If we decremented the counter to 0 and the current token is the end
                // token type, we are done
                if (braceParenthesisCounter == 0 && currentTokenType == endTokenType) {
                    break;
                }
            }

            // Check for the condition where we have something like {(})
            if ((currentTokenType == TokenType::RIGHT_BRACE && lastBraceParenthesis == TokenType::LEFT_PARENTHESIS) || (currentTokenType == TokenType::RIGHT_PARENTHESIS && lastBraceParenthesis == TokenType::LEFT_BRACE)) {
                syntaxError("Unexpected brace or parenthesis " + tokens[currentTokenIndex].lexeme);
                return ERROR_VECTOR;
            }

            // If it is an opening brace or parenthesis, update the last
            // brace/parenthesis
            if (currentTokenType == TokenType::LEFT_BRACE || currentTokenType == TokenType::LEFT_PARENTHESIS) {
                lastBraceParenthesis = currentTokenType;
            }
        }

        // Parse a statement
        gatheredTokens.push_back(&tokens[currentTokenIndex]);
        // Only advance the token index if we expect to find the end token type
        currentTokenIndex++;
    }

    // Check if we reached the end token type
    if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == endTokenType) {
        // Add the end token to the gathered tokens as to include it in the vector
        gatheredTokens.push_back(&tokens[currentTokenIndex]);
        // Advance to the next token
        currentTokenIndex++;
    } else {
        // Expected means we expect to find the token
        syntaxError("Unexpected end of file, expected " + Tokenizer::tokenTypeToString(endTokenType));
        return ERROR_VECTOR;
    }

    return gatheredTokens;
}

int Parser::getPrecedence(const std::string& lexeme) {
    // Returns the precedence of the given token type
    // Higher number is higher precedence, specific numbers are arbitrary
    // Based on https://en.cppreference.com/w/cpp/language/operator_precedence

    if (lexeme == "!") {
        return 10;
    } else if (lexeme == "*" || lexeme == "/" || lexeme == "%") {
        return 8;
    } else if (lexeme == "+" || lexeme == "-") {
        return 6;
    } else if (lexeme == ">" || lexeme == "<" || lexeme == "<=" || lexeme == ">=") {
        return 4;
    } else if (lexeme == "==" || lexeme == "!=") {
        return 3;
    } else if (lexeme == "&&") {
        return 2;
    } else if (lexeme == "||") {
        return 1;
    } else {
        syntaxError("Unexpected operator " + lexeme);
        return -1;
    }
}

// Used for below function
#define NUKE_HIGH_LEVEL_NODES          \
    for (auto node : highLevelNodes) { \
        delete node;                   \
    }

ASTNode* Parser::parseExpression(const std::vector<const Token*>& expressionTokens, bool canBeEmpty) {
    // Parses expressions, potentially with parentheses and nested function calls
    // Ex. 5, x, 3.14, 5 + 8, 5 * 8 + x * (4 + 13) * foo(5, 8 - foo(5, 8))

    // Handle the case where there are no tokens
    if (expressionTokens.size() == 0 && !canBeEmpty) {
        syntaxError("Empty (sub)expression");
        return ERROR_NODE;
    }

    // Handle the base case when there is one token
    if (expressionTokens.size() == 1) {
        // Check if the token is a constant or variable access
        if (expressionTokens[0]->type == TokenType::INTEGER || expressionTokens[0]->type == TokenType::FLOAT) {
            // Parse the constant
            return new NumberNode(expressionTokens[0]->lexeme, expressionTokens[0]->type);
        } else if (expressionTokens[0]->type == TokenType::IDENTIFIER) {
            // Parse the variable access
            return new VariableAccessNode(expressionTokens[0]->lexeme);
        } else {
            syntaxError("Unexpected token " + expressionTokens[0]->lexeme);
            return ERROR_NODE;
        }
    }

    // Look at all high-level operations, treating parentheses as a single token
    // Generate a vector of these high-level operations
    // Ex. 5 * 8 + x * (4 + 13) -> {*, +, *}

    // Also maintain a vector of the nodes that correspond to the tokens
    // Ex. 5 * 8 + x * (4 + 13) -> {5, 8, x, (4 + 13)}

    // First, we need to find the highest-level operation
    // We will do this by keeping track of the parentheses

    std::vector<std::string> highLevelOperators;
    std::vector<ASTNode*> highLevelNodes;

    // Keep track of the parentheses
    int parenthesesCounter = 0;

    // Keep track of potential sub-expressions, i.e. stuff in parentheses
    std::vector<const Token*> subExpressionTokens;

    size_t i = 0;
    while (i < expressionTokens.size()) {
        // Get the current token
        const Token* token = expressionTokens[i];

        // If we find a left parenthesis, increment the counter
        if (token->type == TokenType::LEFT_PARENTHESIS) {
            // If we already are a parenthesis in, add the parenthesis to the
            // sub-expression tokens
            if (parenthesesCounter != 0) {
                subExpressionTokens.push_back(token);
            }

            parenthesesCounter++;
        }
        // If we find a right parenthesis, decrement the counter
        else if (token->type == TokenType::RIGHT_PARENTHESIS) {
            // If we already are a parenthesis in, add the parenthesis to the
            // sub-expression tokens
            if (parenthesesCounter != 0) {
                subExpressionTokens.push_back(token);
            }

            parenthesesCounter--;
        }

        // If we find a function call as a high-level node, we need to parse it
        // Functions are indicated by a function header, i.e. a function name and a (
        // Only take this case when it is going to be a high-level node, i.e. when the parentheses counter is 0
        else if (parenthesesCounter == 0 && token->type == TokenType::IDENTIFIER &&
                 i + 1 < expressionTokens.size() && expressionTokens[i + 1]->type == TokenType::LEFT_PARENTHESIS) {
            // Get the function name
            std::string functionName = token->lexeme;

            // Gather function arguments until the proper right parenthesis
            // Keeping track of the index i

            // Eat the name
            i++;

            // Eat the left parenthesis
            i++;

            // Gather the current function argument
            std::vector<const Token*> currentFunctionArgumentTokens;

            // The collected function arguments
            std::vector<ASTNode*> functionArguments;

            // Keep track of the parentheses
            // Initialized to 1 because we already ate the left parenthesis
            int functionParenthesesCounter = 1;

            while (i < expressionTokens.size()) {
                // Get the current token
                const Token* functionToken = expressionTokens[i];

                // If we find a left parenthesis, increment the counter
                if (functionToken->type == TokenType::LEFT_PARENTHESIS) {
                    // If we already are a parenthesis in, add the parenthesis to the
                    // sub-expression tokens

                    if (functionParenthesesCounter != 0) {
                        currentFunctionArgumentTokens.push_back(functionToken);
                    }

                    functionParenthesesCounter++;

                }
                // If we find a right parenthesis, decrement the counter
                else if (functionToken->type == TokenType::RIGHT_PARENTHESIS) {
                    // If we already are a parenthesis in, add the parenthesis to the
                    // sub-expression tokens

                    functionParenthesesCounter--;

                    if (functionParenthesesCounter != 0) {
                        currentFunctionArgumentTokens.push_back(functionToken);
                    }

                }
                // If we find a comma, we have reached the end of the current function
                // argument
                // Of course, we only do this if we are not in a sub-expression
                else if (functionToken->type == TokenType::COMMA && functionParenthesesCounter == 1) {
                    // If we are not back to 1 parentheses (the opening left), throw an error
                    if (functionParenthesesCounter != 1) {
                        syntaxError("Unexpected comma " + functionToken->lexeme);
                        NUKE_HIGH_LEVEL_NODES
                        // Nuke function arguments
                        for (auto arg : functionArguments) {
                            delete arg;
                        }
                        return ERROR_NODE;
                    }

                    // Parse the current function argument
                    ASTNode* currentFunctionArgument = parseExpression(currentFunctionArgumentTokens, false);

                    if (currentFunctionArgument == ERROR_NODE) {
                        NUKE_HIGH_LEVEL_NODES
                        // Nuke function arguments
                        for (auto arg : functionArguments) {
                            delete arg;
                        }
                        return ERROR_NODE;
                    }

                    // Add the current function argument to the vector
                    functionArguments.push_back(currentFunctionArgument);

                    // Clear the current function argument tokens
                    currentFunctionArgumentTokens.clear();
                }
                // Add everything else to the current function argument tokens
                else {
                    currentFunctionArgumentTokens.push_back(functionToken);
                }

                // If we get back to 0 parentheses, we have reached the end of the
                // function call
                if (functionParenthesesCounter == 0) {
                    // Parse the current function argument

                    ASTNode* currentFunctionArgument = parseExpression(currentFunctionArgumentTokens, true); // true because we can have an empty function argument

                    if (currentFunctionArgument == ERROR_NODE) {
                        NUKE_HIGH_LEVEL_NODES
                        // Nuke function arguments
                        for (auto arg : functionArguments) {
                            delete arg;
                        }
                        return ERROR_NODE;
                    }

                    // Add the current function argument to the vector
                    functionArguments.push_back(currentFunctionArgument);

                    // Clear the current function argument tokens
                    currentFunctionArgumentTokens.clear();

                    // Create the function call node
                    FunctionCallNode* functionCallNode = new FunctionCallNode(functionName, functionArguments);

                    if (functionCallNode == ERROR_NODE) {
                        NUKE_HIGH_LEVEL_NODES
                        // Nuke function arguments
                        for (auto arg : functionArguments) {
                            delete arg;
                        }
                        return ERROR_NODE;
                    }

                    // Add the function call node to the high-level nodes
                    highLevelNodes.push_back(functionCallNode);

                    // Clear the function arguments
                    functionArguments.clear();

                    // Also add the high-level operator (the rest of the logic build up Tokens rather than an ASTNode)
                    if (i + 1 < expressionTokens.size() && expressionTokens[i + 1]->type == TokenType::OPERATOR) {
                        highLevelOperators.push_back(expressionTokens[i + 1]->lexeme);
                        i++;
                    }

                    // Break out of the while loop
                    break;
                }

                // Increment the index
                i++;
            }
        }

        // If we find an operator, check if the parentheses counter is 0
        // If it is, we have found a high-level operator
        else if (token->type == TokenType::OPERATOR && parenthesesCounter == 0) {
            // Add the operator to the vector
            highLevelOperators.push_back(token->lexeme);

            // Parse the sub-expression
            ASTNode* subExpression = parseExpression(subExpressionTokens, false);

            if (subExpression == ERROR_NODE) {
                NUKE_HIGH_LEVEL_NODES
                return ERROR_NODE;
            }

            // Add the sub-expression to the vector
            highLevelNodes.push_back(subExpression);

            // Clear the sub-expression tokens
            subExpressionTokens.clear();
        }
        // Add to the sub-expression tokens
        else {
            subExpressionTokens.push_back(token);
        }

        // Increment the index
        i++;
    }

    // Edge case: if we have a sub-expression at the end, we need to parse it
    // Should have at least one high-level operator
    if (subExpressionTokens.size() > 0) {
        // If the subExpressionTokens is just all of the original tokens, we have an error
        // As in, whatever junk that was passed in was not parsed properly by the above logic
        if (subExpressionTokens.size() == expressionTokens.size()) {
            syntaxError("Unexpected token " + subExpressionTokens[0]->lexeme);
            NUKE_HIGH_LEVEL_NODES
            return ERROR_NODE;
        }

        // Parse the sub-expression
        ASTNode* subExpression = parseExpression(subExpressionTokens, false);

        if (subExpression == ERROR_NODE) {
            NUKE_HIGH_LEVEL_NODES
            return ERROR_NODE;
        }

        // Add the sub-expression to the vector
        highLevelNodes.push_back(subExpression);
    }

    // Now, construct the AST using the high-level operators and nodes
    // Using getPrecedence, we can find the highest-level operator

    // If we canBeEmpty, we can have an empty expression
    if (highLevelNodes.size() == 0 && highLevelOperators.size() == 0 && canBeEmpty) {
        return new EmptyExpressionNode();
    }

    // First check that we have the right number of operators for the number of nodes
    if (highLevelOperators.size() != highLevelNodes.size() - 1) {
        syntaxError("Unexpected number of operators");
        NUKE_HIGH_LEVEL_NODES
        return ERROR_NODE;
    }

    // Loop until there are no more high-level operators
    while (highLevelOperators.size() > 0) {
        // Find the highest-level operator
        int highestPrecedence = 0;
        size_t highestPrecedenceIndex = 0;
        for (size_t i = 0; i < highLevelOperators.size(); i++) {
            size_t precedence = getPrecedence(highLevelOperators[i]);
            if (precedence > highestPrecedence) {
                highestPrecedence = precedence;
                highestPrecedenceIndex = i;
            }
        }

        // Get the highest-level operator
        std::string highestLevelOperator = highLevelOperators[highestPrecedenceIndex];

        // Get the left and right nodes
        ASTNode* leftNode = highLevelNodes[highestPrecedenceIndex];
        ASTNode* rightNode = highLevelNodes[highestPrecedenceIndex + 1];

        // Create a binary operation node based on the operator
        BinaryOperationNode* binaryNode = new BinaryOperationNode(leftNode, highestLevelOperator, rightNode);

        // Replace the left and right nodes with the binary node
        highLevelNodes[highestPrecedenceIndex] = binaryNode;
        highLevelNodes.erase(highLevelNodes.begin() + highestPrecedenceIndex + 1);

        // Remove the operator
        highLevelOperators.erase(highLevelOperators.begin() + highestPrecedenceIndex);
    }

    // Return the last node
    return highLevelNodes[0];
}

NumberNode* Parser::parseConstant() {
    // Check if the current token is an integer or float
    if (tokens[currentTokenIndex].type == TokenType::INTEGER || tokens[currentTokenIndex].type == TokenType::FLOAT) {
        // Parse the constant
        NumberNode* constant = new NumberNode(tokens[currentTokenIndex].lexeme, tokens[currentTokenIndex].type);
        eatToken(tokens[currentTokenIndex].type);

        if (errorHandler.shouldStopExecution()) {
            delete constant;
            return ERROR_NODE;
        }

        return constant;
    } else {
        syntaxError("Unexpected token " + tokens[currentTokenIndex].lexeme);
        return ERROR_NODE;
    }
}

VariableAccessNode* Parser::parseVariableAccess() {
    // Check if the current token is an identifier
    if (tokens[currentTokenIndex].type == TokenType::IDENTIFIER) {
        // Parse the identifier
        std::string identifier = tokens[currentTokenIndex].lexeme;
        eatToken(TokenType::IDENTIFIER);

        if (errorHandler.shouldStopExecution()) {
            return ERROR_NODE;
        }

        return new VariableAccessNode(identifier);
    } else {
        syntaxError("Unexpected token " + tokens[currentTokenIndex].lexeme);
        return ERROR_NODE;
    }
}

AssignmentNode* Parser::parseAssignment(TokenType terminator) {
    // Check if the current token is an identifier
    if (tokens[currentTokenIndex].type == TokenType::IDENTIFIER) {
        // Parse the identifier
        std::string identifier = tokens[currentTokenIndex].lexeme;
        eatToken(TokenType::IDENTIFIER);

        if (errorHandler.shouldStopExecution()) {
            return ERROR_NODE;
        }

        // Check if there is an assignment operator
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::OPERATOR && tokens[currentTokenIndex].lexeme == "=") {
            // Eat the assignment operator
            eatToken(TokenType::OPERATOR);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            std::vector<const Token*> expressionTokens = gatherTokensUntil(terminator);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // Parse the initializer, minus the semicolon
            expressionTokens.pop_back();

            // Parse the expression
            ASTNode* expression = parseExpression(expressionTokens, false);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            return new AssignmentNode(identifier, expression);
        } else {
            syntaxError("AssignmentNode1: Unexpected token " + tokens[currentTokenIndex].lexeme);
            return ERROR_NODE;
        }
    } else {
        syntaxError("AssignmentNode2: Unexpected token " + tokens[currentTokenIndex].lexeme);
        return ERROR_NODE;
    }
}

IfNode* Parser::parseIfStatement() {
    // Construct an if node
    // Can be a simple if or an if-else, or an if-else-if-else, etc.

    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD) {
        // Check which keyword it is
        if (tokens[currentTokenIndex].lexeme == "if") {
            // Eat the if keyword
            eatToken(TokenType::KEYWORD);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            std::vector<ASTNode*> expressions;
            std::vector<BlockNode*> blocks;

            // Handle the first if

            // Parse the expression by gathering tokens until the right parenthesis
            eatToken(TokenType::LEFT_PARENTHESIS);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // Parse the expression
            ASTNode* expression = parseExpression(expressionTokens, false);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            expressions.push_back(expression);

            // Parse the block
            BlockNode* block = parseBlock();

            if (errorHandler.shouldStopExecution()) {
                delete expression;
                return ERROR_NODE;
            }

            blocks.push_back(block);

            // Handle the else-if's and else's

            // Check if there is an else or an else followed by an if
            while (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::KEYWORD && (tokens[currentTokenIndex].lexeme == "else" || (tokens[currentTokenIndex].lexeme == "else" && currentTokenIndex + 1 < tokens.size() && tokens[currentTokenIndex + 1].lexeme == "if"))) {
                // Check if it is an else-if
                if (tokens[currentTokenIndex].lexeme == "else" && currentTokenIndex + 1 < tokens.size() && tokens[currentTokenIndex + 1].lexeme == "if") {
                    // Eat the else
                    eatToken(TokenType::KEYWORD);

                    if (errorHandler.shouldStopExecution()) {
                        for (auto expression : expressions) {
                            delete expression;
                        }
                        for (auto block : blocks) {
                            delete block;
                        }
                        return ERROR_NODE;
                    }

                    // Eat the if
                    eatToken(TokenType::KEYWORD);

                    if (errorHandler.shouldStopExecution()) {
                        for (auto expression : expressions) {
                            delete expression;
                        }
                        for (auto block : blocks) {
                            delete block;
                        }
                        return ERROR_NODE;
                    }

                    // Parse the expression by gathering tokens until the right parenthesis
                    eatToken(TokenType::LEFT_PARENTHESIS);

                    if (errorHandler.shouldStopExecution()) {
                        for (auto expression : expressions) {
                            delete expression;
                        }
                        for (auto block : blocks) {
                            delete block;
                        }
                        return ERROR_NODE;
                    }

                    std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS);

                    if (errorHandler.shouldStopExecution()) {
                        for (auto expression : expressions) {
                            delete expression;
                        }
                        for (auto block : blocks) {
                            delete block;
                        }
                        return ERROR_NODE;
                    }

                    // Parse the expression
                    ASTNode* expression = parseExpression(expressionTokens, false);

                    if (errorHandler.shouldStopExecution()) {
                        for (auto expression : expressions) {
                            delete expression;
                        }
                        for (auto block : blocks) {
                            delete block;
                        }
                        return ERROR_NODE;
                    }

                    expressions.push_back(expression);

                    // Parse the block
                    BlockNode* block = parseBlock();

                    if (errorHandler.shouldStopExecution()) {
                        for (auto expression : expressions) {
                            delete expression;
                        }
                        for (auto block : blocks) {
                            delete block;
                        }
                        delete expression;
                        return ERROR_NODE;
                    }

                    blocks.push_back(block);
                } else {
                    // Eat the else
                    eatToken(TokenType::KEYWORD);

                    if (errorHandler.shouldStopExecution()) {
                        for (auto expression : expressions) {
                            delete expression;
                        }
                        for (auto block : blocks) {
                            delete block;
                        }
                        return ERROR_NODE;
                    }

                    // Parse the block
                    BlockNode* block = parseBlock();

                    if (errorHandler.shouldStopExecution()) {
                        for (auto expression : expressions) {
                            delete expression;
                        }
                        for (auto block : blocks) {
                            delete block;
                        }
                        return ERROR_NODE;
                    }

                    blocks.push_back(block);

                    // Break out of the while loop
                    break;
                }
            }

            return new IfNode(expressions, blocks);

        } else {
            syntaxError("IfNode: Unexpected keyword " + tokens[currentTokenIndex].lexeme);
            return ERROR_NODE;
        }

    } else {
        syntaxError("IfNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
        return ERROR_NODE;
    }        
    
}

BlockNode* Parser::parseBlock() {
    // Parse a block of code (ex the main program or a loop body)

    // Eat the leading brace
    eatToken(TokenType::LEFT_BRACE);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_NODE;
    }

    // The resulting vector of tokens should be parsed into a vector of ASTNodes
    std::vector<ASTNode*> statements;

    // This is where we need to differentiate between assignment, declaration, if,
    // while, ...
    while (currentTokenIndex < tokens.size()) {
        // Get the next token
        const Token* token = &tokens[currentTokenIndex];

        // If we find a right brace, we are done
        if (token->type == TokenType::RIGHT_BRACE) {
            break;
        }

        // Check if the token is a keyword
        if (token->type == TokenType::KEYWORD) {
            // Check which keyword it is
            if (token->lexeme == "int" || token->lexeme == "float" || token->lexeme == "void") {
                // Either a variable or function declaration
                // Functions are indicated by a function header, i.e. a type, a function name, and a (
                if (currentTokenIndex + 1 < tokens.size() && tokens[currentTokenIndex + 1].type == TokenType::IDENTIFIER && currentTokenIndex + 2 < tokens.size() && tokens[currentTokenIndex + 2].type == TokenType::LEFT_PARENTHESIS) {
                    // Parse the function declaration
                    statements.push_back(parseFunctionDeclaration());
                } else if (currentTokenIndex + 1 < tokens.size() && tokens[currentTokenIndex + 1].type == TokenType::IDENTIFIER) {
                    // Parse the variable declaration
                    statements.push_back(parseVariableDeclaration());
                } else {
                    syntaxError("BlockNode: Unexpected keyword " + token->lexeme);
                    // Nuke statements
                    for (auto statement : statements) {
                        delete statement;
                    }
                    return ERROR_NODE;
                }
            } else if (token->lexeme == "if") {
                // Parse the if statement
                statements.push_back(parseIfStatement());
            } else if (token->lexeme == "while") {
                // Parse the while loop
                statements.push_back(parseWhile());
            } else if (token->lexeme == "break") {
                // Parse the break statement
                statements.push_back(parseBreak());
            } else if (token->lexeme == "continue") {
                // Parse the continue statement
                statements.push_back(parseContinue());
            } else if (token->lexeme == "return") {
                // Parse the return statement
                statements.push_back(parseReturn());
            } else if (token->lexeme == "for") {
                // Parse the for loop
                statements.push_back(parseFor());
            } else {
                syntaxError("BlockNode1: Unexpected keyword " + token->lexeme);
                // Nuke statements
                for (auto statement : statements) {
                    delete statement;
                }
                return ERROR_NODE;
            }
        } else if (token->type == TokenType::IDENTIFIER) {
            if (currentTokenIndex + 1 < tokens.size() && tokens[currentTokenIndex + 1].type == TokenType::LEFT_PARENTHESIS) {
                // Function calls with no assignment in current scope
                statements.push_back(parseFunctionCall());

            } else {
                // Parse the assignment
                statements.push_back(parseAssignment(TokenType::SEMICOLON));
            }
        } else {
            syntaxError("BlockNode3: Unexpected token " + tokens[currentTokenIndex].lexeme);
            // Nuke statements
            for (auto statement : statements) {
                delete statement;
            }
            return ERROR_NODE;
        }

        // Check from the error handler after any statement
        if (errorHandler.shouldStopExecution()) {
            // Nuke statements
            for (auto statement : statements) {
                delete statement;
            }
            return ERROR_NODE;
        }
    }

    // Eat the trailing brace
    eatToken(TokenType::RIGHT_BRACE);

    if (errorHandler.shouldStopExecution()) {
        // Nuke statements
        for (auto statement : statements) {
            delete statement;
        }
        return ERROR_NODE;
    }

    return new BlockNode(statements);
}

VariableDeclarationNode* Parser::parseVariableDeclaration() {
    // Parse a variable declaration
    // Ex: int x = 5;
    // Ex: float y = x * 5.4;

    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD) {
        // Check which keyword it is
        if (tokens[currentTokenIndex].lexeme == "int" || tokens[currentTokenIndex].lexeme == "float") {
            // Parse the type
            std::string type = tokens[currentTokenIndex].lexeme;
            eatToken(TokenType::KEYWORD);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // Parse the identifier
            std::string identifier = tokens[currentTokenIndex].lexeme;
            eatToken(TokenType::IDENTIFIER);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // Check if there is an initializer
            ASTNode* initializer = nullptr;
            if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::OPERATOR && tokens[currentTokenIndex].lexeme == "=") {
                // Eat the assignment operator
                eatToken(TokenType::OPERATOR);

                if (errorHandler.shouldStopExecution()) {
                    return ERROR_NODE;
                }

                std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::SEMICOLON);

                if (errorHandler.shouldStopExecution()) {
                    return ERROR_NODE;
                }

                // Parse the initializer, minus the semicolon
                expressionTokens.pop_back();
                initializer = parseExpression(expressionTokens, false);

                if (errorHandler.shouldStopExecution()) {
                    return ERROR_NODE;
                }
            } else {
                syntaxError("VariableDeclarationNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
                return ERROR_NODE;
            }

            // Semicolon is already eaten by gatherTokensUntil and we popped it off
            // the expressionTokens vector
            return new VariableDeclarationNode(identifier, type, initializer);

        } else {
            syntaxError("VariableDeclarationNode: Unexpected keyword " + tokens[currentTokenIndex].lexeme);
            return ERROR_NODE;
        }
    } else {
        syntaxError("VariableDeclarationNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
        return ERROR_NODE;
    }
}

WhileNode* Parser::parseWhile() {
    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "while") {
        // Eat the while keyword
        eatToken(TokenType::KEYWORD);

        if (errorHandler.shouldStopExecution()) {
            return ERROR_NODE;
        }

        // Check if there is an opening parenthesis
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::LEFT_PARENTHESIS) {
            // Eat the opening parenthesis
            eatToken(TokenType::LEFT_PARENTHESIS);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // Pop off the right parenthesis
            expressionTokens.pop_back();

            // Parse the expression
            ASTNode* expression = parseExpression(expressionTokens, false);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // Parse the block
            BlockNode* block = parseBlock();

            if (errorHandler.shouldStopExecution()) {
                delete expression;
                return ERROR_NODE;
            }

            return new WhileNode(expression, block);
        } else {
            syntaxError("WhileNode1: Unexpected token " + tokens[currentTokenIndex].lexeme);
            return ERROR_NODE;
        }
    } else {
        syntaxError("WhileNode2: Unexpected token " + tokens[currentTokenIndex].lexeme);
        return ERROR_NODE;
    }
}

BreakNode* Parser::parseBreak() {
    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "break") {
        // Eat the break keyword
        eatToken(TokenType::KEYWORD);

        if (errorHandler.shouldStopExecution()) {
            return ERROR_NODE;
        }

        // Eat the semicolon
        eatToken(TokenType::SEMICOLON);

        if (errorHandler.shouldStopExecution()) {
            return ERROR_NODE;
        }

        return new BreakNode();
    } else {
        syntaxError("BreakNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
        return ERROR_NODE;
    }
}

ContinueNode* Parser::parseContinue() {
    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "continue") {
        // Eat the continue keyword
        eatToken(TokenType::KEYWORD);

        if (errorHandler.shouldStopExecution()) {
            return ERROR_NODE;
        }

        // Eat the semicolon
        eatToken(TokenType::SEMICOLON);

        if (errorHandler.shouldStopExecution()) {
            return ERROR_NODE;
        }

        return new ContinueNode();
    } else {
        syntaxError("ContinueNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
        return ERROR_NODE;
    }
}

FunctionCallNode* Parser::parseFunctionCall() {
    // Check if the current token is an identifier
    if (tokens[currentTokenIndex].type == TokenType::IDENTIFIER) {
        // We can just leverage parseExpression to parse the function call

        // Note the identifier token
        const Token* identifierToken = &tokens[currentTokenIndex];

        // Eat the identifier
        eatToken(TokenType::IDENTIFIER);

        if (errorHandler.shouldStopExecution()) {
            return ERROR_NODE;
        }

        // Note the left parenthesis token
        const Token* leftParenthesisToken = &tokens[currentTokenIndex];

        // Eat the left parenthesis
        eatToken(TokenType::LEFT_PARENTHESIS);

        if (errorHandler.shouldStopExecution()) {
            return ERROR_NODE;
        }

        // Gather the tokens until the right parenthesis
        std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS);

        if (errorHandler.shouldStopExecution()) {
            return ERROR_NODE;
        }

        // Prepend the identifier token and a left parenthesis token to prepare for parseExpression
        expressionTokens.insert(expressionTokens.begin(), identifierToken);
        expressionTokens.insert(expressionTokens.begin() + 1, leftParenthesisToken);

        // Will also need to append the right parenthesis token to complete the addition of the function call
        Token* rightParenthesisToken = new Token();
        rightParenthesisToken->type = TokenType::RIGHT_PARENTHESIS;
        rightParenthesisToken->lexeme = ")";
        expressionTokens.push_back(rightParenthesisToken);

        // Note how we had to do this workaround in order to have the safely keep track of the token index
        // This makes sure that the token index is at the right place and that these tokens actually exist

        ASTNode* expression = parseExpression(expressionTokens, false);

        if (errorHandler.shouldStopExecution()) {
            delete rightParenthesisToken;
            return ERROR_NODE;
        }

        // Eat the semicolon
        eatToken(TokenType::SEMICOLON);

        if (errorHandler.shouldStopExecution()) {
            delete rightParenthesisToken;
            delete expression;
            return ERROR_NODE;
        }

        // The expression should be a function call node
        if (expression->getNodeType() == ASTNodeType::FUNCTION_CALL_NODE) {
            delete rightParenthesisToken;
            return (FunctionCallNode*)expression;
        } else {
            syntaxError("FunctionCallNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
            delete rightParenthesisToken;
            delete expression;
            return ERROR_NODE;
        }
    } else {
        syntaxError("FunctionCallNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
        return ERROR_NODE;
    }
}

FunctionDeclarationNode* Parser::parseFunctionDeclaration() {
    // A function declaration is a keyword followed by an identifier followed by a left parenthesis
    std::string type = tokens[currentTokenIndex].lexeme;
    eatToken(TokenType::KEYWORD);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_NODE;
    }

    std::string identifier = tokens[currentTokenIndex].lexeme;
    eatToken(TokenType::IDENTIFIER);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_NODE;
    }

    eatToken(TokenType::LEFT_PARENTHESIS);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_NODE;
    }

    // Gather the tokens until the right parenthesis
    std::vector<std::string> parameterTypes;
    std::vector<std::string> parameterIdentifiers;

    while (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type != TokenType::RIGHT_PARENTHESIS) {
        // Check if the current token is a keyword
        if (tokens[currentTokenIndex].type == TokenType::KEYWORD) {
            // Check which keyword it is
            if (tokens[currentTokenIndex].lexeme == "int" || tokens[currentTokenIndex].lexeme == "float") {
                // Parse the type
                std::string type = tokens[currentTokenIndex].lexeme;
                eatToken(TokenType::KEYWORD);

                if (errorHandler.shouldStopExecution()) {
                    return ERROR_NODE;
                }

                // Add the type to the vector
                parameterTypes.push_back(type);
            }
        } else {
            syntaxError("FunctionDeclarationNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
            return ERROR_NODE;
        }

        // Check if the current token is an identifier
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::IDENTIFIER) {
            // Parse the identifier
            std::string identifier = tokens[currentTokenIndex].lexeme;
            eatToken(TokenType::IDENTIFIER);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // Add the identifier to the vector
            parameterIdentifiers.push_back(identifier);
        } else {
            syntaxError("FunctionDeclarationNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
            return ERROR_NODE;
        }

        // If there is a comma, eat it, if not, it should be a right parenthesis for which we will break out of the loop
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::COMMA) {
            eatToken(TokenType::COMMA);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }
        } else if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::RIGHT_PARENTHESIS) {
            break;
        } else {
            syntaxError("FunctionDeclarationNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
            return ERROR_NODE;
        }
    }

    // Eat the right parenthesis
    eatToken(TokenType::RIGHT_PARENTHESIS);

    if (errorHandler.shouldStopExecution()) {
        return ERROR_NODE;
    }

    // Parse the block
    BlockNode* block = parseBlock();

    if (errorHandler.shouldStopExecution()) {
        return ERROR_NODE;
    }

    return new FunctionDeclarationNode(type, identifier, parameterIdentifiers, parameterTypes, block);
}

ReturnNode* Parser::parseReturn() {
    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "return") {
        // Eat the return keyword
        eatToken(TokenType::KEYWORD);

        if (errorHandler.shouldStopExecution()) {
            return ERROR_NODE;
        }

        // Check if there is an expression
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type != TokenType::SEMICOLON) {
            std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::SEMICOLON);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // Parse the expression, minus the semicolon
            expressionTokens.pop_back();
            ASTNode* expression = parseExpression(expressionTokens, false);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            return new ReturnNode(expression);
        } else {
            // Eat the semicolon
            eatToken(TokenType::SEMICOLON);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            return new ReturnNode();
        }
    } else {
        syntaxError("ReturnNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
        return ERROR_NODE;
    }
}

ForNode* Parser::parseFor() {
    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "for") {
        // Eat the for keyword
        eatToken(TokenType::KEYWORD);

        if (errorHandler.shouldStopExecution()) {
            return ERROR_NODE;
        }

        // Check if there is an opening parenthesis
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::LEFT_PARENTHESIS) {
            // Eat the opening parenthesis
            eatToken(TokenType::LEFT_PARENTHESIS);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // Parse the initializer
            ASTNode* initializer = parseVariableDeclaration();

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // Parse the condition
            std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::SEMICOLON);

            if (errorHandler.shouldStopExecution()) {
                delete initializer;
                return ERROR_NODE;
            }

            // Parse the expression, minus the semicolon
            expressionTokens.pop_back();
            ASTNode* condition = parseExpression(expressionTokens, false);

            if (errorHandler.shouldStopExecution()) {
                delete initializer;
                return ERROR_NODE;
            }

            // Parse the increment
            ASTNode* increment = parseAssignment(TokenType::RIGHT_PARENTHESIS);

            if (errorHandler.shouldStopExecution()) {
                delete initializer;
                delete condition;
                return ERROR_NODE;
            }

            // Parse the block
            BlockNode* block = parseBlock();

            if (errorHandler.shouldStopExecution()) {
                delete initializer;
                delete condition;
                delete increment;
                return ERROR_NODE;
            }

            return new ForNode(initializer, condition, increment, block);
        } else {
            syntaxError("ForNode1: Unexpected token " + tokens[currentTokenIndex].lexeme);
            return ERROR_NODE;
        }
    } else {
        syntaxError("ForNode2: Unexpected token " + tokens[currentTokenIndex].lexeme);
        return ERROR_NODE;
    }

}

//================================================================================================
// ASTNode Implementations
//================================================================================================

BlockNode::BlockNode(const std::vector<ASTNode*>& statements)
    : statements(statements) {
}

BlockNode::~BlockNode() {
    while (!statements.empty()) {
        ASTNode* statement = statements.back();
        statements.pop_back();
        delete statement;
    }
}

std::string BlockNode::toString() const {
    std::string result = "BLOCK NODE {\n";
    for (ASTNode* statement : statements) {
        result += statement->toString() + "\n";
    }
    result += "}";
    return result;
}

ASTNodeType BlockNode::getNodeType() const { return ASTNodeType::BLOCK_NODE; }

std::vector<ASTNode*> BlockNode::getStatements() const { return statements; }

VariableDeclarationNode::VariableDeclarationNode(
    const std::string& identifier, const std::string& type, ASTNode* initializer)
    : identifier(identifier), type(type), initializer(initializer) {
}

VariableDeclarationNode::~VariableDeclarationNode() { delete initializer; }

std::string VariableDeclarationNode::toString() const {
    std::string result = "VARIABLE DECLARATION " + type + " " + identifier;
    if (initializer != nullptr) {
        result += " = " + initializer->toString();
    }
    return result;
}

std::string VariableDeclarationNode::getIdentifier() const { return identifier; }

std::string VariableDeclarationNode::getType() const { return type; }

ASTNode* VariableDeclarationNode::getInitializer() const { return initializer; }

ASTNodeType VariableDeclarationNode::getNodeType() const { return ASTNodeType::VARIABLE_DECLARATION_NODE; }

AssignmentNode::AssignmentNode(const std::string& identifier, ASTNode* expression)
    : identifier(identifier), expression(expression) {
}

AssignmentNode::~AssignmentNode() { delete expression; }

std::string AssignmentNode::getIdentifier() const { return identifier; }

std::string AssignmentNode::toString() const { return "ASSIGNMENT " + identifier + " = " + expression->toString(); }

ASTNode* AssignmentNode::getExpression() const { return expression; }

ASTNodeType AssignmentNode::getNodeType() const { return ASTNodeType::ASSIGNMENT_NODE; }

NumberNode::NumberNode(std::string val, TokenType type)
    : value(val), type(type) {
}

NumberNode::~NumberNode() {}

std::string NumberNode::toString() const { return "NUMBER " + value; }

TokenType NumberNode::getType() const { return type; }

std::string NumberNode::getValue() const { return value; }

ASTNodeType NumberNode::getNodeType() const { return ASTNodeType::NUMBER_NODE; }

VariableAccessNode::VariableAccessNode(const std::string& identifier)
    : identifier(identifier) {
}

std::string VariableAccessNode::toString() const { return "VARIABLE ACCESS " + identifier; }

std::string VariableAccessNode::getIdentifier() const { return identifier; }

ASTNodeType VariableAccessNode::getNodeType() const { return ASTNodeType::VARIABLE_ACCESS_NODE; }

VariableAccessNode::~VariableAccessNode() {}

IfNode::IfNode(std::vector<ASTNode*> expressions, std::vector<BlockNode*> bodies)
    : expressions(expressions), bodies(bodies) {
}

std::string IfNode::toString() const {
    std::string result = "IF NODE ( ";
    for (size_t i = 0; i < expressions.size(); i++) {
        result += expressions[i]->toString() + " ) " + bodies[i]->toString() + "";
    }
    if (expressions.size() < bodies.size()) {
        result += "ELSE ) " + bodies.back()->toString();
    }
    return result;
}

std::vector<ASTNode*> IfNode::getExpressions() const { return expressions; }

std::vector<BlockNode*> IfNode::getBodies() const { return bodies; }

ASTNodeType IfNode::getNodeType() const { return ASTNodeType::IF_NODE; }

IfNode::~IfNode() {
    while (!expressions.empty()) {
        ASTNode* expression = expressions.back();
        expressions.pop_back();
        delete expression;
    }
    while (!bodies.empty()) {
        BlockNode* body = bodies.back();
        bodies.pop_back();
        delete body;
    }
}

BinaryOperationNode::BinaryOperationNode(ASTNode* left, std::string op, ASTNode* right)
    : left(left), op(op), right(right) {
}

std::string BinaryOperationNode::toString() const {
    return "BINARY OPERATION (" + left->toString() + " " + op + " " + right->toString() + ")";
}

BinaryOperationNode::~BinaryOperationNode() {
    delete left;
    delete right;
}

ASTNode* BinaryOperationNode::getLeftExpression() const { return left; }

ASTNode* BinaryOperationNode::getRightExpression() const { return right; }

std::string BinaryOperationNode::getOperator() const { return op; }

ASTNodeType BinaryOperationNode::getNodeType() const { return ASTNodeType::BINARY_OPERATION_NODE; }

MonoOperationNode::MonoOperationNode(std::string op, ASTNode* expression)
    : op(op), expression(expression) {
}

std::string MonoOperationNode::toString() const { return "MONO OPERATION (" + op + " " + expression->toString() + ")"; }

MonoOperationNode::~MonoOperationNode() { delete expression; }

std::string MonoOperationNode::getOperator() const { return op; }

ASTNode* MonoOperationNode::getExpression() const { return expression; }

ASTNodeType MonoOperationNode::getNodeType() const { return ASTNodeType::MONO_OPERATION_NODE; }

WhileNode::WhileNode(ASTNode* expression, BlockNode* body)
    : expression(expression), body(body) {
}

std::string WhileNode::toString() const { return "WHILE LOOP ( " + expression->toString() + " ) " + body->toString(); }

ASTNode* WhileNode::getExpression() const { return expression; }

BlockNode* WhileNode::getBody() const { return body; }

ASTNodeType WhileNode::getNodeType() const { return ASTNodeType::WHILE_NODE; }

WhileNode::~WhileNode() {
    delete expression;
    delete body;
}

BreakNode::BreakNode() {}

std::string BreakNode::toString() const { return "BREAK STATEMENT"; }

ASTNodeType BreakNode::getNodeType() const { return ASTNodeType::BREAK_NODE; }

BreakNode::~BreakNode() {}

ContinueNode::ContinueNode() {}

std::string ContinueNode::toString() const { return "CONTINUE STATEMENT"; }

ASTNodeType ContinueNode::getNodeType() const { return ASTNodeType::CONTINUE_NODE; }

ContinueNode::~ContinueNode() {}

FunctionDeclarationNode::FunctionDeclarationNode(const std::string& type, const std::string& name, const std::vector<std::string>& parameters, const std::vector<std::string>& parameterTypes, BlockNode* body)
    : type(type), name(name), parameters(parameters), parameterTypes(parameterTypes), body(body) {
}

std::string FunctionDeclarationNode::toString() const {
    std::string result = "FUNCTION DECLARATION (" + type + ") " + name + " ( ";
    for (size_t i = 0; i < parameters.size(); i++) {
        result += parameterTypes[i] + " " + parameters[i] + ", ";
    }
    result += ") " + body->toString();
    return result;
}

std::string FunctionDeclarationNode::getType() const { return type; }

std::string FunctionDeclarationNode::getName() const { return name; }

std::vector<std::string> FunctionDeclarationNode::getParameters() const { return parameters; }

std::vector<std::string> FunctionDeclarationNode::getParameterTypes() const { return parameterTypes; }

BlockNode* FunctionDeclarationNode::getBody() const { return body; }

ASTNodeType FunctionDeclarationNode::getNodeType() const { return ASTNodeType::FUNCTION_DECLARATION_NODE; }

FunctionDeclarationNode::~FunctionDeclarationNode() {
    delete body;
}

FunctionCallNode::FunctionCallNode(const std::string& name, const std::vector<ASTNode*>& arguments)
    : name(name), arguments(arguments) {
}

std::string FunctionCallNode::toString() const {
    std::string result = "FUNCTION CALL " + name + " ( ";
    for (ASTNode* argument : arguments) {
        result += argument->toString() + ", ";
    }
    result += ")";
    return result;
}

std::string FunctionCallNode::getName() const { return name; }

std::vector<ASTNode*> FunctionCallNode::getArguments() const { return arguments; }

ASTNodeType FunctionCallNode::getNodeType() const { return ASTNodeType::FUNCTION_CALL_NODE; }

FunctionCallNode::~FunctionCallNode() {
    while (!arguments.empty()) {
        ASTNode* argument = arguments.back();
        arguments.pop_back();
        delete argument;
    }
}

ReturnNode::ReturnNode(ASTNode* expression)
    : expression(expression) {
}

ReturnNode::ReturnNode()
    : expression(nullptr) {
}

std::string ReturnNode::toString() const {
    if (expression != nullptr) {
        return "RETURN STATEMENT ( " + expression->toString() + " )";
    } else {
        return "RETURN STATEMENT";
    }
}

ASTNode* ReturnNode::getExpression() const { return expression; }

ASTNodeType ReturnNode::getNodeType() const { return ASTNodeType::RETURN_NODE; }

ReturnNode::~ReturnNode() {
    if (expression != nullptr) {
        delete expression;
    }
}

ForNode::ForNode(ASTNode* initializer, ASTNode* condition, ASTNode* increment, BlockNode* body)
    : initializer(initializer), condition(condition), increment(increment), body(body) {
}

std::string ForNode::toString() const {
    return "FOR LOOP ( " + initializer->toString() + " ; " + condition->toString() + " ; " + increment->toString() + " ) " + body->toString();
}

ASTNode* ForNode::getInitializer() const { return initializer; }

ASTNode* ForNode::getCondition() const { return condition; }

ASTNode* ForNode::getIncrement() const { return increment; }

BlockNode* ForNode::getBody() const { return body; }

ASTNodeType ForNode::getNodeType() const { return ASTNodeType::FOR_NODE; }

ForNode::~ForNode() {
    delete initializer;
    delete condition;
    delete increment;
    delete body;
}

EmptyExpressionNode::EmptyExpressionNode() {}

std::string EmptyExpressionNode::toString() const { return "EMPTY EXPRESSION"; }

ASTNodeType EmptyExpressionNode::getNodeType() const { return ASTNodeType::EMPTY_EXPRESSION_NODE; }

EmptyExpressionNode::~EmptyExpressionNode() {}