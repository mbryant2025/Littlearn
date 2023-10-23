#include "ast.hpp"
#include "tokenizer.hpp"
#include "error.hpp"


Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), currentTokenIndex(0) {}

void Parser::syntaxError(const std::string &message)
{
    handleError("Syntax Error at token " + std::to_string(currentTokenIndex+1) + ": " + tokens[currentTokenIndex].lexeme + ": " + message);
}

BlockNode* Parser::parseProgram()
{
    // The entry point for parsing a program into an AST.
    // Example: Parse a block of code (e.g., the main program)

    BlockNode* programBlock = parseBlock();

    // Check if there are any remaining tokens; if yes, report an error
    if (currentTokenIndex < tokens.size())
    {
        syntaxError("Unexpected tokens after the program.");
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

std::vector<const Token*> Parser::gatherTokensUntil(TokenType endTokenType, bool advanceIndex) {
    // Gather tokens until the end token type is reached
    //Precondition: for wrapping syntax, the currentTokenIndex will be immediately after the opening version
    // Ex. If we are looking for } we should be right after the { (which should have been eaten prior to this call)

    // If we want a closing parenthesis or brace, we need to keep a counter
    // This counter will go up with every left brace/parenthesis and down with every right brace/parenthesis
    // When the counter reaches 0, we know we have reached the end of the block
    // A syntax error will be thrown if the counter goes below 0
    // A syntax error will also be thrown if we reach the end of the file without the counter reaching 0
    // The last case for a syntax error is that of {(})

    
    size_t braceParenthesisCounter = 0;

    int startingIndex = currentTokenIndex;

     // This is just a placeholder, it will be overwritten. It also indicates if we are looking for a brace or parenthesis
     // It just keeps track of the opened ones: ( or {
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
    // we should go if (we are not looking for a brace or parenthesis and the current token is not the end token type) OR (we are looking for a brace or parenthesis and the counter is not 0)
    while (currentTokenIndex < tokens.size() && ((lastBraceParenthesis == TokenType::UNKNOWN && tokens[currentTokenIndex].type != endTokenType) || (lastBraceParenthesis != TokenType::UNKNOWN && braceParenthesisCounter != 0))) {
        
        // If we are looking for a brace or parenthesis, check if the current token is one
        if (lastBraceParenthesis != TokenType::UNKNOWN) {

            TokenType currentTokenType = tokens[currentTokenIndex].type;
            // Increment or decrement the counter
            if (currentTokenType == TokenType::LEFT_BRACE || currentTokenType == TokenType::LEFT_PARENTHESIS) {
                braceParenthesisCounter++;
            } else if (currentTokenType == TokenType::RIGHT_BRACE || currentTokenType == TokenType::RIGHT_PARENTHESIS) {
                braceParenthesisCounter--;

                // If we decremented the counter to 0 and the current token is not the end token type, we have an error
                if (braceParenthesisCounter == 0 && currentTokenType != endTokenType) {
                    syntaxError("Unexpected brace or parenthesis " + tokens[currentTokenIndex].lexeme);
                }

                // If we decremented the counter to -1, we have an error
                if (braceParenthesisCounter < 0) {
                    syntaxError("Unexpected brace or parenthesis " + tokens[currentTokenIndex].lexeme);
                }

                // If we decremented the counter to 0 and the current token is the end token type, we are done
                if (braceParenthesisCounter == 0 && currentTokenType == endTokenType) {
                    break;
                }
            }

            // Check for the condition where we have something like {(})
            if ((currentTokenType == TokenType::RIGHT_BRACE && lastBraceParenthesis == TokenType::LEFT_PARENTHESIS)
            || (currentTokenType == TokenType::RIGHT_PARENTHESIS && lastBraceParenthesis == TokenType::LEFT_BRACE)) {
                syntaxError("Unexpected brace or parenthesis " + tokens[currentTokenIndex].lexeme);
            }

            // If it is an opening brace or parenthesis, update the last brace/parenthesis
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
    }

    if(!advanceIndex)
        currentTokenIndex = startingIndex;

    return gatheredTokens;
}

size_t Parser::getPrecedence(std::string lexeme) {
    // Returns the precedence of the given token type
    // Higher number is higher precedence

    if (lexeme == "*" || lexeme == "/" || lexeme == "%") {
        return 2;
    } else if (lexeme == "+" || lexeme == "-") {
        return 1;
    } else if (lexeme == ">" || lexeme == "<") {
        return 0;
    } else {
        syntaxError("Unexpected operator " + lexeme);
    }

    // Not reached
    return -1;
}

ASTNode* Parser::parseExpression(std::vector<const Token*> expressionTokens)
{
    // Parses expressions without parentheses
    // Ex. 5, x, 3.14, x + 5, 5 * 3.14, x + 5 * 3.14, 4 * 3 + 18
    // Non-ex. (5), (x), (3.14), (x + 5), (5 * 3.14), 5 * (x + 5 * 3.14)

    // Check if the expression is enclosed in parentheses
    if (expressionTokens[0]->type == TokenType::LEFT_PARENTHESIS && expressionTokens.back()->type == TokenType::RIGHT_PARENTHESIS) {
        // Remove the outer parentheses
        std::vector<const Token*> innerTokens(expressionTokens.begin() + 1, expressionTokens.end() - 1);

        // Recursively parse the inner expression
        return parseExpression(innerTokens);
    }

    // Handle the case when there is one token
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
        }
    }

    // There will be 4 tokens when there is a read_port call
    if (expressionTokens.size() == 4) {
        if (expressionTokens[0]->type == TokenType::KEYWORD) {

            //Parse read_port
            if(expressionTokens[0]->lexeme == "read_port") {

                // TODO don't hardcode for single read (i.e. read_port(1) or read_port(x) rather than read_port(x+1))

                // Check that the second token is a left parenthesis
                if (expressionTokens[1]->type != TokenType::LEFT_PARENTHESIS) {
                    syntaxError("Parse Expression: Unexpected token " + expressionTokens[1]->lexeme);
                }

                // Check that the fourth token is a right parenthesis
                if (expressionTokens[3]->type != TokenType::RIGHT_PARENTHESIS) {
                    syntaxError("Parse Expression: Unexpected token " + expressionTokens[3]->lexeme);
                }

                std::vector<const Token*> innerTokens(expressionTokens.begin() + 2, expressionTokens.begin() + 3);

                // Make a vector of tokens for the expression inside the parentheses -- right now it is just the third token
                ASTNode* port = parseExpression(innerTokens);

                return new ReadPortNode(port);
            } else {
                syntaxError("Parse Expression: Unexpected keyword " + expressionTokens[0]->lexeme);
            }
        } else {
            syntaxError("Parse Expression: Unexpected token " + expressionTokens[0]->lexeme);
        }
    }

    // TEMPORARY TODO REMOVE
    // Handle the case when there are three tokens
    if (expressionTokens.size() == 3) {
        // Check if the middle token is an operator
        if (expressionTokens[1]->type == TokenType::OPERATOR) {
            // Parse the left and right expressions
            ASTNode* leftExpression = parseExpression(std::vector<const Token*>(expressionTokens.begin(), expressionTokens.begin() + 1));
            ASTNode* rightExpression = parseExpression(std::vector<const Token*>(expressionTokens.begin() + 2, expressionTokens.end()));

            // Create a binary operation node based on the operator
            BinaryOperationNode* binaryNode = new BinaryOperationNode(leftExpression, expressionTokens[1]->lexeme, rightExpression);

            return binaryNode;
        } else {
            syntaxError("Unexpected token " + expressionTokens[1]->lexeme);
        }
    }

    // Multiple tokens

    // Want to loop through them and convert:
    // 5 * 8 + x * (4 + 13)
    // to
    // NumberNode * NumberNode + VariableAccessNode * ASTNode

    // Then we can loop through the vector and find the highest precedence operator
    // Then we can split the vector into two vectors based on the operator

    // We can then recursively call this function on the two vectors and create a BinaryOperationNode

    std::vector<ASTNode*> highLevelNodes;
    std::vector<const Token*> highLevelTokens; //parallel vector to highLevelNodes


    for (int i = 0; i < expressionTokens.size(); i++) {
        const Token* token = expressionTokens[i];

        // Add number nodes or variable access nodes to the high level nodes vector

        // If we reach an operator, add it to the high level tokens vector

        // If we find parentheses, we need to recursively call this function on the inner expression

        if (token->type == TokenType::INTEGER || token->type == TokenType::FLOAT) {
            // Parse the constant
            highLevelNodes.push_back(new NumberNode(token->lexeme, token->type));
        } else if (token->type == TokenType::IDENTIFIER) {
            // Parse the variable access
            highLevelNodes.push_back(new VariableAccessNode(token->lexeme));
        } else if (token->type == TokenType::OPERATOR) {
            // Add the operator to the high level tokens vector
            highLevelTokens.push_back(token);
        } else if (token->type == TokenType::LEFT_PARENTHESIS) {
            // Find the matching right parenthesis
            int leftParenthesisCounter = 1;
            int j = i + 1;
            while (j < expressionTokens.size()) {
                if (expressionTokens[j]->type == TokenType::LEFT_PARENTHESIS) {
                    leftParenthesisCounter++;
                } else if (expressionTokens[j]->type == TokenType::RIGHT_PARENTHESIS) {
                    leftParenthesisCounter--;
                }

                if (leftParenthesisCounter == 0) {
                    break;
                }

                j++;
            }

            // If we reached the end of the expression without finding a matching right parenthesis, we have an error
            if (j == expressionTokens.size()) {
                syntaxError("Unexpected end of file, expected )");
            }

            // Create a vector of tokens for the inner expression
            std::vector<const Token*> innerTokens(expressionTokens.begin() + i + 1, expressionTokens.begin() + j);

            // Recursively parse the inner expression
            ASTNode* innerExpression = parseExpression(innerTokens);

            // Add the inner expression to the high level nodes vector
            highLevelNodes.push_back(innerExpression);

            // Update i to skip over the inner expression
            i = j;
        } else {
            syntaxError("Unexpected token " + token->lexeme);
        }
    }

    
    // Now to deal with operator precedence

    // Highest: * / -> precedence = 1
    // Lowest: + -  -> precedence = 0

    // We want to loop through the high level tokens vector and find the highest precedence operator

    // If we find an operator with the highest precedence, we want to split the high level nodes vector into two vectors based on the operator
    // Otherwise, we want to split the high level nodes vector into two vectors based on the first operator we find

    // We can then recursively call this function on the two vectors and create a BinaryOperationNode

    // Loop through the high level tokens vector and find the highest precedence operator
    int highestPrecedence = -1;
    int highestPrecedenceIndex = -1;

    for (int i = 0; i < highLevelTokens.size(); i++) {
        const Token* token = highLevelTokens[i];

        // Get the precedence of the current operator
        int precedence = getPrecedence(token->lexeme);

        // If the precedence is higher than the highest precedence, update the highest precedence and highest precedence index
        if (precedence > highestPrecedence) {
            highestPrecedence = precedence;
            highestPrecedenceIndex = i;
        }
    }

    // If we found an operator with the highest precedence, split the high level nodes vector into two vectors based on the operator
    // Otherwise, split the high level nodes vector into two vectors based on the first operator we find

    // Create the left and right vectors
    std::vector<ASTNode*> leftNodes;
    std::vector<ASTNode*> rightNodes;

    // Create the left and right tokens vectors
    std::vector<const Token*> leftTokens;
    std::vector<const Token*> rightTokens;

    // Ex if our expression is 5 * 8 + x * (4 + 13)
    // In this case we have 5 and 8 + x * (4 + 13) as our left and right node vectors
    // The left token vector is empty and the right token vector is + and *

    // TODO offsets here due to fence post problem

    // // At this point we have a mix of ASTNodes and Tokens in the left and right vectors
    // ASTNode* leftNode = parseSimpleExpression(leftTokens, leftNodes);
    // ASTNode* rightNode = parseSimpleExpression(rightTokens, rightNodes);

    // // Create a binary operation node based on the operator
    // const Token* operatorToken = highLevelTokens[highestPrecedenceIndex];
    // BinaryOperationNode* binaryNode = new BinaryOperationNode(leftNode, operatorToken->lexeme, rightNode);

    // return binaryNode;

    return nullptr;

} 

ASTNode* Parser::parseSimpleExpression(std::vector<const Token*> exprTokens, std::vector<ASTNode*> nodes) {
    // This function handles the case where we have a vector of tokens and a vector of ASTNodes
    // Should be parallel vectors and not have any parentheses

    // Check to make sure the inputs are valid
    
    // Make sure the size of nodes is at least 1
    if (nodes.size() == 0) {
        syntaxError("Unexpected empty vector of nodes");
    }

    // Make sure the size of nodes is one more than the size of tokens
    if (nodes.size() != exprTokens.size() + 1) {
        syntaxError("Unexpected number of nodes and tokens");
    }

    // For only one node, return it
    if (nodes.size() == 1) {
        return nodes[0];
    }

    // Ugly copy and paste to deal with operator precedence


    // Now to deal with operator precedence

    // Highest: * / -> precedence = 1
    // Lowest: + -  -> precedence = 0

    // We want to loop through the high level tokens vector and find the highest precedence operator

    // If we find an operator with the highest precedence, we want to split the high level nodes vector into two vectors based on the operator
    // Otherwise, we want to split the high level nodes vector into two vectors based on the first operator we find

    // We can then recursively call this function on the two vectors and create a BinaryOperationNode

    // Loop through the high level tokens vector and find the highest precedence operator
    int highestPrecedence = -1;
    int highestPrecedenceIndex = -1;
    for (int i = 0; i < exprTokens.size(); i++) {
        const Token* token = exprTokens[i];

        // Get the precedence of the current operator
        int precedence = getPrecedence(token->lexeme);

        // If the precedence is higher than the highest precedence, update the highest precedence and highest precedence index
        if (precedence > highestPrecedence) {
            highestPrecedence = precedence;
            highestPrecedenceIndex = i;
        }
    }

    // If we found an operator with the highest precedence, split the high level nodes vector into two vectors based on the operator
    // Otherwise, split the high level nodes vector into two vectors based on the first operator we find

    // Create the left and right vectors
    std::vector<ASTNode*> leftNodes;
    std::vector<ASTNode*> rightNodes;

    // Create the left and right tokens vectors
    std::vector<const Token*> leftTokens;
    std::vector<const Token*> rightTokens;

    if (highestPrecedenceIndex != -1) {
        // Split the high level nodes vector into two vectors based on the operator with the highest precedence
        leftNodes = std::vector<ASTNode*>(nodes.begin(), nodes.begin() + highestPrecedenceIndex);
        rightNodes = std::vector<ASTNode*>(nodes.begin() + highestPrecedenceIndex + 1, nodes.end());

        // Split the high level tokens vector into two vectors based on the operator with the highest precedence
        leftTokens = std::vector<const Token*>(exprTokens.begin(), exprTokens.begin() + highestPrecedenceIndex);
        rightTokens = std::vector<const Token*>(exprTokens.begin() + highestPrecedenceIndex + 1, exprTokens.end());
    } else {
        // Split the high level nodes vector into two vectors based on the first operator we find
        leftNodes = std::vector<ASTNode*>(nodes.begin(), nodes.begin() + 1);
        rightNodes = std::vector<ASTNode*>(nodes.begin() + 1, nodes.end());

        // Split the high level tokens vector into two vectors based on the first operator we find
        leftTokens = std::vector<const Token*>(exprTokens.begin(), exprTokens.begin() + 1);
        rightTokens = std::vector<const Token*>(exprTokens.begin() + 1, exprTokens.end());
    }

    return new BinaryOperationNode(parseSimpleExpression(leftTokens, leftNodes), exprTokens[highestPrecedenceIndex]->lexeme, parseSimpleExpression(rightTokens, rightNodes));
}

NumberNode* Parser::parseConstant()
{
    // Check if the current token is an integer or float
    if (tokens[currentTokenIndex].type == TokenType::INTEGER || tokens[currentTokenIndex].type == TokenType::FLOAT)
    {
        // Parse the constant
        NumberNode* constant = new NumberNode(tokens[currentTokenIndex].lexeme, tokens[currentTokenIndex].type);
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

VariableAccessNode* Parser::parseVariableAccess()
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

AssignmentNode* Parser::parseAssignment()
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

            std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::SEMICOLON, true);

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

IfNode* Parser::parseIfStatement() {
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

            std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            //Pop off the right parenthesis
            expressionTokens.pop_back();

            // Parse the expression
            ASTNode *expression = parseExpression(expressionTokens);

            // Parse the block
            BlockNode* block = parseBlock();

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

BlockNode* Parser::parseBlock()
{
    // Parse a block of code (ex the main program or a loop body)

    // Eat the leading brace
    eatToken(TokenType::LEFT_BRACE);

    // The resulting vector of tokens should be parsed into a vector of ASTNodes
    std::vector<ASTNode*> statements;

    // This is where we need to differentiate between assignment, declaration, if, while, ...
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
            if (token->lexeme == "int" || token->lexeme == "float") {
                // Parse the variable declaration
                statements.push_back(parseVariableDeclaration());
            } else if (token->lexeme == "if") {
                // Parse the if statement
                statements.push_back(parseIfStatement());
            } else if (token->lexeme == "print") {
                // Parse the print statement
                statements.push_back(parsePrint());
            } else if (token->lexeme == "while") {
                // Parse the while loop
                statements.push_back(parseWhile());
            } else if (token->lexeme == "wait") {
                // Parse the wait statement
                statements.push_back(parseWait());
            } else if (token->lexeme == "print_seven_segment") {
                // Parse the seven segment statement
                statements.push_back(parseSevenSegment());
            } else if (token->lexeme == "write_port") {
                // Parse the write port statement
                statements.push_back(parseWritePort());
            } else {
                syntaxError("BlockNode1: Unexpected keyword " + token->lexeme);
            }
        } else if (token->type == TokenType::IDENTIFIER) {
            statements.push_back(parseAssignment());
        } else {
            syntaxError("BlockNode3: Unexpected token " + tokens[currentTokenIndex].lexeme);
        }
    }

    // Eat the trailing brace
    eatToken(TokenType::RIGHT_BRACE);

    return new BlockNode(statements);
}

VariableDeclarationNode* Parser::parseVariableDeclaration()
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

                std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::SEMICOLON, true);

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

