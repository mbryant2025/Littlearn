#include "ast.hpp"
#include "tokenizer.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), currentTokenIndex(0) {}

// BlockNode* Parser::parseProgram() {
//     // The entry point for parsing a program into an AST.
//     // Example: Parse a block of code (e.g., the main program)

//     // Eat the leading brace
//     eatToken(TokenType::LEFT_BRACE);

//     BlockNode* programBlock = parseBlock();

//     // Check if there are any remaining tokens; if yes, report an error
//     if (currentTokenIndex < tokens.size()) {
//         syntaxError("Unexpected tokens after the program.");
//     }

//     return programBlock;
// }

void Parser::syntaxError(const std::string& message) {
    std::cerr << "Syntax Error at token " << tokens[currentTokenIndex].lexeme << ": " << message << std::endl;

    // Exit the program
    exit(1);
}

std::vector<const Token*> Parser::gatherTokensUntil(TokenType endTokenType, bool expected) {
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
        
        // std::cout << "Current brace/parenthesis counter: " << braceParenthesisCounter << " we are processing token " << tokens[currentTokenIndex].lexeme << std::endl;

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
        if (expected)
            currentTokenIndex++;
    }

    // std::cout << "Exited. Current brace/parenthesis counter: " << braceParenthesisCounter << std::endl;

    // Check if we reached the end token type
    if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == endTokenType) {
        // Add the end token to the gathered tokens as to include it in the vector
        gatheredTokens.push_back(&tokens[currentTokenIndex]);
        // Advance to the next token
        currentTokenIndex++;
    } else {
        // Expected means we expect to find the token
        if (expected)
            syntaxError("Unexpected end of file, expected " + Tokenizer::tokenTypeToString(endTokenType));
    }

    return gatheredTokens;
}

void Parser::eatToken(TokenType expectedTokenType) {
    // Eat the next token if it matches the expected token type
    if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == expectedTokenType) {
        currentTokenIndex++;
    } else {
        syntaxError("Unexpected token type, expected " + Tokenizer::tokenTypeToString(expectedTokenType) + ", got " + Tokenizer::tokenTypeToString(tokens[currentTokenIndex].type));
    }
}

// ASTNode* Parser::parseStatement() {
//     // Look at the current token and determine what type of statement (assignment, declaration, while, etc.) it is
//     // Then, parse the statement into an AST node and return it

//     // Check if there are any tokens left
//     if (currentTokenIndex >= tokens.size()) {
//         syntaxError("Unexpected end of file, expected statement.");
//     }

//     // Check if the current token is a keyword
//     if (tokens[currentTokenIndex].type == TokenType::KEYWORD) {
//         // Check which keyword it is
//         /*if (tokens[currentTokenIndex].lexeme == "while") {
//             // Parse a while loop
//             return parseWhileLoop();
//         } else if (tokens[currentTokenIndex].lexeme == "if") {
//             // Parse an if statement
//             return parseIfStatement();
//         } else*/ if (tokens[currentTokenIndex].lexeme == "int" || tokens[currentTokenIndex].lexeme == "float") {
//             // Parse a variable declaration
//             return parseVariableDeclaration();
//         } else {
//             syntaxError("Unexpected keyword " + tokens[currentTokenIndex].lexeme);
//         }
//     } else if (tokens[currentTokenIndex].type == TokenType::IDENTIFIER) {
//         // Check if the next token is an assignment operator
//         if (currentTokenIndex + 1 < tokens.size() && tokens[currentTokenIndex + 1].type == TokenType::OPERATOR && tokens[currentTokenIndex + 1].lexeme == "=") {
//             // Parse an assignment statement
//             return parseAssignment();
//         } else {
//             syntaxError("Unexpected identifier " + tokens[currentTokenIndex].lexeme);
//         }
//     } else {
//         syntaxError("Unexpected token " + tokens[currentTokenIndex].lexeme);
//     }
    
// }

// //TODO
// ASTNode* Parser::parseExpression() {
//     // Parse an expression (e.g. a constant or variable access)
//     // Example: 5, x + 3.14, (5 + 3 * (x - 2))

//     // Check if there are any tokens left
//     if (currentTokenIndex >= tokens.size()) {
//         syntaxError("Unexpected end of file, expected expression.");
//     }

//     // The expression will go until we reach either a semicolon or right brace
//     std::vector<const Token*> expressionTokensSemicolon = gatherTokensUntil(TokenType::SEMICOLON, false);
//     std::vector<const Token*> expressionTokensBrace = gatherTokensUntil(TokenType::RIGHT_BRACE, false);

