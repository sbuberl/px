
#include <iostream>
#include <sstream>
#include <typeinfo>
#include "Parser.h"
#include <ast/Literal.h>
#include <ast/Statement.h>
#include <ast/Declaration.h>

using namespace std;
using namespace px::ast;

namespace px {

    Parser::Parser(ErrorLog *errorLog) : errors{ errorLog }
    {
    }

    void Parser::accept()
    {
        scanner->accept();
        currentToken.reset(new Token(scanner->nextToken()));
    }

    bool Parser::accept(TokenType type)
    {
        if (scanner->accept(type))
        {
            currentToken.reset(new Token(scanner->nextToken()));
            return true;
        }
        return false;
    }

    void Parser::expect(TokenType type)
    {
        if (!accept(type))
        {
            Utf8String errorMesage = Utf8String{ "Expected a " } + Token::getTokenName(type) + Utf8String{ "but found a " } + Token::getTokenName(currentToken->type);
            compilerError(currentToken->position, errorMesage);
        }
    }

    void Parser::rewind()
    {
        scanner->rewind();
        currentToken.reset(new Token(scanner->nextToken()));
    }

    void Parser::compilerError(const SourcePosition &location, const Utf8String &message)
    {
        auto error = Error{ location, message };
        errors->addError(error);
        throw error;
    }

    std::unique_ptr<ast::Module> Parser::parse(const Utf8String &fileName, std::istream &in)
    {
        std::stringstream buffer;
        buffer << in.rdbuf();
        Utf8String source{buffer.str()};

        scanner.reset(new Scanner(fileName, source));
        currentToken.reset(new Token(scanner->nextToken()));

        auto startPosition = currentToken->position;
        std::unique_ptr<BlockStatement> block{ new BlockStatement{ startPosition } };

        while (currentToken->type != TokenType::END_FILE && currentToken->type != TokenType::BAD)
        {
            //std::cout << "Parsing statement " << statements.size() << std::endl;
            std::unique_ptr<Statement> statement = parseStatement();

            //	std::cout << "Adding statement " << typeid(*statement).name() << std::endl;
            block->addStatement(std::move(statement));
        }

        //std::cout << "Statements :" << statements.size() << std::endl;;
        return std::make_unique<ast::Module>(startPosition, fileName, std::move(block));

    }

    std::unique_ptr<Statement> Parser::parseStatement()
    {
        switch (currentToken->type)
        {
            case TokenType::KW_EXTERN:
                return parseExternFunction();
            case TokenType::KW_FUNC:
                return parseFunctionDeclaration();
            case TokenType::KW_IF:
                return parseIfStatement();
            case TokenType::KW_RETURN:
                return parseReturnStatement();
            case TokenType::LBRACKET:
                return parseBlockStatement();
            case TokenType::IDENTIFIER:
            {
                Token& next = scanner->nextToken();
                if (next.type == TokenType::OP_COLON)
                {
                    rewind();
                    return parseVariableDeclaration();
                }
                else if (next.type == TokenType::OP_ASSIGN)
                {
                    rewind();
                    return parseAssignment();
                }
                else
                    rewind();
            }
            default:
                // if none of the above, parse as expression
                return parseExpressionStatement();
        }


    }

    std::unique_ptr<ast::AssignmentStatement> Parser::parseAssignment()
    {
        SourcePosition start = currentToken->position;
        Utf8String variableName = currentToken->str;
        expect(TokenType::IDENTIFIER);

        expect(TokenType::OP_ASSIGN);

        std::unique_ptr<Expression> expression = parseExpression();

        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<AssignmentStatement>(start, variableName, std::move(expression));
    }

    std::unique_ptr<ast::BlockStatement> Parser::parseBlockStatement()
    {
        auto startPos = currentToken->position;
        expect(TokenType::LBRACKET);
        std::unique_ptr<BlockStatement> block{ new BlockStatement{ startPos } };

        while (currentToken->type != TokenType::RBRACKET)
        {
            std::unique_ptr<Statement> statement = parseStatement();
            block->addStatement(std::move(statement));
        }
        accept();
        return block;
    }

