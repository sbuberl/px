
#ifndef DECLARATION_H_
#define DECLARATION_H_

#include <ast/AST.h>
#include <ast/Statement.h>

namespace px
{
    namespace ast
    {
        class FunctionDeclaration : public Statement
        {
        public:
            const Utf8String name;
            const Utf8String returnTypeName;
            std::unique_ptr<BlockStatement> block;
            Function *function;

            FunctionDeclaration(const SourcePosition &pos, const Utf8String &fname, const Utf8String &retTypeName, std::unique_ptr<BlockStatement> stmts)
                : Statement{ pos }, name{ fname }, returnTypeName{ retTypeName }, block{ std::move(stmts) }, function{ nullptr }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class VariableDeclaration : public Statement
        {
        public:
            const Utf8String typeName;
            const Utf8String name;
            std::unique_ptr<Expression> initialValue;

            VariableDeclaration(const SourcePosition &pos, const Utf8String &t, const Utf8String &n, std::unique_ptr<Expression> value)
                : Statement{ pos }, typeName{ t }, name{ n }, initialValue{ std::move(value) }
            {
            }

            void *accept(Visitor &visitor) override;
        };
    }
}

#endif
