#include "ast.hpp"

#include "tokenizer.hpp"

// Upon error, return nullptr
// Therefore, all calls expecting an ASTNode* should check for nullptr
// This #define is used to clarify that this is the case

// In general, call the syntaxError function at the source and pass the error
// Therefore, we'll be checking the erroHandler a lot (or equivalently, calling seeing if we were returned ERROR_NODE)
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
    errorHandler.handleError("Syntax Error at token " + std::to_string(currentTokenIndex + 1) + ": " + tokens[currentTokenIndex].lexeme + ": " + message);
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
        syntaxError("Unexpected token type, expected " + Tokenizer::tokenTypeToString(expectedTokenType) + ", got " + Tokenizer::tokenTypeToString(tokens[currentTokenIndex].type));
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

    int startingIndex = currentTokenIndex;

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

ASTNode* Parser::parseExpression(const std::vector<const Token*>& expressionTokens) {
    // Parses expressions, potentially with parentheses
    // Ex. 5, x, 3.14, 5 + 8, 5 * 8 + x * (4 + 13)

    // Handle the case where there are no tokens
    if (expressionTokens.size() == 0) {
        syntaxError("Empty (sub)expression");
        return ERROR_NODE;
    }

    std::cout << "Parsing expression: ";
    for (auto token : expressionTokens) {
        std::cout << token->lexeme << " ";
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

        std::cout << "Beginning of while loop, current token is " << token->lexeme << " braces counter is " << parenthesesCounter << std::endl;

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
                else if (functionToken->type == TokenType::COMMA) {
                    // If we are not back to 1 parentheses (the opening left), throw an error
                    if (functionParenthesesCounter != 1) {
                        syntaxError("Unexpected comma " + functionToken->lexeme);
                        NUKE_HIGH_LEVEL_NODES
                        return ERROR_NODE;
                    }

                    // Parse the current function argument
                    ASTNode* currentFunctionArgument = parseExpression(currentFunctionArgumentTokens);

                    if (currentFunctionArgument == ERROR_NODE) {
                        NUKE_HIGH_LEVEL_NODES
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
                    ASTNode* currentFunctionArgument = parseExpression(currentFunctionArgumentTokens);

                    if (currentFunctionArgument == ERROR_NODE) {
                        NUKE_HIGH_LEVEL_NODES
                        return ERROR_NODE;
                    }

                    // Add the current function argument to the vector
                    functionArguments.push_back(currentFunctionArgument);

                    // Clear the current function argument tokens
                    currentFunctionArgumentTokens.clear();

                    // Eat the right parenthesis
                    // i++;

                    // print the function arguments
                    std::cout << "Function arguments: ";
                    for (auto arg : functionArguments) {
                        std::cout << arg->toString() << " ";
                    }

                    std::cout << "the current token is " << expressionTokens[i]->lexeme << std::endl;

                    // Create the function call node
                    FunctionCallNode* functionCallNode = new FunctionCallNode(functionName, functionArguments);

                    if (functionCallNode == ERROR_NODE) {
                        NUKE_HIGH_LEVEL_NODES
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
            ASTNode* subExpression = parseExpression(subExpressionTokens);

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
    if (subExpressionTokens.size() > 0) {
        // Parse the sub-expression
        ASTNode* subExpression = parseExpression(subExpressionTokens);

        if (subExpression == ERROR_NODE) {
            NUKE_HIGH_LEVEL_NODES
            return ERROR_NODE;
        }

        // Add the sub-expression to the vector
        highLevelNodes.push_back(subExpression);
    }

    std::cout << "High-level operators: ";
    for (auto op : highLevelOperators) {
        std::cout << op << " ";
    }

    std::cout << std::endl;

    std::cout << "High-level nodes: ";
    for (auto node : highLevelNodes) {
        std::cout << node->toString() << " ";
    }

    std::cout << std::endl;

    // Now, construct the AST using the high-level operators and nodes
    // Using getPrecedence, we can find the highest-level operator

    // First check that we have the right number of operators for the number of nodes
    if (highLevelOperators.size() != highLevelNodes.size() - 1) {
        syntaxError("Unexpected number of operators");
        NUKE_HIGH_LEVEL_NODES
        return ERROR_NODE;
    }

    // Loop until there are no more high-level operators
    while (highLevelOperators.size() > 0) {
        // Find the highest-level operator
        size_t highestPrecedence = 0;
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

AssignmentNode* Parser::parseAssignment() {
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

            std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::SEMICOLON);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // Parse the initializer, minus the semicolon
            expressionTokens.pop_back();

            // Parse the expression
            ASTNode* expression = parseExpression(expressionTokens);

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
    // Check if the current token is an identifier
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "if") {
        // Eat the if keyword
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
            ASTNode* expression = parseExpression(expressionTokens);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // Parse the block
            BlockNode* block = parseBlock();

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // If there is an else keyword, parse the else block
            BlockNode* elseBlock = nullptr;
            if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "else") {
                // Eat the else keyword
                eatToken(TokenType::KEYWORD);

                if (errorHandler.shouldStopExecution()) {
                    return ERROR_NODE;
                }

                // Parse the else block
                elseBlock = parseBlock();

                if (errorHandler.shouldStopExecution()) {
                    return ERROR_NODE;
                }

                return new IfNode(expression, block, elseBlock);
            } else {
                return new IfNode(expression, block);
            }

        } else {
            syntaxError("IfNode1: Unexpected token " + tokens[currentTokenIndex].lexeme);
            return ERROR_NODE;
        }
    } else {
        syntaxError("IfNode2: Unexpected token " + tokens[currentTokenIndex].lexeme);
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

        // TODO functions, return, and for loops

        // Check if the token is a keyword
        if (token->type == TokenType::KEYWORD) {
            // Check which keyword it is
            if (token->lexeme == "int" || token->lexeme == "float") {
                // Parse the variable declaration
                statements.push_back(parseVariableDeclaration());
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
                // statements.push_back(parseReturn()); //TODO
            } else if (token->lexeme == "for") {
                // Parse the for loop
                // statements.push_back(parseFor()); //TODO
            } else {
                syntaxError("BlockNode1: Unexpected keyword " + token->lexeme);
                return ERROR_NODE;
            }
        } else if (token->type == TokenType::IDENTIFIER) {
            if (currentTokenIndex + 1 < tokens.size() && tokens[currentTokenIndex + 1].type == TokenType::LEFT_PARENTHESIS) {
                // Function calls with no assignment in current scope
                statements.push_back(parseFunctionCall());

            } else {
                // Parse the assignment
                statements.push_back(parseAssignment());
            }
        } else {
            syntaxError("BlockNode3: Unexpected token " + tokens[currentTokenIndex].lexeme);
            return ERROR_NODE;
        }
    }

    // Eat the trailing brace
    eatToken(TokenType::RIGHT_BRACE);

    if (errorHandler.shouldStopExecution()) {
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
                initializer = parseExpression(expressionTokens);

                if (initializer == ERROR_NODE) {
                    return ERROR_NODE;
                }
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
            ASTNode* expression = parseExpression(expressionTokens);

            if (errorHandler.shouldStopExecution()) {
                return ERROR_NODE;
            }

            // Parse the block
            BlockNode* block = parseBlock();

            if (errorHandler.shouldStopExecution()) {
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

        ASTNode* expression = parseExpression(expressionTokens);

        if (errorHandler.shouldStopExecution()) {
            delete rightParenthesisToken;
            return ERROR_NODE;
        }

        // Eat the semicolon
        eatToken(TokenType::SEMICOLON);

        if (errorHandler.shouldStopExecution()) {
            delete rightParenthesisToken;
            return ERROR_NODE;
        }

        // The expression should be a function call node
        if (expression->getNodeType() == ASTNodeType::FUNCTION_CALL_NODE) {
            delete rightParenthesisToken;
            return (FunctionCallNode*)expression;
        } else {
            syntaxError("FunctionCallNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
            delete rightParenthesisToken;
            return ERROR_NODE;
        }
    } else {
        syntaxError("FunctionCallNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
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

IfNode::IfNode(ASTNode* expression, BlockNode* body)
    : expression(expression), body(body), elseBody(nullptr) {
}

IfNode::IfNode(ASTNode* expression, BlockNode* body, BlockNode* elseBody)
    : expression(expression), body(body), elseBody(elseBody) {
}

std::string IfNode::toString() const {
    if (elseBody != nullptr) {
        return "IF STATEMENT ( " + expression->toString() + " ) " + body->toString() + " ELSE: " + elseBody->toString();
    } else {
        return "IF STATEMENT ( " + expression->toString() + " ) " + body->toString();
    }
}

ASTNode* IfNode::getExpression() const { return expression; }

BlockNode* IfNode::getBody() const { return body; }

BlockNode* IfNode::getElseBody() const { return elseBody; }

ASTNodeType IfNode::getNodeType() const { return ASTNodeType::IF_NODE; }

IfNode::~IfNode() {
    delete expression;
    delete body;
    if (elseBody != nullptr) {
        delete elseBody;
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
