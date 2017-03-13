/*
 * Parser.cpp
 *
 *  Created on: Dec 13, 2009
 *      Author: steve
 */

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
		if(scanner->accept(type))
		{
			currentToken = scanner->nextToken();
			return true;
		}
		return false;
	}

	bool Parser::accept(std::string &token)
	{
		if(scanner->accept(token))
		{
			currentToken = scanner->nextToken();
			return true;
		}
		return false;
	}

	void Parser::expect(TokenType type)
	{
		if(scanner->accept(type))
		{
			currentToken = scanner->nextToken();
		}
		// TODO: raise error
	}

	void Parser::expect(std::string &token)
	{
		if(scanner->accept(token))
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
		std::string source(buffer.str());

		scanner = new Scanner(source);
		currentToken = scanner->nextToken();

        std::unique_ptr<BlockStatement> block(new BlockStatement());

		while(currentToken.type != TokenType::END_FILE && currentToken.type != TokenType::BAD)
		{
			//std::cout << "Parsing statement " << statements.size() << std::endl;
			std::unique_ptr<Statement> statement = parseStatement();
			if(statement != nullptr)
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
		if(currentToken.type == TokenType::KW_RETURN)
			return parseReturnStatement();
		else if(currentToken.type == TokenType::IDENTIFIER)
		{
			Token& next = scanner->nextToken();
			if(next.type == TokenType::IDENTIFIER)
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
		if(expr == nullptr)
			return nullptr;

		expect(TokenType::OP_END_STATEMENT);
		return std::make_unique<ExpressionStatement>(std::move(expr));
	}

	std::unique_ptr<Statement> Parser::parseReturnStatement()
	{
		accept();
		std::unique_ptr<Expression> retValue = nullptr;
		if(currentToken.type != TokenType::OP_END_STATEMENT)
		{
			retValue = parseExpression();
			if(retValue == nullptr)
				return nullptr;
		}

		accept(TokenType::OP_END_STATEMENT);
        return std::unique_ptr<ReturnStatement>{new ReturnStatement{std::move(retValue)}};
	}

	std::unique_ptr<Statement> Parser::parseVariableDeclaration()
	{
		std::unique_ptr<Expression> initializer = nullptr;
		string typeName = currentToken.str;
		accept();
		string variableName = currentToken.str;
		expect(TokenType::IDENTIFIER);
		if(accept(TokenType::OP_ASSIGN))
		{
			initializer = parseExpression();
		}
		expect(TokenType::OP_END_STATEMENT);
		return std::make_unique<DeclarationStatement>(typeName, variableName, std::move(initializer));
	}

	std::unique_ptr<Expression> Parser::parseExpression()
	{
		if(currentToken.type == TokenType::IDENTIFIER)
		{
			Token next = scanner->nextToken();
			if (next.type == TokenType::OP_ASSIGN)
			{
				rewind();
				return parseAssignment();
			}
		}

		rewind();
		return parseBitwiseOr();
	}

	std::unique_ptr<ast::Expression> Parser::parseAssignment()
	{
		expect(TokenType::IDENTIFIER);

		string variableName = currentToken.str;

		expect(TokenType::OP_ASSIGN);

		std::unique_ptr<Expression> expression = parseExpression();

		expect(TokenType::OP_END_STATEMENT);
		return std::make_unique<AssignmentExpression>(variableName, std::move(expression));
	}

	std::unique_ptr<Expression> Parser::parseOr()
	{
		std::unique_ptr<Expression> left = parseAnd();
		while (currentToken.type == TokenType::OP_OR)
		{
			accept();
			std::unique_ptr<Expression> right = parseAnd();
			left.reset(new BinaryOpExpression{ BinaryOperator::OR, std::move(left), std::move(right) });
		}
		return left;
	}

	std::unique_ptr<Expression> Parser::parseAnd()
	{
		std::unique_ptr<Expression> left = parseConditionals();
		while (currentToken.type == TokenType::OP_AND)
		{
			accept();
			std::unique_ptr<Expression> right = parseConditionals();
			left.reset(new BinaryOpExpression{ BinaryOperator::AND, std::move(left), std::move(right) });
		}
		return left;
	}

	std::unique_ptr<Expression> Parser::parseConditionals()
	{
		std::unique_ptr<Expression> left = parseBitwiseOr();
		TokenType token = currentToken.type;
		while (token == TokenType::OP_EQUALS || token == TokenType::OP_NOT_EQUAL || token == TokenType::OP_LESS || token == TokenType::OP_LESS_OR_EQUAL
			|| token == TokenType::OP_GREATER || token == TokenType::OP_GREATER_OR_EQUAL)
		{
			accept();
			std::unique_ptr<Expression> right = parseBitwiseOr();
			BinaryOperator op;
			switch (token)
			{
			case TokenType::OP_EQUALS:
				op = BinaryOperator::EQ;
				break;
			case TokenType::OP_NOT_EQUAL:
				op = BinaryOperator::NE;
				break;
			case TokenType::OP_LESS:
				op = BinaryOperator::LT;
				break;
			case TokenType::OP_LESS_OR_EQUAL:
				op = BinaryOperator::LTE;
				break;
			case TokenType::OP_GREATER:
				op = BinaryOperator::GT;
				break;
			default:
				op = BinaryOperator::GTE;
				break;
			}
			left.reset(new BinaryOpExpression{ op, std::move(left), std::move(right) });
			token = currentToken.type;
		}
		return left;
	}

	std::unique_ptr<Expression> Parser::parseBitwiseOr()
	{
		std::unique_ptr<Expression> left = parseBitwiseXor();
		while (currentToken.type == TokenType::OP_BIT_OR)
		{
			accept();
			std::unique_ptr<Expression> right = parseBitwiseXor();
			left.reset(new BinaryOpExpression{ BinaryOperator::BIT_OR, std::move(left), std::move(right) });
		}
		return left;
	}

	std::unique_ptr<Expression> Parser::parseBitwiseXor()
	{
		std::unique_ptr<Expression> left = parseBitwiseAnd();
		while (currentToken.type == TokenType::OP_BIT_XOR)
		{
			accept();
			std::unique_ptr<Expression> right = parseBitwiseAnd();
			left.reset(new BinaryOpExpression{ BinaryOperator::BIT_XOR, std::move(left), std::move(right) });
		}
		return left;
	}

	std::unique_ptr<Expression> Parser::parseBitwiseAnd()
	{
		std::unique_ptr<Expression> left = parseShift();
		while (currentToken.type == TokenType::OP_BIT_AND)
		{
			accept();
			std::unique_ptr<Expression> right = parseShift();
			left.reset(new BinaryOpExpression{ BinaryOperator::BIT_AND, std::move(left), std::move(right) });
		}
		return left;
	}

	std::unique_ptr<Expression> Parser::parseShift()
	{
		std::unique_ptr<Expression> left = parseAddSub();
		TokenType token = currentToken.type;
		while (token == TokenType::OP_LEFT_SHIFT || token == TokenType::OP_RIGHT_SHIFT)
		{
			accept();
			std::unique_ptr<Expression> right = parseAddSub();
			BinaryOperator op = (token == TokenType::OP_LEFT_SHIFT) ? BinaryOperator::LSH : BinaryOperator::RSH;
			left.reset(new BinaryOpExpression{ op, std::move(left), std::move(right) });
			token = currentToken.type;
		}
		return left;
	}

	std::unique_ptr<Expression> Parser::parseAddSub()
	{
		std::unique_ptr<Expression> left = parseMultDiv();
		TokenType token = currentToken.type;
		while(token == TokenType::OP_ADD || token == TokenType::OP_SUB)
		{
			accept();
			std::unique_ptr<Expression> right = parseMultDiv();
            BinaryOperator op = (token == TokenType::OP_ADD) ? BinaryOperator::ADD : BinaryOperator::SUB;
            left.reset(new BinaryOpExpression{op, std::move(left), std::move(right)});
			token = currentToken.type;
		}
		return left;
	}

	std::unique_ptr<Expression> Parser::parseMultDiv()
	{
		std::unique_ptr<Expression> left = parseUnary();
		TokenType token = currentToken.type;
		while(token == TokenType::OP_STAR || token == TokenType::OP_DIV || token == TokenType::OP_MOD)
		{
			accept();
			std::unique_ptr<Expression> right = parseUnary();
			BinaryOperator op;
			switch(token)
			{
			case TokenType::OP_STAR:
                op = BinaryOperator::MUL;
                break;
			case TokenType::OP_DIV:
                op = BinaryOperator::DIV;
                break;
			default:
                op = BinaryOperator::MOD;
                break;
			}
            left.reset(new BinaryOpExpression{op, std::move(left), std::move(right)});
			token = currentToken.type;
		}
		return left;
	}

	std::unique_ptr<Expression> Parser::parseUnary()
	{
		std::unique_ptr<Expression> result, right;

		switch(currentToken.type)
		{
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
		switch(currentToken.type)
		{
		case TokenType::IDENTIFIER:
            value.reset(new VariableExpression{currentToken.str});
			break;
		case TokenType::INTEGER:
            value.reset(new IntegerLiteral{currentToken.str});
			break;
		case TokenType::FLOAT:
            value.reset(new FloatLiteral{currentToken.str});
			break;
		case TokenType::STRING:
            value.reset(new StringLiteral{currentToken.str});
			break;
		default:
			// TODO parse error
			return value;
		}

		accept();
		return value;
	}
}
