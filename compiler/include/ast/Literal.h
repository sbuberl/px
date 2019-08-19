
#ifndef _PX_AST_LITERAL_H_
#define _PX_AST_LITERAL_H_

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
            Literal(NodeType nodeType, const SourcePosition &pos, Type *type, const Utf8String &l) : Expression{ nodeType, pos, type }, literal{ l } {}
        };

        class BoolLiteral : public Literal
        {
        public:
            const bool value;

            BoolLiteral(const SourcePosition &pos, const Utf8String &l)
                : Literal{ NodeType::LITERAL_BOOL, pos, Type::BOOL, l }, value{ l == Utf8String("true") }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class CharLiteral : public Literal
        {
        public:
            CharLiteral(const SourcePosition &pos, const Utf8String &l)
                : Literal{ NodeType::LITERAL_CHAR, pos, Type::CHAR, l }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class IntegerLiteral : public Literal
        {
        public:
            const int64_t value;

            IntegerLiteral(const SourcePosition &pos, Type *type, const Utf8String &l, int64_t v)
                : Literal{ NodeType::LITERAL_INT, pos, type != nullptr ? type : Type::INT32, l }, value{ v }

            {
            }

            void *accept(Visitor &visitor) override;
        };

        class FloatLiteral : public Literal
        {
        public:
            const double value;

            FloatLiteral(const SourcePosition &pos, Type *type, const Utf8String &l)
                : Literal{ NodeType::LITERAL_FLOAT, pos, type != nullptr ? type : Type::FLOAT32, l }, value{ std::stod(l.toString()) }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class StringLiteral : public Literal
        {
        public:
            StringLiteral(const SourcePosition &pos, const Utf8String &l) : Literal{ NodeType::LITERAL_STRING, pos, Type::STRING, l }
            {

            }

            void *accept(Visitor &visitor) override;
        };
    }
}

#endif
