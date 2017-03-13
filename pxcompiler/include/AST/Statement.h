#ifndef STATEMENT_H_
#define STATEMENT_H_

#include <ast/AST.h>
#include <ast/Expression.h>
#include <Symbol.h>

#include <vector>
#include <algorithm>

namespace px
{
namespace ast
{
	class Statement : public AST
	{

	};

	class BlockStatement : public Statement
	{
	public:
	    std::vector<std::unique_ptr<Statement>> statements;

        void addStatement(std::unique_ptr<Statement> statement)
        {
            statements.push_back(std::move(statement));
        }

		void *accept(Visitor &visitor) override;
	};

	class DeclarationStatement : public Statement
	{
	public:
		const std::string typeName;
		const std::string name;
        std::unique_ptr<Expression> initialValue;

		DeclarationStatement(std::string t, std::string n, std::unique_ptr<Expression> value)
			: typeName(t), name(n), initialValue(std::move(value))
		{
		}

        void *accept(Visitor &visitor) override;
	};

	class ExpressionStatement : public Statement
	{
	public:
        std::unique_ptr<Expression> expression;

		ExpressionStatement(std::unique_ptr<Expression> expr) : expression(std::move(expr)) { }

        void *accept(Visitor &visitor) override;

	};

	class ReturnStatement : public Statement
	{
	public:
        std::unique_ptr<Expression> returnValue;

		ReturnStatement() : returnValue(nullptr) { }

		ReturnStatement(std::unique_ptr<Expression> value) : returnValue(std::move(value)) { }

        void *accept(Visitor &visitor) override;

	};

}
}

#endif