    std::unique_ptr<ast::ExternFunctionDeclaration> Parser::parseExternFunction()
    {
        auto startPos = currentToken->position;
        expect(TokenType::KW_EXTERN);
        std::unique_ptr<ast::FunctionPrototype> prototype = parseFunctionPrototype();
        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<ExternFunctionDeclaration>(startPos, std::move(prototype));
    }

    std::unique_ptr<ast::ExpressionStatement> Parser::parseExpressionStatement()
    {
        auto startPos = currentToken->position;
        std::unique_ptr<Expression> expr = parseExpression();

        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<ExpressionStatement>(startPos, std::move(expr));
    }

    std::unique_ptr<ast::FunctionDeclaration> Parser::parseFunctionDeclaration()
    {
        auto startPos = currentToken->position;
        std::unique_ptr<ast::FunctionPrototype> prototype = parseFunctionPrototype();
        std::unique_ptr<ast::BlockStatement> block = parseBlockStatement();
        return std::make_unique<FunctionDeclaration>(startPos, std::move(prototype), std::move(block));
    }

    std::unique_ptr<ast::FunctionPrototype> Parser::parseFunctionPrototype()
    {
        expect(TokenType::KW_FUNC);
        Utf8String functionName = currentToken->str;
        expect(TokenType::IDENTIFIER);
        expect(TokenType::LPAREN);
        std::vector<ast::Parameter> arguments;
        if (currentToken->type != TokenType::RPAREN)
        {
            do
            {
                Utf8String argName = currentToken->str;
                expect(TokenType::IDENTIFIER);
                expect(TokenType::OP_COLON);
                Utf8String argTypeName = currentToken->str;
                expect(TokenType::IDENTIFIER);
                arguments.push_back({ argName, argTypeName });
            } while (accept(TokenType::OP_COMMA));
        }
        expect(TokenType::RPAREN);
        expect(TokenType::OP_COLON);
        Utf8String returnType = currentToken->str;
        expect(TokenType::IDENTIFIER);
        return std::make_unique<FunctionPrototype>(functionName, returnType, arguments);
    }

    std::unique_ptr<ast::IfStatement> Parser::parseIfStatement()
    {
        auto startPos = currentToken->position;
        expect(TokenType::KW_IF);
        expect(TokenType::LPAREN);
        std::unique_ptr<Expression> condition = parseExpression();
        expect(TokenType::RPAREN);
        std::unique_ptr<Statement> trueClause = parseStatement();
        std::unique_ptr<Statement> elseClause = nullptr;
        if (accept(TokenType::KW_ELSE))
            elseClause = parseStatement();
        return std::make_unique<IfStatement>(startPos, std::move(condition), std::move(trueClause), std::move(elseClause));
    }

    std::unique_ptr<ReturnStatement> Parser::parseReturnStatement()
    {
        auto startPos = currentToken->position;
        expect(TokenType::KW_RETURN);
        std::unique_ptr<Expression> retValue = nullptr;
        if (currentToken->type != TokenType::OP_END_STATEMENT)
        {
            retValue = parseExpression();
        }

        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<ReturnStatement>(startPos, std::move(retValue));
    }

    std::unique_ptr<VariableDeclaration> Parser::parseVariableDeclaration()
    {
        SourcePosition start = currentToken->position;
        std::unique_ptr<Expression> initializer = nullptr;
        Utf8String variableName = currentToken->str;
        expect(TokenType::IDENTIFIER);
        expect(TokenType::OP_COLON);
        Utf8String typeName = currentToken->str;
        expect(TokenType::IDENTIFIER);
        if (accept(TokenType::OP_ASSIGN))
        {
            initializer = parseExpression();
        }
        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<VariableDeclaration>(start, typeName, variableName, std::move(initializer));
    }

