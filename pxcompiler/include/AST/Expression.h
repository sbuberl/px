
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
            Expression(const SourcePosition &pos, Type *type) : AST{ pos }, type{ type }
            {
            }

            Expression(const SourcePosition &pos) : Expression{ pos , Type::UNKNOWN }
            {
            }
        };

        enum class BinaryOperator
        {
            BAD,
            OR,
            AND,
            LT, LTE, GT, GTE, EQ, NE,
            BIT_OR,
            BIT_XOR,
            BIT_AND,
            LSH, RSH,
            ADD, SUB,
            MUL, DIV, MOD,
            EXP
        };

        class BinaryOpExpression : public Expression
        {
        public:
            std::unique_ptr<Expression> left;
            std::unique_ptr<Expression> right;
            BinaryOperator op;

            BinaryOpExpression(const SourcePosition &pos, BinaryOperator op, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
                : Expression{ pos }, op{ op }, left{ std::move(l) }, right{ std::move(r) }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class CastExpression : public Expression
        {
        public:
            std::unique_ptr<Expression> expression;
            const Utf8String newTypeName;

            CastExpression(const SourcePosition &pos, const Utf8String &type, std::unique_ptr<Expression> exp)
                : Expression{ pos }, newTypeName{ type }, expression{ std::move(exp) }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class FunctionCallExpression : public Expression
        {
        public:
            const Utf8String functionName;
            std::vector<std::unique_ptr<Expression>> arguments;
            Function *function;

            FunctionCallExpression(const SourcePosition &pos, const Utf8String &name, std::vector<std::unique_ptr<Expression>> args)
                : Expression{ pos }, functionName{ name }, arguments{ std::move(args) }, function{}
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

            UnaryOpExpression(const SourcePosition &pos, UnaryOperator op, std::unique_ptr<Expression> e)
                : Expression{ pos }, expression{ std::move(e) }, op{ op }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class TernaryOpExpression : public Expression
        {
        public:
            std::unique_ptr<Expression> condition;
            std::unique_ptr<Expression> trueExpr;
            std::unique_ptr<Expression> falseExpr;

            TernaryOpExpression(const SourcePosition &pos, std::unique_ptr<Expression> c, std::unique_ptr<Expression> t, std::unique_ptr<Expression> f)
                : Expression{ pos }, condition{ std::move(c) }, trueExpr{ std::move(t) }, falseExpr{ std::move(f) }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class VariableExpression : public Expression
        {
        protected:

        public:
            const Utf8String variable;

            VariableExpression(const SourcePosition &pos, const Utf8String &var) : Expression{ pos }, variable{ var } {}

            ~VariableExpression() {}

            void *accept(Visitor &visitor) override;

        };
    }
}

#endif
