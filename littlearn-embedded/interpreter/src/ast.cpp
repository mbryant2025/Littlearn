#include "ast.hpp"
#include "tokenizer.hpp"
#include "error.hpp"

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), currentTokenIndex(0) {}

void Parser::syntaxError(const std::string &message)
{
    handleError("Syntax Error at token " + std::to_string(currentTokenIndex + 1) + ": " + tokens[currentTokenIndex].lexeme + ": " + message);
}

BlockNode *Parser::parseProgram()
{
    // The entry point for parsing a program into an AST.
    // Example: Parse a block of code (e.g., the main program)

    BlockNode *programBlock = parseBlock();

    // Check if there are any remaining tokens; if yes, report an error
    if (currentTokenIndex < tokens.size())
    {
        syntaxError("Unexpected tokens after the program.");
        return nullptr;
    }

    return programBlock;
}

void Parser::eatToken(TokenType expectedTokenType)
{
    // Eat the next token if it matches the expected token type
    if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == expectedTokenType)
    {
        currentTokenIndex++;
    }
    else
    {
        syntaxError("Unexpected token type, expected " + Tokenizer::tokenTypeToString(expectedTokenType) + ", got " + Tokenizer::tokenTypeToString(tokens[currentTokenIndex].type));
    }
}

std::vector<const Token *> Parser::gatherTokensUntil(TokenType endTokenType, bool advanceIndex)
{
    // Gather tokens until the end token type is reached
    // Precondition: for wrapping syntax, the currentTokenIndex will be immediately after the opening version
    // Ex. If we are looking for } we should be right after the { (which should have been eaten prior to this call)

    // If we want a closing parenthesis or brace, we need to keep a counter
    // This counter will go up with every left brace/parenthesis and down with every right brace/parenthesis
    // When the counter reaches 0, we know we have reached the end of the block
    // A syntax error will be thrown if the counter goes below 0
    // A syntax error will also be thrown if we reach the end of the file without the counter reaching 0
    // The last case for a syntax error is that of {(})

    int braceParenthesisCounter = 0;

    int startingIndex = currentTokenIndex;

    // This is just a placeholder, it will be overwritten. It also indicates if we are looking for a brace or parenthesis
    // It just keeps track of the opened ones: ( or {
    TokenType lastBraceParenthesis = TokenType::UNKNOWN;

    if (endTokenType == TokenType::RIGHT_BRACE || endTokenType == TokenType::RIGHT_PARENTHESIS)
    {
        // We want to match, so assume we already ate an opening brace/parenthesis
        braceParenthesisCounter++;
        if (endTokenType == TokenType::RIGHT_BRACE)
        {
            lastBraceParenthesis = TokenType::LEFT_BRACE;
        }
        else
        {
            lastBraceParenthesis = TokenType::LEFT_PARENTHESIS;
        }
    }

    std::vector<const Token *> gatheredTokens;
    // while we can keep going and (we still should go)
    // we should go if (we are not looking for a brace or parenthesis and the current token is not the end token type) OR (we are looking for a brace or parenthesis and the counter is not 0)
    while (currentTokenIndex < tokens.size() && ((lastBraceParenthesis == TokenType::UNKNOWN && tokens[currentTokenIndex].type != endTokenType) || (lastBraceParenthesis != TokenType::UNKNOWN && braceParenthesisCounter != 0)))
    {

        // If we are looking for a brace or parenthesis, check if the current token is one
        if (lastBraceParenthesis != TokenType::UNKNOWN)
        {

            TokenType currentTokenType = tokens[currentTokenIndex].type;
            // Increment or decrement the counter
            if (currentTokenType == TokenType::LEFT_BRACE || currentTokenType == TokenType::LEFT_PARENTHESIS)
            {
                braceParenthesisCounter++;
            }
            else if (currentTokenType == TokenType::RIGHT_BRACE || currentTokenType == TokenType::RIGHT_PARENTHESIS)
            {
                braceParenthesisCounter--;

                // If we decremented the counter to 0 and the current token is not the end token type, we have an error
                if (braceParenthesisCounter == 0 && currentTokenType != endTokenType)
                {
                    syntaxError("Unexpected brace or parenthesis " + tokens[currentTokenIndex].lexeme);
                }

                // If we decremented the counter to -1, we have an error
                if (braceParenthesisCounter < 0)
                {
                    syntaxError("Unexpected brace or parenthesis " + tokens[currentTokenIndex].lexeme);
                }

                // If we decremented the counter to 0 and the current token is the end token type, we are done
                if (braceParenthesisCounter == 0 && currentTokenType == endTokenType)
                {
                    break;
                }
            }

            // Check for the condition where we have something like {(})
            if ((currentTokenType == TokenType::RIGHT_BRACE && lastBraceParenthesis == TokenType::LEFT_PARENTHESIS) || (currentTokenType == TokenType::RIGHT_PARENTHESIS && lastBraceParenthesis == TokenType::LEFT_BRACE))
            {
                syntaxError("Unexpected brace or parenthesis " + tokens[currentTokenIndex].lexeme);
            }

            // If it is an opening brace or parenthesis, update the last brace/parenthesis
            if (currentTokenType == TokenType::LEFT_BRACE || currentTokenType == TokenType::LEFT_PARENTHESIS)
            {
                lastBraceParenthesis = currentTokenType;
            }
        }

        // Parse a statement
        gatheredTokens.push_back(&tokens[currentTokenIndex]);
        // Only advance the token index if we expect to find the end token type
        currentTokenIndex++;
    }

    // Check if we reached the end token type
    if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == endTokenType)
    {
        // Add the end token to the gathered tokens as to include it in the vector
        gatheredTokens.push_back(&tokens[currentTokenIndex]);
        // Advance to the next token
        currentTokenIndex++;
    }
    else
    {
        // Expected means we expect to find the token
        syntaxError("Unexpected end of file, expected " + Tokenizer::tokenTypeToString(endTokenType));
    }

    if (!advanceIndex)
        currentTokenIndex = startingIndex;

    return gatheredTokens;
}