    int Parser::getPrecedence(TokenType type)
    {
        switch (type)
        {
            case TokenType::OP_OR:
                return 1;
            case TokenType::OP_AND:
                return 2;
            case TokenType::OP_EQUALS:
            case TokenType::OP_NOT_EQUAL:
            case TokenType::OP_LESS:
            case TokenType::OP_LESS_OR_EQUAL:
            case TokenType::OP_GREATER:
            case TokenType::OP_GREATER_OR_EQUAL:
                return 3;
            case TokenType::OP_ADD:
            case TokenType::OP_SUB:
            case TokenType::OP_BIT_OR:
            case TokenType::OP_BIT_XOR:
                return 4;
            case TokenType::OP_STAR:
            case TokenType::OP_DIV:
            case TokenType::OP_MOD:
            case TokenType::OP_BIT_AND:
            case TokenType::OP_LEFT_SHIFT:
            case TokenType::OP_RIGHT_SHIFT:
                return 5;
            default:
                return 0;
        }
    }

    BinaryOperator Parser::getBinaryOp(TokenType type)
    {
        switch (type)
        {
            case TokenType::OP_OR:
                return BinaryOperator::OR;
            case TokenType::OP_AND:
                return BinaryOperator::AND;
            case TokenType::OP_EQUALS:
                return BinaryOperator::EQ;
            case TokenType::OP_NOT_EQUAL:
                return BinaryOperator::NE;
            case TokenType::OP_LESS:
                return BinaryOperator::LT;
            case TokenType::OP_LESS_OR_EQUAL:
                return BinaryOperator::LTE;
            case TokenType::OP_GREATER:
                return BinaryOperator::GT;
            case TokenType::OP_GREATER_OR_EQUAL:
                return BinaryOperator::GTE;
            case TokenType::OP_BIT_OR:
                return BinaryOperator::BIT_OR;
            case TokenType::OP_BIT_XOR:
                return BinaryOperator::BIT_XOR;
            case TokenType::OP_BIT_AND:
                return BinaryOperator::BIT_AND;
            case TokenType::OP_LEFT_SHIFT:
                return BinaryOperator::LSH;
            case TokenType::OP_RIGHT_SHIFT:
                return BinaryOperator::RSH;
            case TokenType::OP_ADD:
                return BinaryOperator::ADD;
            case TokenType::OP_SUB:
                return BinaryOperator::SUB;
            case TokenType::OP_STAR:
                return BinaryOperator::MUL;
            case TokenType::OP_DIV:
                return BinaryOperator::DIV;
            case TokenType::OP_MOD:
                return BinaryOperator::MOD;
            default:
                return BinaryOperator::BAD;
        }
    }

    std::unique_ptr<Expression> Parser::parseExpression()
    {
        auto startPos = currentToken->position;
        std::unique_ptr<Expression> expr = parseBinary();
        if (accept(TokenType::OP_QUESTION))
        {
            std::unique_ptr<Expression> trueExpr = parseExpression();
            expect(TokenType::OP_COLON);
            std::unique_ptr<Expression> falseExpr = parseExpression();
            expr.reset(new TernaryOpExpression{ startPos, std::move(expr), std::move(trueExpr), std::move(falseExpr) });
        }
        return expr;
    }

    std::unique_ptr<ast::Expression> Parser::parseBinary(int precedence)
    {
        std::unique_ptr<ast::Expression> expr = parseUnary();
        auto start = currentToken->position;
        for (int prec = getPrecedence(currentToken->type); prec >= precedence; prec--)
        {
            for (;;)
            {
                TokenType opType = currentToken->type;
                int op_prec = getPrecedence(opType);
                if (op_prec != prec)
                {
                    break;
                }

                accept(opType);

                BinaryOperator op = getBinaryOp(opType);
                std::unique_ptr<ast::Expression> right = parseBinary(prec + 1);
                expr.reset(new BinaryOpExpression{ start, op, std::move(expr), std::move(right) });
            }
        }
        return expr;
    }

