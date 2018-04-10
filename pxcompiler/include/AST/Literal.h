
#ifndef LITERAL_H_
#define LITERAL_H_

#include <ast/Expression.h>
#include <Symbol.h>
#include <Utf8String.h>
#include <stdlib.h>

namespace px
{
    namespace ast
    {
        class Literal : public Expression
        {
        public:
            const Utf8String literal;

            virtual ~Literal() = default;

        protected:
            Literal(Type *type, const Utf8String &l) : Expression(type), literal(l) {}
        };

        class BoolLiteral : public Literal
        {
        public:
            const bool value;

            BoolLiteral(const Utf8String &l)
                : Literal(Type::BOOL, l), value(l == Utf8String("true"))
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class CharLiteral : public Literal
        {
        public:
            CharLiteral(const Utf8String &l)
                : Literal(Type::CHAR, l)
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class IntegerLiteral : public Literal
        {
        public:
            const int64_t value;

            IntegerLiteral(Type *type, const Utf8String &l, int64_t v)
                : Literal(type != nullptr ? type : Type::INT32, l), value(v)
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class FloatLiteral : public Literal
        {
        public:
            const double value;

            FloatLiteral(Type *type, const Utf8String &l)
                : Literal(type != nullptr ? type : Type::FLOAT32, l), value(std::stod(l.toString()))
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class StringLiteral : public Literal
        {
        public:
            StringLiteral(const Utf8String &l) : Literal(Type::STRING, l)
            {

            }

            void *accept(Visitor &visitor) override;
        };
    }
}

#endif