int Parser::getPrecedence(std::string lexeme)
{
    // Returns the precedence of the given token type
    // Higher number is higher precedence

    if (lexeme == "*" || lexeme == "/" || lexeme == "%")
    {
        return 2;
    }
    else if (lexeme == "+" || lexeme == "-")
    {
        return 1;
    }
    else if (lexeme == ">" || lexeme == "<")
    {
        return 0;
    }
    else
    {
        syntaxError("Unexpected operator " + lexeme);
        return -1;
    }
}

bool Parser::isFunctionHeader(std::string lexeme)
{
    // Returns true if the given lexeme is a function header
    // Returns false otherwise

    // TODO implement user-defined functions

    if (lexeme == "read_port" || lexeme == "write_port" || lexeme == "print_seven_segment")
    {
        return true;
    }
    else
    {
        return false;
    }
}

ASTNode* Parser::createFunctionCallNode(std::string name, std::vector<ASTNode *> functionArguments) {
    // Create a function call node based on the name and arguments
    // TODO implement user-defined functions
    // Right now, this only supports read_port as this is the only one that returns a value

    std::cout << "createFunctionCallNode: " << name << std::endl;

    std::cout << "function arguments: " << std::endl;
    for (auto argument : functionArguments)
    {
        std::cout << argument->toString() << std::endl;
    }

    if (name == "read_port") {
        // Check that there is only one argument
        if (functionArguments.size() != 1) {
            syntaxError("read_port: Expected 1 argument, got " + std::to_string(functionArguments.size()));
        }

        // Return the read port node
        return new ReadPortNode(functionArguments[0]);
    }

    else {
        syntaxError("createFunctionCallNode: Unexpected function name " + name);
    }

    // Not reached
    return nullptr;
}

