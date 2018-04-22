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
        protected:
            using AST::AST;
        };

        class AssignmentStatement : public Statement
        {
        public:
            const Utf8String variableName;
            std::unique_ptr<Expression> expression;

            AssignmentStatement(const SourcePosition &pos, const Utf8String &n, std::unique_ptr<Expression> e)
                : Statement{ pos }, variableName{ n }, expression{ std::move(e) }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class BlockStatement : public Statement
        {
        public:
            using Statement::Statement;

            std::vector<std::unique_ptr<Statement>> statements;

            void addStatement(std::unique_ptr<Statement> statement)
            {
                statements.push_back(std::move(statement));
            }

            void *accept(Visitor &visitor) override;
        };

        class ExpressionStatement : public Statement
        {
        public:
            std::unique_ptr<Expression> expression;

            ExpressionStatement(const SourcePosition &pos, std::unique_ptr<Expression> expr)
                : Statement{ pos }, expression{ std::move(expr) }
            {
            }

            void *accept(Visitor &visitor) override;

        };

        class ReturnStatement : public Statement
        {
        public:
            std::unique_ptr<Expression> returnValue;

            ReturnStatement(const SourcePosition &pos) : ReturnStatement{ pos, nullptr } {}

            ReturnStatement(const SourcePosition &pos, std::unique_ptr<Expression> value)
                : Statement{ pos }, returnValue{ std::move(value) }
            {
            }

            void *accept(Visitor &visitor) override;

        };

    }
}

#endif