//     // Take whichever one is shorter
//     std::vector<const Token*> expressionTokens = expressionTokensSemicolon.size() < expressionTokensBrace.size() ? expressionTokensSemicolon : expressionTokensBrace;

//     // Since the gatherTokensUntil function does not advance the token index when we do not expect the token, we need to do it manually
//     currentTokenIndex += expressionTokens.size();

//     // Handle the case where there is only one token
//     if (expressionTokens.size() == 1) {
//         // Check if the token is an integer or float
//         if (expressionTokens[0]->type == TokenType::INTEGER || expressionTokens[0]->type == TokenType::FLOAT) {
//             // Parse a constant
//             return parseConstant();
//         } else if (expressionTokens[0]->type == TokenType::IDENTIFIER) {
//             // Parse a variable access
//             return parseVariableAccess();
//         } else {
//             syntaxError("Unexpected token " + expressionTokens[0]->lexeme);
//         }
//     }

//     // If there is more than one token
//     else if (tokens[currentTokenIndex].type == TokenType::LEFT_PARENTHESIS) {
//         // Parse a parenthesized expression
//         return parseParenthesizedExpression();
//     } else if (tokens[currentTokenIndex].type == TokenType::IDENTIFIER || tokens[currentTokenIndex].type == TokenType::INTEGER || tokens[currentTokenIndex].type == TokenType::FLOAT) {
//         // Parse a binary expression
//         //TODO order of operations
//         return parseBinaryExpression();
//     } 
//     else {
//         syntaxError("Unexpected token " + tokens[currentTokenIndex].lexeme);
//     }
// }

// ASTNode* Parser::parseParenthesizedExpression() {
// //TODO
// }

// ASTNode* Parser::parseConstant() {
// //TODO
// }

// ASTNode* Parser::parseBinaryExpression() {
// //TODO
// }

// VariableAccessNode* parseVariableAccess() {
// //TODO
// }

// AssignmentNode* parseAssignment() {
// //TODO
// }


// BlockNode* Parser::parseBlock() {
//     // Parse a block of code (ex the main program or a loop body)

//     //Eat the leading brace
//     eatToken(TokenType::LEFT_BRACE);

//     std::vector<const Token*> tokens = gatherTokensUntil(TokenType::RIGHT_BRACE, true); // FIXME -- if there is another nested block, this will not work

//     // Parse the statements in the block
//     std::vector<ASTNode*> statements;

//     for (const Token* token : tokens) {
//         // Parse a statement
//         ASTNode* statement = parseStatement();
//         statements.push_back(statement);
//     }
    
//     // Eat the trailing brace
//     eatToken(TokenType::RIGHT_BRACE);

//     return new BlockNode(statements);
// }

// VariableDeclarationNode* Parser::parseVariableDeclaration() {
//     // Parse a variable declaration
//     // Example: int x = 5;

//     // Check if the current token is a keyword
//     if (tokens[currentTokenIndex].type == TokenType::KEYWORD) {
//         // Check which keyword it is
//         if (tokens[currentTokenIndex].lexeme == "int" || tokens[currentTokenIndex].lexeme == "float") {
//             // Parse the type
//             std::string type = tokens[currentTokenIndex].lexeme;
//             eatToken(TokenType::KEYWORD);

//             // Parse the identifier
//             std::string identifier = tokens[currentTokenIndex].lexeme;
//             eatToken(TokenType::IDENTIFIER);

//             // Check if there is an initializer
//             ASTNode* initializer = nullptr;
//             if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::OPERATOR && tokens[currentTokenIndex].lexeme == "=") {
//                 // Eat the assignment operator
//                 eatToken(TokenType::OPERATOR);

//                 // Parse the initializer
//                 initializer = parseExpression();
//             }

//             // Eat the semicolon
//             eatToken(TokenType::SEMICOLON);

//             return new VariableDeclarationNode(identifier, type, initializer);
//         } else {
//             syntaxError("Unexpected keyword " + tokens[currentTokenIndex].lexeme);
//         }
//     } else {
//         syntaxError("Unexpected token " + tokens[currentTokenIndex].lexeme);
//     }
// }

// BlockNode::BlockNode(const std::vector<ASTNode*>& statements) : statements(statements) {}

// BlockNode::~BlockNode() {
//     for (ASTNode* statement : statements) {
//         delete statement;
//     }
// }

// VariableDeclarationNode::VariableDeclarationNode(const std::string& identifier, const std::string& type, ASTNode* initializer) : identifier(identifier), type(type), initializer(initializer) {}

// VariableDeclarationNode::~VariableDeclarationNode() {
//     delete initializer;
// }