ASTNode *Parser::parseExpression(std::vector<const Token *> expressionTokens)
{

    // Parses expressions, potentially with parentheses
    // Ex. 5, x, 3.14, 5 + 8, 5 * 8 + x * (4 + 13)

    // Handle the base case when there is one token
    if (expressionTokens.size() == 1)
    {
        // Check if the token is a constant or variable access
        if (expressionTokens[0]->type == TokenType::INTEGER || expressionTokens[0]->type == TokenType::FLOAT)
        {
            // Parse the constant
            return new NumberNode(expressionTokens[0]->lexeme, expressionTokens[0]->type);
        }
        else if (expressionTokens[0]->type == TokenType::IDENTIFIER)
        {
            // Parse the variable access
            return new VariableAccessNode(expressionTokens[0]->lexeme);
        }
        else
        {
            syntaxError("Unexpected token " + expressionTokens[0]->lexeme);
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
    std::vector<ASTNode *> highLevelNodes;

    // Keep track of the parentheses
    int parenthesesCounter = 0;

    // Keep track of potential sub-expressions, i.e. stuff in parentheses
    std::vector<const Token *> subExpressionTokens;

    for (size_t i = 0; i < expressionTokens.size(); i++)
    {
        // Get the current token
        const Token *token = expressionTokens[i];

        // If we find a left parenthesis, increment the counter
        if (token->type == TokenType::LEFT_PARENTHESIS)
        {

            // If we already are a parenthesis in, add the parenthesis to the sub-expression tokens
            if (parenthesesCounter != 0)
            {
                subExpressionTokens.push_back(token);
            }

            parenthesesCounter++;
        }
        // If we find a right parenthesis, decrement the counter
        else if (token->type == TokenType::RIGHT_PARENTHESIS)
        {

            // If we already are a parenthesis in, add the parenthesis to the sub-expression tokens
            if (parenthesesCounter != 0)
            {
                subExpressionTokens.push_back(token);
            }

            parenthesesCounter--;
        }
        // If we find a function call as a high-level node, we need to parse it
        // else if (isFunctionHeader(token->lexeme))
        // {
        //     // Get the function name
        //     std::string functionName = token->lexeme;
        //     i++;

        //     std::cout << "function name: " << functionName << std::endl;
        //     std::cout << "i: " << i << std::endl;

        //     // Gather the function arguments -- manually gather tokens until we find a right parenthesis
        //     // These will be passed into a recursive call to parseExpression

        //     // Keep track of the parentheses
        //     int functionParenthesesCounter = 0;

        //     // Keep track of potential sub-expressions, i.e. stuff in parentheses
        //     std::vector<const Token *> functionSubExpressionTokens;

        //     // Keep track of the function arguments
        //     std::vector<ASTNode *> functionArguments;

        //     // Loop until we find a right parenthesis
        //     while (i < tokens.size() && (functionParenthesesCounter != 0 || tokens[currentTokenIndex].type != TokenType::RIGHT_PARENTHESIS))
        //     {
        //         std::cout << "i: " << i << std::endl;
        //         std::cout << "current token: " << tokens[i].lexeme << std::endl;

        //         // Get the current token
        //         const Token *functionToken = &tokens[i];

        //         // If we find a left parenthesis, increment the counter
        //         if (functionToken->type == TokenType::LEFT_PARENTHESIS)
        //         {

        //             // If we already are a parenthesis in, add the parenthesis to the sub-expression tokens
        //             if (functionParenthesesCounter != 0)
        //             {
        //                 functionSubExpressionTokens.push_back(functionToken);
        //             }

        //             functionParenthesesCounter++;
        //         }
        //         // If we find a right parenthesis, decrement the counter
        //         else if (functionToken->type == TokenType::RIGHT_PARENTHESIS)
        //         {

        //             // If we already are a parenthesis in, add the parenthesis to the sub-expression tokens
        //             if (functionParenthesesCounter != 0)
        //             {
        //                 functionSubExpressionTokens.push_back(functionToken);
        //             }

        //             functionParenthesesCounter--;
        //         }
        //         // If we find a comma, we have reached the end of an argument
        //         else if (functionToken->type == TokenType::COMMA)
        //         {
        //             // Parse the sub-expression
        //             ASTNode *subExpression = parseExpression(functionSubExpressionTokens);

        //             // Add the sub-expression to the vector
        //             functionArguments.push_back(subExpression);

        //             // Clear the sub-expression tokens
        //             functionSubExpressionTokens.clear();
        //         }
        //         // Add to the sub-expression tokens
        //         else
        //         {
        //             functionSubExpressionTokens.push_back(functionToken);
        //         }

        //         i++;
                

        //     }

        //     // Edge case: if we have a sub-expression at the end, we need to parse them
        //     if (functionSubExpressionTokens.size() > 0)
        //     {
        //         // Parse the sub-expression
        //         ASTNode *subExpression = parseExpression(functionSubExpressionTokens);

        //         // Add the sub-expression to the vector
        //         functionArguments.push_back(subExpression);
        //     }

        //     //print size of function arguments
        //     std::cout << "function arguments size: " << functionArguments.size() << std::endl;

        //     // Add the function call as a high-level node
        //     ASTNode* functionCall = createFunctionCallNode(functionName, functionArguments);

        // }




        // If we find an operator, check if the parentheses counter is 0
        // If it is, we have found a high-level operator
        else if (token->type == TokenType::OPERATOR && parenthesesCounter == 0)
        {
            // Add the operator to the vector
            highLevelOperators.push_back(token->lexeme);

            // Parse the sub-expression
            ASTNode *subExpression = parseExpression(subExpressionTokens);

            // Add the sub-expression to the vector
            highLevelNodes.push_back(subExpression);

            // Clear the sub-expression tokens
            subExpressionTokens.clear();

        }
        // Add to the sub-expression tokens
        else
        {
            subExpressionTokens.push_back(token);
        }

    }

    // Edge case: if we have a sub-expression at the end, we need to parse it
    if (subExpressionTokens.size() > 0)
    {

        // std::cout << "edge case" << std::endl;
        // std::cout << "subexpression tokens: " << std::endl;
        // for (auto token : subExpressionTokens)
        // {
        //     std::cout << token->lexeme << std::endl;
        // }
        // std::cout << "======\n";

        // Parse the sub-expression
        ASTNode *subExpression = parseExpression(subExpressionTokens);

        // Add the sub-expression to the vector
        highLevelNodes.push_back(subExpression);
    }

    // Now, construct the AST using the high-level operators and nodes
    // Using getPrecedence, we can find the highest-level operator

    // Loop until there are no more high-level operators
    while (highLevelOperators.size() > 0)
    {
        // Find the highest-level operator
        size_t highestPrecedence = 0;
        size_t highestPrecedenceIndex = 0;
        for (size_t i = 0; i < highLevelOperators.size(); i++)
        {
            size_t precedence = getPrecedence(highLevelOperators[i]);
            if (precedence > highestPrecedence)
            {
                highestPrecedence = precedence;
                highestPrecedenceIndex = i;
            }
        }

        // Get the highest-level operator
        std::string highestLevelOperator = highLevelOperators[highestPrecedenceIndex];

        // Get the left and right nodes
        ASTNode *leftNode = highLevelNodes[highestPrecedenceIndex];
        ASTNode *rightNode = highLevelNodes[highestPrecedenceIndex + 1];

        // Create a binary operation node based on the operator
        BinaryOperationNode *binaryNode = new BinaryOperationNode(leftNode, highestLevelOperator, rightNode);

        // Replace the left and right nodes with the binary node
        highLevelNodes[highestPrecedenceIndex] = binaryNode;
        highLevelNodes.erase(highLevelNodes.begin() + highestPrecedenceIndex + 1);

        // Remove the operator
        highLevelOperators.erase(highLevelOperators.begin() + highestPrecedenceIndex);
    }

    // Return the last node
    return highLevelNodes[0];
}

NumberNode *Parser::parseConstant()
{
    // Check if the current token is an integer or float
    if (tokens[currentTokenIndex].type == TokenType::INTEGER || tokens[currentTokenIndex].type == TokenType::FLOAT)
    {
        // Parse the constant
        NumberNode *constant = new NumberNode(tokens[currentTokenIndex].lexeme, tokens[currentTokenIndex].type);
        eatToken(tokens[currentTokenIndex].type);
        return constant;
    }
    else
    {
        syntaxError("Unexpected token " + tokens[currentTokenIndex].lexeme);
    }

    // Not reached
    return nullptr;
}

VariableAccessNode *Parser::parseVariableAccess()
{

    // Check if the current token is an identifier
    if (tokens[currentTokenIndex].type == TokenType::IDENTIFIER)
    {
        // Parse the identifier
        std::string identifier = tokens[currentTokenIndex].lexeme;
        eatToken(TokenType::IDENTIFIER);

        return new VariableAccessNode(identifier);
    }
    else
    {
        syntaxError("Unexpected token " + tokens[currentTokenIndex].lexeme);
    }

    // Not reached
    return nullptr;
}

AssignmentNode *Parser::parseAssignment()
{
    // Check if the current token is an identifier
    if (tokens[currentTokenIndex].type == TokenType::IDENTIFIER)
    {
        // Parse the identifier
        std::string identifier = tokens[currentTokenIndex].lexeme;
        eatToken(TokenType::IDENTIFIER);

        // Check if there is an assignment operator
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::OPERATOR && tokens[currentTokenIndex].lexeme == "=")
        {
            // Eat the assignment operator
            eatToken(TokenType::OPERATOR);

            std::vector<const Token *> expressionTokens = gatherTokensUntil(TokenType::SEMICOLON, true);

            // Parse the initializer, minus the semicolon
            expressionTokens.pop_back();

            // Parse the expression
            ASTNode *expression = parseExpression(expressionTokens);

            return new AssignmentNode(identifier, expression);
        }
        else
        {
            syntaxError("AssignmentNode1: Unexpected token " + tokens[currentTokenIndex].lexeme);
        }
    }
    else
    {
        syntaxError("AssignmentNode2: Unexpected token " + tokens[currentTokenIndex].lexeme);
    }

    // Not reached
    return nullptr;
}

IfNode *Parser::parseIfStatement()
{
    // Check if the current token is an identifier
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "if")
    {
        // Eat the if keyword
        eatToken(TokenType::KEYWORD);

        // Check if there is an opening parenthesis
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::LEFT_PARENTHESIS)
        {
            // Eat the opening parenthesis
            eatToken(TokenType::LEFT_PARENTHESIS);

            std::vector<const Token *> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            // Pop off the right parenthesis
            expressionTokens.pop_back();

            // Parse the expression
            ASTNode *expression = parseExpression(expressionTokens);

            // Parse the block
            BlockNode *block = parseBlock();

            return new IfNode(expression, block);
        }
        else
        {
            syntaxError("IfNode1: Unexpected token " + tokens[currentTokenIndex].lexeme);
        }
    }
    else
    {
        syntaxError("IfNode2: Unexpected token " + tokens[currentTokenIndex].lexeme);
    }

    // Not reached
    return nullptr;
}

