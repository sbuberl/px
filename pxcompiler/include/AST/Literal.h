
#ifndef LITERAL_H_
#define LITERAL_H_

#include <ast/Expression.h>
#include <Symbol.h>
#include <stdlib.h>

namespace px
{
    namespace ast
    {
        class Literal : public Expression
        {
        public:
            const std::string literal;

            virtual ~Literal() = default;

        protected:
            Literal(Type *type, const std::string l) : Expression(type), literal(l) {}
        };

        class BoolLiteral : public Literal
        {
        public:
            const bool value;

            BoolLiteral(const std::string &l)
                : Literal(Type::BOOL, l), value(l == "true")
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class IntegerLiteral : public Literal
        {
        public:
            const int32_t value;

            IntegerLiteral(const std::string &l, int32_t v)
                : Literal(Type::INT32, l), value(v)
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class FloatLiteral : public Literal
        {
        public:
            const double value;

            FloatLiteral(const std::string &l)
                : Literal(Type::FLOAT32, l), value(std::stod(l))
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class StringLiteral : public Literal
        {
        public:
            StringLiteral(const std::string l) : Literal(Type::STRING, l)
            {

            }

            void *accept(Visitor &visitor) override;
        };
    }
}

#endif
