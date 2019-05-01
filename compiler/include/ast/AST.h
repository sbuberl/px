
#ifndef _PX_AST_AST_H_
#define _PX_AST_AST_H_

#include "SourcePosition.h"

#include <memory>

namespace px
{
    namespace ast
    {
        class Visitor;
        class Statement;

        class AST
        {
        public:
            const SourcePosition position;

            AST(const SourcePosition &pos) : position{ pos }
            {
            }
            virtual ~AST() = default;
            virtual void *accept(Visitor &visitor) = 0;
        };

        class Module : public AST
        {
        public:
            const Utf8String &fileName;
            std::vector<std::unique_ptr<Statement>> statements;

            Module(const SourcePosition &pos, const Utf8String &file)
                : AST{ pos }, fileName{ fileName }
            {
            }

            void *accept(Visitor &visitor) override;

            void addStatement(std::unique_ptr<Statement> statement)
            {
                statements.push_back(std::move(statement));
            }

        };
    }
}

#endif