BlockNode *Parser::parseBlock()
{
    // Parse a block of code (ex the main program or a loop body)

    // Eat the leading brace
    eatToken(TokenType::LEFT_BRACE);

    // The resulting vector of tokens should be parsed into a vector of ASTNodes
    std::vector<ASTNode *> statements;

    // This is where we need to differentiate between assignment, declaration, if, while, ...
    while (currentTokenIndex < tokens.size())
    {

        // Get the next token
        const Token *token = &tokens[currentTokenIndex];

        // If we find a right brace, we are done
        if (token->type == TokenType::RIGHT_BRACE)
        {
            break;
        }

        // Check if the token is a keyword
        if (token->type == TokenType::KEYWORD)
        {
            // Check which keyword it is
            if (token->lexeme == "int" || token->lexeme == "float")
            {
                // Parse the variable declaration
                statements.push_back(parseVariableDeclaration());
            }
            else if (token->lexeme == "if")
            {
                // Parse the if statement
                statements.push_back(parseIfStatement());
            }
            else if (token->lexeme == "print")
            {
                // Parse the print statement
                statements.push_back(parsePrint());
            }
            else if (token->lexeme == "while")
            {
                // Parse the while loop
                statements.push_back(parseWhile());
            }
            else if (token->lexeme == "wait")
            {
                // Parse the wait statement
                statements.push_back(parseWait());
            }
            else if (token->lexeme == "print_seven_segment")
            {
                // Parse the seven segment statement
                statements.push_back(parseSevenSegment());
            }
            else if (token->lexeme == "write_port")
            {
                // Parse the write port statement
                statements.push_back(parseWritePort());
            }
            else if (token->lexeme == "break")
            {
                // Parse the break statement
                statements.push_back(parseBreak());
            }
            else if (token->lexeme == "continue")
            {
                // Parse the continue statement
                statements.push_back(parseContinue());
            }
            else
            {
                syntaxError("BlockNode1: Unexpected keyword " + token->lexeme);
            }
        }
        else if (token->type == TokenType::IDENTIFIER)
        {
            statements.push_back(parseAssignment());
        }
        else
        {
            syntaxError("BlockNode3: Unexpected token " + tokens[currentTokenIndex].lexeme);
        }
    }

    // Eat the trailing brace
    eatToken(TokenType::RIGHT_BRACE);

    return new BlockNode(statements);
}

