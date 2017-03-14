
#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <ast/AST.h>
#include <Symbol.h>

namespace px
{
    namespace ast
    {
        class Expression : public AST
        {
        public:
            Type *type;

        protected:
            Expression(Type *type) : type(type)
            {
            }
        };

        enum class BinaryOperator
        {
            OR,
            AND,
            NOT,
            LT, LTE, GT, GTE, EQ, NE,
            BIT_OR,
            BIT_XOR,
            BIT_AND,
            LSH, RSH,
            ADD, SUB,
            MUL, DIV, MOD,
            EXP
        };


        class AssignmentExpression : public Expression
        {
        public:
            const std::string variableName;
            std::unique_ptr<Expression> expression;

            AssignmentExpression(const std::string &n, std::unique_ptr<Expression> e)
                : Expression(nullptr), variableName(n), expression(std::move(e))
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class BinaryOpExpression : public Expression
        {
        public:
            std::unique_ptr<Expression> left;
            std::unique_ptr<Expression> right;
            BinaryOperator op;

            BinaryOpExpression(BinaryOperator op, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
                : Expression(nullptr), op(op), left(std::move(l)), right(std::move(r))
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class CastExpression : public Expression
        {
        public:
            std::unique_ptr<Expression> expression;

            CastExpression(Type *type, std::unique_ptr<Expression> exp)
                : Expression(type), expression(std::move(exp))
            {
            }

            void *accept(Visitor &visitor) override;
        };

        enum class UnaryOperator
        {
            NEG,
            CMPL,
            NOT,
        };

        class UnaryOpExpression : public Expression
        {
        public:
            std::unique_ptr<Expression> expression;
            UnaryOperator op;

            UnaryOpExpression(UnaryOperator op, std::unique_ptr<Expression> e)
                : Expression(nullptr), expression(std::move(e)), op(op)
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class VariableExpression : public Expression
        {
        protected:

        public:
            const std::string variable;

            VariableExpression(const std::string &var) : Expression(nullptr), variable(var) {}

            ~VariableExpression() {}

            void *accept(Visitor &visitor) override;

        };
    }
}

#endif
