#include "ast.hpp"
#include "tokenizer.hpp"

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), currentTokenIndex(0) {}

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

void Parser::syntaxError(const std::string &message)
{
    std::cerr << "Syntax Error at token " << currentTokenIndex+1 << ": " << tokens[currentTokenIndex].lexeme << ": " << message << std::endl;

    // Exit the program
    exit(1);
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

ASTNode* Parser::parseStatement()
{
    // TODO
    return nullptr;
}

ASTNode* Parser::parseExpression(std::vector<const Token*> expressionTokens)
{
    // Parse an expression (e.g. a constant or variable access)
    // This is passed in as a vector of tokens
    // Example: 5, x + 3.14, (5 + 3 * (x - 2))

    //print the expression tokens
    std::cout << "Expression tokens for parse expression: ";
    for (auto token : expressionTokens) {
        std::cout << token->lexeme << "\n";
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

    // Handle the case when there are more than one tokens
    syntaxError("Not implemented yet");

    
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
            // } else if (token->lexeme == "while") {
            //     // Parse the while loop
            //     statements.push_back(parseWhileLoop());
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

//================================================================================================
// ASTNode Implementations
//================================================================================================

BlockNode::BlockNode(const std::vector<ASTNode *> &statements) : statements(statements) {}

BlockNode::~BlockNode()
{
    for (ASTNode *statement : statements)
    {
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
    result += ";";
    return result;
}

AssignmentNode::AssignmentNode(const std::string &identifier, ASTNode *expression) : identifier(identifier), expression(expression) {}

AssignmentNode::~AssignmentNode()
{
    delete expression;
}

std::string AssignmentNode::toString() const
{
    return "ASSIGNMENT " + identifier + " = " + expression->toString() + ";";
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

VariableAccessNode::VariableAccessNode(const std::string &identifier) : identifier(identifier) {}

std::string VariableAccessNode::toString() const
{
    return "VARIABLE ACCESS " + identifier;
}

VariableAccessNode::~VariableAccessNode() {}

IfNode::IfNode(ASTNode* expression, BlockNode* body) : expression(expression), body(body) {}

std::string IfNode::toString() const {
    return "IF STATEMENT ( " + expression->toString() + " ) " + body->toString();
}

IfNode::~IfNode() {
    delete expression;
    delete body;
}