VariableDeclarationNode *Parser::parseVariableDeclaration()
{
    // Parse a variable declaration
    // Ex: int x = 5;
    // Ex: float y = x * 5.4;

    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD)
    {
        // Check which keyword it is
        if (tokens[currentTokenIndex].lexeme == "int" || tokens[currentTokenIndex].lexeme == "float")
        {
            // Parse the type
            std::string type = tokens[currentTokenIndex].lexeme;
            eatToken(TokenType::KEYWORD);

            // Parse the identifier
            std::string identifier = tokens[currentTokenIndex].lexeme;
            eatToken(TokenType::IDENTIFIER);

            // Check if there is an initializer
            ASTNode *initializer = nullptr;
            if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::OPERATOR && tokens[currentTokenIndex].lexeme == "=")
            {
                // Eat the assignment operator
                eatToken(TokenType::OPERATOR);

                std::vector<const Token *> expressionTokens = gatherTokensUntil(TokenType::SEMICOLON, true);

                // Parse the initializer, minus the semicolon
                expressionTokens.pop_back();
                initializer = parseExpression(expressionTokens);
            }

            // Semicolon is already eaten by gatherTokensUntil and we popped it off the expressionTokens vector

            return new VariableDeclarationNode(identifier, type, initializer);
        }
        else
        {
            syntaxError("VariableDeclarationNode: Unexpected keyword " + tokens[currentTokenIndex].lexeme);
        }
    }
    else
    {
        syntaxError("VariableDeclarationNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
    }

    // This should never be reached
    return nullptr;
}

PrintNode *Parser::parsePrint()
{
    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "print")
    {
        // Eat the print keyword
        eatToken(TokenType::KEYWORD);

        // Check if there is an opening parenthesis
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::LEFT_PARENTHESIS)
        {
            // Eat the opening parenthesis
            eatToken(TokenType::LEFT_PARENTHESIS);

            std::vector<const Token *> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            // Pop off the right parenthesis
            expressionTokens.pop_back();

            // Parse the expression
            ASTNode *expression = parseExpression(expressionTokens);

            // Eat the semicolon
            eatToken(TokenType::SEMICOLON);

            return new PrintNode(expression);
        }
        else
        {
            syntaxError("PrintNode1: Unexpected token " + tokens[currentTokenIndex].lexeme);
        }
    }
    else
    {
        syntaxError("PrintNode2: Unexpected token " + tokens[currentTokenIndex].lexeme);
    }

    // Not reached
    return nullptr;
}