    std::unique_ptr<Expression> Parser::parseUnary()
    {
        SourcePosition start = currentToken->position;
        std::unique_ptr<Expression> result, right;

        switch (currentToken->type)
        {
            case TokenType::OP_ADD:
                accept();
                result = parseUnary();
                break;
            case TokenType::OP_SUB:
                accept();
                right = parseUnary();
                result = std::make_unique<UnaryOpExpression>(start, UnaryOperator::NEG, std::move(right));
                break;
            case TokenType::OP_NOT:
                accept();
                right = parseUnary();
                result = std::make_unique<UnaryOpExpression>(start, UnaryOperator::NOT, std::move(right));
            case TokenType::OP_COMPL:
                accept();
                right = parseUnary();
                result = std::make_unique<UnaryOpExpression>(start, UnaryOperator::CMPL, std::move(right));
            case TokenType::LPAREN:
            {
                accept();
                std::unique_ptr<Expression> expression = parseExpression();
                expect(TokenType::RPAREN);
                return expression;
            }
            default:
                result = parseValue();
        }

        // cast
        if (accept(TokenType::KW_AS))
        {
            if (currentToken->type == TokenType::IDENTIFIER)
            {
                Utf8String newTypeName = currentToken->str;
                accept();
                return std::make_unique<CastExpression>(start, newTypeName, std::move(result));
            }
            else
            {
                Utf8String errorMesage = Utf8String{ "Expected an identifer after as but found a " } + Token::getTokenName(currentToken->type);
                compilerError(currentToken->position, errorMesage);
            }
        }

        return result;
    }

    std::unique_ptr<Expression> Parser::parseValue()
    {
        std::unique_ptr<Expression> value = nullptr;
        int64_t i64Literal;
        auto start = currentToken->position;
        std::string tokenString = currentToken->str.toString();
        Type *suffix = currentToken->suffixType;
        switch (currentToken->type)
        {
            case TokenType::IDENTIFIER:
            {
                {
                    Utf8String identifier = currentToken->str;
                    Token &nextToken = scanner->nextToken();
                    if (nextToken.type == TokenType::LPAREN)
                    {
                        accept();
                        std::vector<std::unique_ptr<Expression>> arguments;
                        if (currentToken->type != TokenType::RPAREN)
                        {
                            do
                            {
                                std::unique_ptr<Expression> argument = parseExpression();
                                arguments.push_back(std::move(argument));
                            } while (accept(TokenType::OP_COMMA));
                        }
                        expect(TokenType::RPAREN);
                        value.reset(new FunctionCallExpression{ start, identifier, std::move(arguments) });
                        return value;
                    }
                    else
                    {
                        rewind();
                        value.reset(new VariableExpression{ start, identifier });
                    }
                }
                break;
            }
            case TokenType::INTEGER:
                i64Literal = std::stoll(tokenString, nullptr, currentToken->integerBase);
                value.reset(new IntegerLiteral{ start, suffix, currentToken->str, i64Literal });
                break;
            case TokenType::FLOAT:
                value.reset(new FloatLiteral{ start, suffix, currentToken->str });
                break;
            case TokenType::CHAR:
                value.reset(new CharLiteral{ start, currentToken->str });
                break;
            case TokenType::STRING:
                value.reset(new StringLiteral{ start, currentToken->str });
                break;
            case TokenType::KW_TRUE:
            case TokenType::KW_FALSE:
                value.reset(new BoolLiteral{ start, currentToken->str });
                break;
            default:
                Utf8String errorMesage = Utf8String{ "Unknown value: " } + Token::getTokenName(currentToken->type);
                compilerError(currentToken->position, errorMesage);
                break;
        }

        accept();
        return value;
    }
}
