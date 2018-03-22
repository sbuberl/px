
#ifndef VISITOR_H_
#define VISITOR_H_

#include <ast/AST.h>
#include <ast/Expression.h>
#include <ast/Literal.h>
#include <ast/Statement.h>
#include <ast/Declaration.h>

namespace px
{
    namespace ast
    {
        class Visitor
        {
        public:
            virtual ~Visitor() = default;
            virtual void *visit(AssignmentExpression &a) = 0;
            virtual void *visit(BoolLiteral &b) = 0;
            virtual void *visit(BinaryOpExpression &f) = 0;
            virtual void *visit(BlockStatement &s) = 0;
            virtual void *visit(CastExpression &f) = 0;
            virtual void *visit(CharLiteral &c) = 0;
            virtual void *visit(DeclarationStatement &s) = 0;
            virtual void *visit(ExpressionStatement &s) = 0;
            virtual void *visit(FloatLiteral &i) = 0;
            virtual void *visit(FunctionDeclaration &f) = 0;
            virtual void *visit(IntegerLiteral &i) = 0;
            virtual void *visit(ReturnStatement &s) = 0;
            virtual void *visit(StringLiteral &s) = 0;
            virtual void *visit(TernaryOpExpression &t) = 0;
            virtual void *visit(UnaryOpExpression &u) = 0;
            virtual void *visit(VariableExpression &v) = 0;
        };
    }
}

#endif