WhileNode *Parser::parseWhile()
{
    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "while")
    {
        // Eat the while keyword
        eatToken(TokenType::KEYWORD);

        // Check if there is an opening parenthesis
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::LEFT_PARENTHESIS)
        {
            // Eat the opening parenthesis
            eatToken(TokenType::LEFT_PARENTHESIS);

            std::vector<const Token *> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            // Pop off the right parenthesis
            expressionTokens.pop_back();

            // Parse the expression
            ASTNode *expression = parseExpression(expressionTokens);

            // Parse the block
            BlockNode *block = parseBlock();

            return new WhileNode(expression, block);
        }
        else
        {
            syntaxError("WhileNode1: Unexpected token " + tokens[currentTokenIndex].lexeme);
        }
    }
    else
    {
        syntaxError("WhileNode2: Unexpected token " + tokens[currentTokenIndex].lexeme);
    }

    // Not reached
    return nullptr;
}

WaitNode *Parser::parseWait()
{
    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "wait")
    {
        // Eat the wait keyword
        eatToken(TokenType::KEYWORD);

        // Check if there is an opening parenthesis
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::LEFT_PARENTHESIS)
        {
            // Eat the opening parenthesis
            eatToken(TokenType::LEFT_PARENTHESIS);

            std::vector<const Token *> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            // Pop off the right parenthesis
            expressionTokens.pop_back();

            // Parse the expression
            ASTNode *expression = parseExpression(expressionTokens);

            // Eat the semicolon
            eatToken(TokenType::SEMICOLON);

            return new WaitNode(expression);
        }
        else
        {
            syntaxError("WaitNode1: Unexpected token " + tokens[currentTokenIndex].lexeme);
        }
    }
    else
    {
        syntaxError("WaitNode2: Unexpected token " + tokens[currentTokenIndex].lexeme);
    }

    // Not reached
    return nullptr;
}

SevenSegmentNode *Parser::parseSevenSegment()
{
    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "print_seven_segment")
    {
        // Eat the sevensegment keyword
        eatToken(TokenType::KEYWORD);

        // Check if there is an opening parenthesis
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::LEFT_PARENTHESIS)
        {
            // Eat the opening parenthesis
            eatToken(TokenType::LEFT_PARENTHESIS);

            std::vector<const Token *> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            // Pop off the right parenthesis
            expressionTokens.pop_back();

            // Parse the expression
            ASTNode *expression = parseExpression(expressionTokens);

            // Eat the semicolon
            eatToken(TokenType::SEMICOLON);

            return new SevenSegmentNode(expression);
        }
        else
        {
            syntaxError("SevenSegmentNode1: Unexpected token " + tokens[currentTokenIndex].lexeme);
        }
    }
    else
    {
        syntaxError("SevenSegmentNode2: Unexpected token " + tokens[currentTokenIndex].lexeme);
    }

    // Not reached
    return nullptr;
}

ReadPortNode *Parser::parseReadPort()
{
    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "read_port")
    {
        // Eat the read_port keyword
        eatToken(TokenType::KEYWORD);

        // Check if there is an opening parenthesis
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::LEFT_PARENTHESIS)
        {
            // Eat the opening parenthesis
            eatToken(TokenType::LEFT_PARENTHESIS);

            std::vector<const Token *> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            // Pop off the right parenthesis
            expressionTokens.pop_back();

            // Parse the expression
            ASTNode *expression = parseExpression(expressionTokens);

            // Eat the semicolon
            eatToken(TokenType::SEMICOLON);

            return new ReadPortNode(expression);
        }
        else
        {
            syntaxError("ReadPortNode1: Unexpected token " + tokens[currentTokenIndex].lexeme);
        }
    }
    else
    {
        syntaxError("ReadPortNode2: Unexpected token " + tokens[currentTokenIndex].lexeme);
    }

    // Not reached
    return nullptr;
}

WritePortNode *Parser::parseWritePort()
{
    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "write_port")
    {
        // Eat the write_port keyword
        eatToken(TokenType::KEYWORD);

        // Check if there is an opening parenthesis
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::LEFT_PARENTHESIS)
        {
            // Eat the opening parenthesis
            eatToken(TokenType::LEFT_PARENTHESIS);

            std::vector<const Token *> portExpressionTokens = gatherTokensUntil(TokenType::COMMA, true);

            // Pop off the comma
            portExpressionTokens.pop_back();

            // Parse the expression
            ASTNode *port = parseExpression(portExpressionTokens);

            // Gather until the right parenthesis
            std::vector<const Token *> valueExpressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            // Pop off the right parenthesis
            valueExpressionTokens.pop_back();

            // Parse the expression
            ASTNode *value = parseExpression(valueExpressionTokens);

            // Eat the semicolon
            eatToken(TokenType::SEMICOLON);

            return new WritePortNode(port, value);
        }
        else
        {
            syntaxError("WritePortNode1: Unexpected token " + tokens[currentTokenIndex].lexeme);
        }
    }
    else
    {
        syntaxError("WritePortNode2: Unexpected token " + tokens[currentTokenIndex].lexeme);
    }

    // Not reached
    return nullptr;
}

