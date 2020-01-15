
#include <iostream>
#include <sstream>
#include <typeinfo>
#include "Parser.h"
#include <ast/Literal.h>
#include <ast/Statement.h>
#include <ast/Declaration.h>
#include <IO.h>

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
        Utf8String source = readFile(in);

        scanner.reset(new Scanner(fileName, source));
        currentToken.reset(new Token(scanner->nextToken()));

        auto startPosition = currentToken->position;
        std::unique_ptr<Module> module = std::make_unique<ast::Module>(startPosition, fileName);

        while (currentToken->type != TokenType::END_FILE && currentToken->type != TokenType::BAD)
        {
            //std::cout << "Parsing statement " << statements.size() << std::endl;
            std::unique_ptr<Statement> statement = parseStatement();

            //	std::cout << "Adding statement " << typeid(*statement).name() << std::endl;
            module->addStatement(std::move(statement));
        }

        //std::cout << "Statements :" << statements.size() << std::endl;;
        return module;

    }

    std::unique_ptr<Statement> Parser::parseStatement()
    {
        switch (currentToken->type)
        {
            case TokenType::KW_BREAK:
                return parseBreakStatement();
            case TokenType::KW_CONTINUE:
                return parseContinueStatement();
            case TokenType::KW_DO:
                return parseDoWhileStatement();
            case TokenType::KW_EXTERN:
            case TokenType::KW_FUNC:
                return parseFunctionDeclaration();
            case TokenType::KW_IF:
                return parseIfStatement();
            case TokenType::KW_RETURN:
                return parseReturnStatement();
            case TokenType::KW_WHILE:
                return parseWhileStatement();
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
                else
                {
                    bool hasArrayRef = false;
                    if(next.type == TokenType::LSQUARE_BRACKET) {
                        do {
                            next = scanner->nextToken();
                        } while(next.type != TokenType::RSQUARE_BRACKET);
                        next = scanner->nextToken();
                        hasArrayRef = true;
                    }

                    if (next.type >= TokenType::OP_ASSIGN && next.type <= TokenType::OP_ASSIGN_SUB) {
                        rewind();
                        if (hasArrayRef) {
                            return parseArrayIndexAssignment();
                        } else {
                            return parseAssignment();
                        }
                    } else
                        rewind();
                }
            }
            default:
                // if none of the above, parse as expression
                return parseExpressionStatement();
        }
    }

    std::unique_ptr<ast::Statement> Parser::parseArrayIndexAssignment()
    {
        SourcePosition start = currentToken->position;
        std::unique_ptr<Expression> arrayRef = parseValue();
        TokenType opType = currentToken->type;
        accept();

        std::unique_ptr<Expression> expression = parseExpression();

        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<ArrayIndexAssignmentStatement>(start, std::move(arrayRef), opType, std::move(expression));
    }

    std::unique_ptr<ast::Statement> Parser::parseAssignment()
    {
        SourcePosition start = currentToken->position;
        Utf8String variableName = currentToken->str;
        expect(TokenType::IDENTIFIER);
        TokenType opType = currentToken->type;
        accept();

        std::unique_ptr<Expression> expression = parseExpression();

        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<AssignmentStatement>(start, variableName, opType, std::move(expression));
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

    std::unique_ptr<ast::BreakStatement> Parser::parseBreakStatement()
    {
        SourcePosition start = currentToken->position;
        expect(TokenType::KW_BREAK);

        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<ast::BreakStatement>(start);
    }

    std::unique_ptr<ast::ContinueStatement> Parser::parseContinueStatement()
    {
        SourcePosition start = currentToken->position;
        expect(TokenType::KW_CONTINUE);

        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<ast::ContinueStatement>(start);
    }


    std::unique_ptr<ast::DoWhileStatement> Parser::parseDoWhileStatement()
    {
        auto startPos = currentToken->position;
        expect(TokenType::KW_DO);
        std::unique_ptr<Statement> body = parseStatement();
        expect(TokenType::KW_WHILE);
        expect(TokenType::LPAREN);
        std::unique_ptr<Expression> condition = parseExpression();
        expect(TokenType::RPAREN);
        return std::make_unique<DoWhileStatement>(startPos, std::move(condition), std::move(body));
    }

    std::unique_ptr<ast::ExpressionStatement> Parser::parseExpressionStatement()
    {
        auto startPos = currentToken->position;
        std::unique_ptr<Expression> expr = parseExpression();

        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<ExpressionStatement>(startPos, std::move(expr));
    }

    std::unique_ptr<ast::Statement> Parser::parseFunctionDeclaration()
    {
        auto startPos = currentToken->position;
        std::unique_ptr<ast::FunctionPrototype> prototype = parseFunctionPrototype();
        if(accept(TokenType::OP_END_STATEMENT))
        {
            return std::make_unique<FunctionDeclaration>(startPos, std::move(prototype));
        }
        else
        {
            std::unique_ptr<ast::BlockStatement> block = parseBlockStatement();
            return std::make_unique<FunctionDefinition>(startPos, std::move(prototype), std::move(block));
        }

    }

    std::unique_ptr<ast::FunctionPrototype> Parser::parseFunctionPrototype()
    {
        bool isExtern = accept(TokenType::KW_EXTERN);
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
        return std::make_unique<FunctionPrototype>(functionName, returnType, arguments, isExtern);
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

    std::unique_ptr<ast::WhileStatement> Parser::parseWhileStatement()
    {
        auto startPos = currentToken->position;
        expect(TokenType::KW_WHILE);
        expect(TokenType::LPAREN);
        std::unique_ptr<Expression> condition = parseExpression();
        expect(TokenType::RPAREN);
        std::unique_ptr<Statement> body = parseStatement();
        return std::make_unique<WhileStatement>(startPos, std::move(condition), std::move(body));
    }

    std::unique_ptr<VariableDeclaration> Parser::parseVariableDeclaration()
    {
        SourcePosition start = currentToken->position;
        int64_t *arraySize = nullptr;
        std::unique_ptr<Expression> initializer = nullptr;
        Utf8String variableName = currentToken->str;
        expect(TokenType::IDENTIFIER);
        expect(TokenType::OP_COLON);
        Utf8String typeName = currentToken->str;
        expect(TokenType::IDENTIFIER);
        if (accept(TokenType::LSQUARE_BRACKET))
        {
            if(currentToken->type == TokenType::INTEGER) {
                std::string tokenString = currentToken->str.toString();
                arraySize = new int64_t(std::stoll(tokenString, nullptr, currentToken->integerBase));
                accept();
            }
            expect(TokenType::RSQUARE_BRACKET);
        }
        if (accept(TokenType::OP_ASSIGN))
        {
            initializer = parseExpression();
        }
        expect(TokenType::OP_END_STATEMENT);
        return std::make_unique<VariableDeclaration>(start, typeName, variableName, std::move(initializer), arraySize);
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
                expr.reset(new BinaryOpExpression{ start, op, opType, std::move(expr), std::move(right) });
            }
        }
        return expr;
    }

    std::unique_ptr<Expression> Parser::parseUnary()
    {
        SourcePosition start = currentToken->position;
        std::unique_ptr<Expression> result, right;

        TokenType opType = currentToken->type;
        switch (opType)
        {
            case TokenType::OP_ADD:
                accept();
                result = parseUnary();
                break;
            case TokenType::OP_SUB:
                accept();
                right = parseUnary();
                result = std::make_unique<UnaryOpExpression>(start, UnaryOperator::NEG, opType, std::move(right));
                break;
            case TokenType::OP_NOT:
                accept();
                right = parseUnary();
                result = std::make_unique<UnaryOpExpression>(start, UnaryOperator::NOT, opType, std::move(right));
            case TokenType::OP_COMPL:
                accept();
                right = parseUnary();
                result = std::make_unique<UnaryOpExpression>(start, UnaryOperator::CMPL, opType, std::move(right));
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
                Utf8String identifier = currentToken->str;
                Token &nextToken = scanner->nextToken();
                    switch (nextToken.type) {
                        case TokenType::LPAREN: {
                            accept();
                            std::vector<std::unique_ptr<Expression>> arguments;
                            if (currentToken->type != TokenType::RPAREN) {
                                do {
                                    std::unique_ptr<Expression> argument = parseExpression();
                                    arguments.push_back(std::move(argument));
                                } while (accept(TokenType::OP_COMMA));
                            }
                            expect(TokenType::RPAREN);
                            value.reset(new FunctionCallExpression{start, identifier, std::move(arguments)});
                            return value;
                        }
                        case TokenType::LSQUARE_BRACKET: {
                            accept();

                            std::unique_ptr<Expression> array(new VariableExpression{ start, identifier });
                            std::unique_ptr<Expression> index = parseExpression();

                            expect(TokenType::RSQUARE_BRACKET);

                            value.reset(new ArrayIndexReference{start, std::move(array), std::move(index) });
                            return value;
                        }
                        default:
                            rewind();
                            value.reset(new VariableExpression{ start, identifier });

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
            case TokenType::LSQUARE_BRACKET: {
                std::unique_ptr<ArrayLiteral> elements{ new ArrayLiteral{start}};
                accept(TokenType::LSQUARE_BRACKET);
                while (currentToken->type != TokenType::RSQUARE_BRACKET) {
                    auto element = parseExpression();
                    elements->addValue( std::move(element) );
                    accept(TokenType::OP_COMMA);
                }
                value = std::move(elements);
                break;

            }
            default:
                Utf8String errorMesage = Utf8String{ "Unknown value: " } + Token::getTokenName(currentToken->type);
                compilerError(currentToken->position, errorMesage);
                break;
        }

        accept();
        return value;
    }
}