PrintNode* Parser::parsePrint() {
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

            std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            //Pop off the right parenthesis
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

WhileNode* Parser::parseWhile() {
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

            std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            //Pop off the right parenthesis
            expressionTokens.pop_back();

            // Parse the expression
            ASTNode *expression = parseExpression(expressionTokens);

            // Parse the block
            BlockNode* block = parseBlock();

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

WaitNode* Parser::parseWait() {
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

            std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            //Pop off the right parenthesis
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

SevenSegmentNode* Parser::parseSevenSegment() {
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

            std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            //Pop off the right parenthesis
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

ReadPortNode* Parser::parseReadPort() {
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

            std::vector<const Token*> expressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            //Pop off the right parenthesis
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

WritePortNode* Parser::parseWritePort() {
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

            std::vector<const Token*> portExpressionTokens = gatherTokensUntil(TokenType::COMMA, true);

            //Pop off the comma
            portExpressionTokens.pop_back();

            // Parse the expression
            ASTNode *port = parseExpression(portExpressionTokens);

            // Gather until the right parenthesis
            std::vector<const Token*> valueExpressionTokens = gatherTokensUntil(TokenType::RIGHT_PARENTHESIS, true);

            //Pop off the right parenthesis
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

//================================================================================================
// ASTNode Implementations
//================================================================================================

BlockNode::BlockNode(const std::vector<ASTNode *> &statements) : statements(statements) {}

BlockNode::~BlockNode()
{
    while (!statements.empty())
    {
        ASTNode* statement = statements.back();
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

std::string BlockNode::getNodeType() const
{
    return "block";
}

std::vector<ASTNode*> BlockNode::getStatements() const {
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

ASTNode* VariableDeclarationNode::getInitializer() const {
    return initializer;
}

std::string VariableDeclarationNode::getNodeType() const
{
    return "variableDeclaration";
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

ASTNode* AssignmentNode::getExpression() const {
    return expression;
}

std::string AssignmentNode::getNodeType() const
{
    return "assignment";
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

std::string NumberNode::getNodeType() const
{
    return "number";
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

std::string VariableAccessNode::getNodeType() const
{
    return "variableAccess";
}

VariableAccessNode::~VariableAccessNode() {}

IfNode::IfNode(ASTNode* expression, BlockNode* body) : expression(expression), body(body) {}

std::string IfNode::toString() const {
    return "IF STATEMENT ( " + expression->toString() + " ) " + body->toString();
}

ASTNode* IfNode::getExpression() const {
    return expression;
}

BlockNode* IfNode::getBody() const {
    return body;
}

std::string IfNode::getNodeType() const {
    return "if";
}

IfNode::~IfNode() {
    delete expression;
    delete body;
}

PrintNode::PrintNode(ASTNode* expression) : expression(expression) {}

std::string PrintNode::toString() const {
    return "PRINT STATEMENT ( " + expression->toString() + " )";
}

ASTNode* PrintNode::getExpression() const {
    return expression;
}

std::string PrintNode::getNodeType() const {
    return "print";
}

PrintNode::~PrintNode() {
    delete expression;
}

BinaryOperationNode::BinaryOperationNode(ASTNode* left, std::string op, ASTNode* right) : left(left), op(op), right(right) {}

std::string BinaryOperationNode::toString() const {
    return "BINARY OPERATION " + left->toString() + " " + op + " " + right->toString();
}

BinaryOperationNode::~BinaryOperationNode() {
    delete left;
    delete right;
}

ASTNode* BinaryOperationNode::getLeftExpression() const {
    return left;
}

ASTNode* BinaryOperationNode::getRightExpression() const {
    return right;
}

std::string BinaryOperationNode::getOperator() const {
    return op;
}

std::string BinaryOperationNode::getNodeType() const {
    return "binaryOperation";
}

WhileNode::WhileNode(ASTNode* expression, BlockNode* body) : expression(expression), body(body) {}

std::string WhileNode::toString() const {
    return "WHILE LOOP ( " + expression->toString() + " ) " + body->toString();
}

ASTNode* WhileNode::getExpression() const {
    return expression;
}

BlockNode* WhileNode::getBody() const {
    return body;
}

std::string WhileNode::getNodeType() const {
    return "while";
}

WhileNode::~WhileNode() {
    delete expression;
    delete body;
}

WaitNode::WaitNode(ASTNode* expression) : expression(expression) {}

std::string WaitNode::toString() const {
    return "WAIT STATEMENT ( " + expression->toString() + " )";
}

ASTNode* WaitNode::getExpression() const {
    return expression;
}

std::string WaitNode::getNodeType() const {
    return "wait";
}

WaitNode::~WaitNode() {
    delete expression;
}

SevenSegmentNode::SevenSegmentNode(ASTNode* expression) : expression(expression) {}

std::string SevenSegmentNode::toString() const {
    return "SEVEN SEGMENT STATEMENT ( " + expression->toString() + " )";
}

ASTNode* SevenSegmentNode::getExpression() const {
    return expression;
}

std::string SevenSegmentNode::getNodeType() const {
    return "sevenSegment";
}

SevenSegmentNode::~SevenSegmentNode() {
    delete expression;
}

ReadPortNode::ReadPortNode(ASTNode* expression) : expression(expression) {}

std::string ReadPortNode::toString() const {
    return "READ PORT STATEMENT ( " + expression->toString() + " )";
}

ASTNode* ReadPortNode::getExpression() const {
    return expression;
}

std::string ReadPortNode::getNodeType() const {
    return "readPort";
}

ReadPortNode::~ReadPortNode() {
    delete expression;
}

WritePortNode::WritePortNode(ASTNode* port, ASTNode* value) : port(port), value(value) {}

std::string WritePortNode::toString() const {
    return "WRITE PORT STATEMENT ( " + port->toString() + ", " + value->toString() + " )";
}

ASTNode* WritePortNode::getPort() const {
    return port;
}

ASTNode* WritePortNode::getValue() const {
    return value;
}

std::string WritePortNode::getNodeType() const {
    return "writePort";
}

WritePortNode::~WritePortNode() {
    delete port;
    delete value;
}