BreakNode *Parser::parseBreak()
{
    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "break")
    {
        // Eat the break keyword
        eatToken(TokenType::KEYWORD);

        // Eat the semicolon
        eatToken(TokenType::SEMICOLON);

        return new BreakNode();
    }
    else
    {
        syntaxError("BreakNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
    }

    // Not reached
    return nullptr;
}

ContinueNode *Parser::parseContinue()
{
    // Check if the current token is a keyword
    if (tokens[currentTokenIndex].type == TokenType::KEYWORD && tokens[currentTokenIndex].lexeme == "continue")
    {
        // Eat the continue keyword
        eatToken(TokenType::KEYWORD);

        // Eat the semicolon
        eatToken(TokenType::SEMICOLON);

        return new ContinueNode();
    }
    else
    {
        syntaxError("ContinueNode: Unexpected token " + tokens[currentTokenIndex].lexeme);
    }

    // Not reached
    return nullptr;
}

//================================================================================================
// ASTNode Implementations
//================================================================================================

BlockNode::BlockNode(const std::vector<ASTNode *> &statements) : statements(statements) {}

BlockNode::~BlockNode()
{
    while (!statements.empty())
    {
        ASTNode *statement = statements.back();
        statements.pop_back();
        delete statement;
    }
}

std::string BlockNode::toString() const
{
    std::string result = "BLOCK NODE {\n";
    for (ASTNode *statement : statements)
    {
        result += statement->toString() + "\n";
    }
    result += "}";
    return result;
}

ASTNodeType BlockNode::getNodeType() const
{
    return ASTNodeType::BLOCK_NODE;
}

std::vector<ASTNode *> BlockNode::getStatements() const
{
    return statements;
}

VariableDeclarationNode::VariableDeclarationNode(const std::string &identifier, const std::string &type, ASTNode *initializer) : identifier(identifier), type(type), initializer(initializer) {}

VariableDeclarationNode::~VariableDeclarationNode()
{
    delete initializer;
}

std::string VariableDeclarationNode::toString() const
{
    std::string result = "VARIABLE DECLARATION " + type + " " + identifier;
    if (initializer != nullptr)
    {
        result += " = " + initializer->toString();
    }
    return result;
}

std::string VariableDeclarationNode::getIdentifier() const
{
    return identifier;
}

std::string VariableDeclarationNode::getType() const
{
    return type;
}

ASTNode *VariableDeclarationNode::getInitializer() const
{
    return initializer;
}

ASTNodeType VariableDeclarationNode::getNodeType() const
{
    return ASTNodeType::VARIABLE_DECLARATION_NODE;
}

AssignmentNode::AssignmentNode(const std::string &identifier, ASTNode *expression) : identifier(identifier), expression(expression) {}

AssignmentNode::~AssignmentNode()
{
    delete expression;
}

std::string AssignmentNode::getIdentifier() const
{
    return identifier;
}

std::string AssignmentNode::toString() const
{
    return "ASSIGNMENT " + identifier + " = " + expression->toString();
}

ASTNode *AssignmentNode::getExpression() const
{
    return expression;
}

ASTNodeType AssignmentNode::getNodeType() const
{
    return ASTNodeType::ASSIGNMENT_NODE;
}

NumberNode::NumberNode(std::string val, TokenType type) : value(val), type(type) {}

NumberNode::~NumberNode() {}

std::string NumberNode::toString() const
{
    return "NUMBER " + value;
}

TokenType NumberNode::getType() const
{
    return type;
}

std::string NumberNode::getValue() const
{
    return value;
}

ASTNodeType NumberNode::getNodeType() const
{
    return ASTNodeType::NUMBER_NODE;
}

VariableAccessNode::VariableAccessNode(const std::string &identifier) : identifier(identifier) {}

std::string VariableAccessNode::toString() const
{
    return "VARIABLE ACCESS " + identifier;
}

std::string VariableAccessNode::getIdentifier() const
{
    return identifier;
}

ASTNodeType VariableAccessNode::getNodeType() const
{
    return ASTNodeType::VARIABLE_ACCESS_NODE;
}

VariableAccessNode::~VariableAccessNode() {}

IfNode::IfNode(ASTNode *expression, BlockNode *body) : expression(expression), body(body) {}

std::string IfNode::toString() const
{
    return "IF STATEMENT ( " + expression->toString() + " ) " + body->toString();
}

