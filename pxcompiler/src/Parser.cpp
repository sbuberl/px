
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

    Parser::Parser(SymbolTable *globals)
        : symbols(globals)
    {
    }

    void Parser::accept()
    {
        scanner->accept();
        currentToken = scanner->nextToken();
    }

    bool Parser::accept(TokenType type)
    {
        if (scanner->accept(type))
        {
            currentToken = scanner->nextToken();
            return true;
        }
        return false;
    }

    bool Parser::accept(const Utf8String &token)
    {
        if (scanner->accept(token))
        {
            currentToken = scanner->nextToken();
            return true;
        }
        return false;
    }

    void Parser::expect(TokenType type)
    {
        if (scanner->accept(type))
        {
            currentToken = scanner->nextToken();
        }
        // TODO: raise error
    }

    void Parser::expect(const Utf8String &token)
    {
        if (scanner->accept(token))
        {
            currentToken = scanner->nextToken();
        }
        // TODO: raise error
    }

    void Parser::rewind()
    {
        scanner->rewind();
        currentToken = scanner->nextToken();
    }

    AST *Parser::parse(std::istream &in)
    {
        std::stringstream buffer;
        buffer << in.rdbuf();
        Utf8String source{buffer.str()};

        scanner = new Scanner(source);
        currentToken = scanner->nextToken();

        std::unique_ptr<BlockStatement> block(new BlockStatement());

        while (currentToken.type != TokenType::END_FILE && currentToken.type != TokenType::BAD)
        {
            //std::cout << "Parsing statement " << statements.size() << std::endl;
            std::unique_ptr<Statement> statement = parseStatement();
            if (statement != nullptr)
            {
                //	std::cout << "Adding statement " << typeid(*statement).name() << std::endl;
                block->addStatement(std::move(statement));
            }
        }

        //std::cout << "Statements :" << statements.size() << std::endl;
        return new FunctionDeclaration("main", "int32", std::move(block));
    }

    std::unique_ptr<Statement> Parser::parseStatement()
    {
        if (currentToken.type == TokenType::KW_RETURN)
            return parseReturnStatement();
        else if (currentToken.type == TokenType::IDENTIFIER)
        {
            Token& next = scanner->nextToken();
            if (next.type == TokenType::IDENTIFIER)
            {
                rewind();
                return parseVariableDeclaration();
            }
            else
                rewind();
        }

        // if none of the above, parse as expression
        return parseExpressionStatement();
    }

    std::unique_ptr<Statement> Parser::parseExpressionStatement()
    {
        std::unique_ptr<Expression> expr = parseExpression();
        if (expr == nullptr)
            return nullptr;

        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<ExpressionStatement>(std::move(expr));
    }

    std::unique_ptr<Statement> Parser::parseReturnStatement()
    {
        accept();
        std::unique_ptr<Expression> retValue = nullptr;
        if (currentToken.type != TokenType::OP_END_STATEMENT)
        {
            retValue = parseExpression();
            if (retValue == nullptr)
                return nullptr;
        }

        accept(TokenType::OP_END_STATEMENT);
        return std::unique_ptr<ReturnStatement>{new ReturnStatement{ std::move(retValue) }};
    }

    std::unique_ptr<Statement> Parser::parseVariableDeclaration()
    {
        std::unique_ptr<Expression> initializer = nullptr;
        Utf8String typeName = currentToken.str;
        accept();
        Utf8String variableName = currentToken.str;
        expect(TokenType::IDENTIFIER);
        if (accept(TokenType::OP_ASSIGN))
        {
            initializer = parseExpression();
        }
        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<DeclarationStatement>(typeName, variableName, std::move(initializer));
    }

    std::unique_ptr<Expression> Parser::parseExpression()
    {
        if (currentToken.type == TokenType::IDENTIFIER)
        {
            Token next = scanner->nextToken();
            if (next.type == TokenType::OP_ASSIGN)
            {
                rewind();
                return parseAssignment();
            }

            rewind();
        }

        return parseBinary(1);
    }

    int Parser::getPrecedence(TokenType type)
    {
        switch (type)
        {
            case TokenType::OP_QUESTION:
                return 1;
            case TokenType::OP_OR:
                return 2;
            case TokenType::OP_AND:
                return 3;
            case TokenType::OP_EQUALS:
            case TokenType::OP_NOT_EQUAL:
            case TokenType::OP_LESS:
            case TokenType::OP_LESS_OR_EQUAL:
            case TokenType::OP_GREATER:
            case TokenType::OP_GREATER_OR_EQUAL:
                return 4;
            case TokenType::OP_BIT_OR:
                return 5;
            case TokenType::OP_BIT_XOR:
                return 6;
            case TokenType::OP_BIT_AND:
                return 7;
            case TokenType::OP_LEFT_SHIFT:
            case TokenType::OP_RIGHT_SHIFT:
                return 8;

            case TokenType::OP_ADD:
            case TokenType::OP_SUB:
                return 9;
            case TokenType::OP_STAR:
            case TokenType::OP_DIV:
            case TokenType::OP_MOD:
                return 10;
        }
        return 0;
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
        }
        return BinaryOperator::BAD;
    }

    std::unique_ptr<ast::Expression> Parser::parseAssignment()
    {
        expect(TokenType::IDENTIFIER);

        Utf8String variableName = currentToken.str;

        expect(TokenType::OP_ASSIGN);

        std::unique_ptr<Expression> expression = parseExpression();

        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<AssignmentExpression>(variableName, std::move(expression));
    }

    std::unique_ptr<ast::Expression> Parser::parseBinary(int precedence)
    {
        std::unique_ptr<ast::Expression> expr = parseUnary();
        for (int prec = getPrecedence(currentToken.type); prec >= precedence; prec--)
        {
            for (;;)
            {
                TokenType opType = currentToken.type;
                int op_prec = getPrecedence(opType);
                if (op_prec != prec)
                {
                    break;
                }

                expect(opType);

                if (opType == TokenType::OP_QUESTION)
                {
                    accept();
                    std::unique_ptr<Expression> trueExpr = parseExpression();
                    expect(TokenType::OP_COLON);
                    std::unique_ptr<Expression> falseExpr = parseExpression();
                    expr.reset(new TernaryOpExpression{ std::move(expr), std::move(trueExpr), std::move(falseExpr) });
                }
                else
                {
                    BinaryOperator op = getBinaryOp(opType);
                    std::unique_ptr<ast::Expression> right = parseBinary(prec + 1);

                    expr.reset(new BinaryOpExpression{ op, std::move(expr), std::move(right) });
                }
            }
        }
        return expr;
    }

    std::unique_ptr<Expression> Parser::parseUnary()
    {
        std::unique_ptr<Expression> result, right;

        switch (currentToken.type)
        {
            case TokenType::OP_ADD:
                accept();
                result = parseUnary();
                break;
            case TokenType::OP_SUB:
                accept();
                right = parseUnary();
                result = std::make_unique<UnaryOpExpression>(UnaryOperator::NEG, std::move(right));
                break;
            case TokenType::OP_NOT:
                accept();
                right = parseUnary();
                result = std::make_unique<UnaryOpExpression>(UnaryOperator::NOT, std::move(right));
            case TokenType::OP_COMPL:
                accept();
                right = parseUnary();
                result = std::make_unique<UnaryOpExpression>(UnaryOperator::CMPL, std::move(right));
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
        if (currentToken.type == TokenType::KW_AS)
        {
            accept();

            if (currentToken.type == TokenType::IDENTIFIER)
            {
                Type *type = symbols->getType(currentToken.str);
                accept();
                return std::make_unique<CastExpression>(type, std::move(result));
            }
        }

        return result;
    }

    std::unique_ptr<Expression> Parser::parseValue()
    {
        std::unique_ptr<Expression> value = nullptr;
        int32_t i32Literal;
        std::string tokenString = currentToken.str.toString();
        switch (currentToken.type)
        {
            case TokenType::IDENTIFIER:
                value.reset(new VariableExpression{ currentToken.str });
                break;
            case TokenType::INTEGER:
                i32Literal = std::stoi(tokenString);
                value.reset(new IntegerLiteral{ currentToken.str.toString(), i32Literal });
                break;
            case TokenType::HEX_INT:
                i32Literal = std::stoi(tokenString, nullptr, 16);
                value.reset(new IntegerLiteral{ currentToken.str, i32Literal });
                break;
            case TokenType::BINARY_INT:
                i32Literal = std::stoi(tokenString, nullptr, 2);
                value.reset(new IntegerLiteral{ currentToken.str.toString(), i32Literal });
                break;
            case TokenType::OCTAL_INT:
                i32Literal = std::stoi(tokenString, nullptr, 8);
                value.reset(new IntegerLiteral{ currentToken.str, i32Literal });
                break;
            case TokenType::FLOAT:
                value.reset(new FloatLiteral{ currentToken.str });
                break;
            case TokenType::STRING:
                value.reset(new StringLiteral{ currentToken.str });
                break;
            case TokenType::KW_TRUE:
            case TokenType::KW_FALSE:
                value.reset(new BoolLiteral{ currentToken.str });
                break;
            default:
                // TODO parse error
                return value;
        }

        accept();
        return value;
    }
}
