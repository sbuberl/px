
#ifndef _PX_AST_EXPRESSION_H_
#define _PX_AST_EXPRESSION_H_

#include <ast/AST.h>
#include <Symbol.h>
#include <Token.h>

namespace px
{
    namespace ast
    {
        class Expression : public AST
        {
        public:
            Type *type;

        protected:
            Expression(NodeType nodeType, const SourcePosition &pos, Type *type) : AST{  nodeType, pos }, type{ type }
            {
            }

            Expression(NodeType nodeType, const SourcePosition &pos) : Expression{ nodeType, pos , Type::UNKNOWN }
            {
            }
        };

        class ArrayIndexReference : public Expression
        {
        public:
            std::unique_ptr<Expression> array;
            std::unique_ptr<Expression> index;

            ArrayIndexReference(const SourcePosition &pos, std::unique_ptr<Expression> arr, std::unique_ptr<Expression> ind)
                    : Expression{ NodeType::EXP_ARRAY_ACCESS, pos }, array{ std::move(arr) }, index{ std::move(ind) }
            {
            }

            void *accept(Visitor &visitor) override;
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
            TokenType token;

            BinaryOpExpression(const SourcePosition &pos, BinaryOperator op, TokenType tokenType, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
                : Expression{ NodeType::EXP_BINARY_OP, pos }, left{ std::move(l) }, right{ std::move(r) }, op{ op }, token {tokenType}
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
                : Expression{ NodeType::EXP_CAST, pos }, expression{ std::move(exp) }, newTypeName{ type }
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
                : Expression{ NodeType::EXP_FUNC_CALL, pos }, functionName{ name }, arguments{ std::move(args) }, function{}
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
            TokenType token;

            UnaryOpExpression(const SourcePosition &pos, UnaryOperator op, TokenType tokenType, std::unique_ptr<Expression> e)
                : Expression{ NodeType::EXP_UNARY_OP, pos }, expression{ std::move(e) }, op{ op }, token{ tokenType }
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
                : Expression{ NodeType::EXP_TERNARY_OP, pos }, condition{ std::move(c) }, trueExpr{ std::move(t) }, falseExpr{ std::move(f) }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class VariableExpression : public Expression
        {
        protected:

        public:
            const Utf8String variable;

            VariableExpression(const SourcePosition &pos, const Utf8String &var) : Expression{ NodeType::EXP_VAR_LOAD, pos }, variable{ var }
            {
            }

            ~VariableExpression() {}

            void *accept(Visitor &visitor) override;

        };
    }
}

#endif