ASTNode *IfNode::getExpression() const
{
    return expression;
}

BlockNode *IfNode::getBody() const
{
    return body;
}

ASTNodeType IfNode::getNodeType() const
{
    return ASTNodeType::IF_NODE;
}

IfNode::~IfNode()
{
    delete expression;
    delete body;
}

PrintNode::PrintNode(ASTNode *expression) : expression(expression) {}

std::string PrintNode::toString() const
{
    return "PRINT STATEMENT ( " + expression->toString() + " )";
}

ASTNode *PrintNode::getExpression() const
{
    return expression;
}

ASTNodeType PrintNode::getNodeType() const
{
    return ASTNodeType::PRINT_NODE;
}

PrintNode::~PrintNode()
{
    delete expression;
}

BinaryOperationNode::BinaryOperationNode(ASTNode *left, std::string op, ASTNode *right) : left(left), op(op), right(right) {}

std::string BinaryOperationNode::toString() const
{
    return "BINARY OPERATION " + left->toString() + " " + op + " " + right->toString();
}

BinaryOperationNode::~BinaryOperationNode()
{
    delete left;
    delete right;
}

ASTNode *BinaryOperationNode::getLeftExpression() const
{
    return left;
}

ASTNode *BinaryOperationNode::getRightExpression() const
{
    return right;
}

std::string BinaryOperationNode::getOperator() const
{
    return op;
}

ASTNodeType BinaryOperationNode::getNodeType() const
{
    return ASTNodeType::BINARY_OPERATION_NODE;
}

WhileNode::WhileNode(ASTNode *expression, BlockNode *body) : expression(expression), body(body) {}

std::string WhileNode::toString() const
{
    return "WHILE LOOP ( " + expression->toString() + " ) " + body->toString();
}

ASTNode *WhileNode::getExpression() const
{
    return expression;
}

BlockNode *WhileNode::getBody() const
{
    return body;
}

ASTNodeType WhileNode::getNodeType() const
{
    return ASTNodeType::WHILE_NODE;
}

WhileNode::~WhileNode()
{
    delete expression;
    delete body;
}

WaitNode::WaitNode(ASTNode *expression) : expression(expression) {}

std::string WaitNode::toString() const
{
    return "WAIT STATEMENT ( " + expression->toString() + " )";
}

ASTNode *WaitNode::getExpression() const
{
    return expression;
}

ASTNodeType WaitNode::getNodeType() const
{
    return ASTNodeType::WAIT_NODE;
}

WaitNode::~WaitNode()
{
    delete expression;
}

SevenSegmentNode::SevenSegmentNode(ASTNode *expression) : expression(expression) {}

std::string SevenSegmentNode::toString() const
{
    return "SEVEN SEGMENT STATEMENT ( " + expression->toString() + " )";
}

ASTNode *SevenSegmentNode::getExpression() const
{
    return expression;
}

ASTNodeType SevenSegmentNode::getNodeType() const
{
    return ASTNodeType::SEVEN_SEGMENT_NODE;
}

SevenSegmentNode::~SevenSegmentNode()
{
    delete expression;
}

ReadPortNode::ReadPortNode(ASTNode *expression) : expression(expression) {}

std::string ReadPortNode::toString() const
{
    return "READ PORT STATEMENT ( " + expression->toString() + " )";
}

ASTNode *ReadPortNode::getExpression() const
{
    return expression;
}

ASTNodeType ReadPortNode::getNodeType() const
{
    return ASTNodeType::READ_PORT_NODE;
}

ReadPortNode::~ReadPortNode()
{
    delete expression;
}

WritePortNode::WritePortNode(ASTNode *port, ASTNode *value) : port(port), value(value) {}

std::string WritePortNode::toString() const
{
    return "WRITE PORT STATEMENT ( " + port->toString() + ", " + value->toString() + " )";
}

ASTNode *WritePortNode::getPort() const
{
    return port;
}

ASTNode *WritePortNode::getValue() const
{
    return value;
}

ASTNodeType WritePortNode::getNodeType() const
{
    return ASTNodeType::WRITE_PORT_NODE;
}

WritePortNode::~WritePortNode()
{
    delete port;
    delete value;
}

BreakNode::BreakNode() {}

std::string BreakNode::toString() const
{
    return "BREAK STATEMENT";
}

ASTNodeType BreakNode::getNodeType() const
{
    return ASTNodeType::BREAK_NODE;
}

BreakNode::~BreakNode() {}

ContinueNode::ContinueNode() {}

std::string ContinueNode::toString() const
{
    return "CONTINUE STATEMENT";
}

ASTNodeType ContinueNode::getNodeType() const
{
    return ASTNodeType::CONTINUE_NODE;
}

ContinueNode::~ContinueNode() {}
