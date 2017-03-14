
#ifndef DECLARATION_H_
#define DECLARATION_H_

#include <ast/AST.h>
#include <ast/Statement.h>

#include <string>

namespace px
{
    namespace ast
    {
        class FunctionDeclaration : public AST
        {
        public:
            const std::string name;
            const std::string returnTypeName;
            std::unique_ptr<BlockStatement> block;
            Function *function;

            FunctionDeclaration(const std::string &fname, const std::string &retTypeName, std::unique_ptr<BlockStatement> stmts)
                : name(fname), returnTypeName(retTypeName), block(std::move(stmts)), function(nullptr)
            {
            }

            void *accept(Visitor &visitor) override;
        };
    }
}

#endif
