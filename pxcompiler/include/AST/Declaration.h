
#ifndef DECLARATION_H_
#define DECLARATION_H_

#include <ast/AST.h>
#include <ast/Statement.h>

namespace px
{
    namespace ast
    {
        class FunctionDeclaration : public AST
        {
        public:
            const Utf8String name;
            const Utf8String returnTypeName;
            std::unique_ptr<BlockStatement> block;
            Function *function;

            FunctionDeclaration(const SourcePosition &pos, const Utf8String &fname, const Utf8String &retTypeName, std::unique_ptr<BlockStatement> stmts)
                : AST{ pos }, name{ fname }, returnTypeName{ retTypeName }, block{ std::move(stmts) }, function{ nullptr }
            {
            }

            void *accept(Visitor &visitor) override;
        };
    }
}

#endif
