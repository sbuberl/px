
#include "ContextAnalyzer.h"

#include <iostream>
#include <functional>

namespace px
{
	ContextAnalyzer::ContextAnalyzer(SymbolTable *globals)
		: _globals(globals), _currentScope(globals)
	{

	}

	void ContextAnalyzer::analyze(ast::AST* ast)
	{
		ast->accept(*this);
	}

	void* ContextAnalyzer::visit(ast::AssignmentExpression &a)
	{
		Variable *variable = _currentScope->getVariable(a.variableName);
		a.expression->accept(*this);
		return nullptr;
	}

	void* ContextAnalyzer::visit(ast::BinaryOpExpression &b)
	{
		b.left->accept(*this);
		b.right->accept(*this);

		Type *leftType = b.left->type;
		Type *rightType = b.right->type;

		unsigned int combinedFlags = leftType->flags & leftType->flags;

		if(combinedFlags & Type::BUILTIN)
		{
			if(leftType == rightType)
			{
				b.type = leftType;
			}
			else if(leftType->isBuiltinFloat())
			{
				b.type = leftType;
				b.right = std::make_unique<ast::CastExpression>(leftType, std::move(b.right));
			}
			else if(rightType->isBuiltinFloat())
			{
				b.type = rightType;
				b.left = std::make_unique<ast::CastExpression>(rightType, std::move(b.left));
			}
		}

		return nullptr;
	}

	void* ContextAnalyzer::visit(ast::BlockStatement &s)
	{
        for(auto &statement : s.statements)
        {
            statement->accept(*this);
        }
		return nullptr;
	}

	void* ContextAnalyzer::visit(ast::CastExpression &c)
	{
		c.expression->accept(*this);

		Type *originalType = c.expression->type;
		Type *castTo = c.type;

		unsigned int combinedFlags = originalType->flags & castTo->flags;

		if (combinedFlags & Type::BUILTIN)
		{
			
		}

		return nullptr;
	}

	void* ContextAnalyzer::visit(ast::DeclarationStatement &d)
	{
		Type *type = _currentScope->getType(d.typeName);
        _currentScope->addSymbol(new Variable{d.name, type});
		if (d.initialValue)
		{
			d.initialValue->accept(*this);
		}
		return nullptr;
	}

	void* ContextAnalyzer::visit(ast::ExpressionStatement &s)
	{
		s.expression->accept(*this);
		return nullptr;
	}

	void* ContextAnalyzer::visit(ast::FloatLiteral &f)
	{
		return nullptr;
	}

	void* ContextAnalyzer::visit(ast::FunctionDeclaration &f)
	{
		SymbolTable *oldScope = _currentScope;
		Type *returnType = _currentScope->getType(f.returnTypeName);
		Function *function = new Function(f.name, returnType, _currentScope);
		f.function = function;
		_currentScope->addSymbol(function);
		_currentScope = &function->symbols;
		f.block->accept(*this);
		_currentScope = oldScope;
		return nullptr;
	}

	void* ContextAnalyzer::visit(ast::IntegerLiteral &i)
	{
		return nullptr;
	}

	void* ContextAnalyzer::visit(ast::ReturnStatement &s)
	{
		if(s.returnValue != nullptr)
			s.returnValue->accept(*this);
		return nullptr;
	}

	void* ContextAnalyzer::visit(ast::StringLiteral &s)
	{
		return nullptr;
	}

	void* ContextAnalyzer::visit(ast::UnaryOpExpression &e)
	{
		e.expression->accept(*this);
		e.type = e.expression->type;
		return nullptr;
	}

	void* ContextAnalyzer::visit(ast::VariableExpression &v)
	{
		Variable *variable = _currentScope->getVariable(v.variable);
		v.type = variable->type;
		return nullptr;
	}
}
