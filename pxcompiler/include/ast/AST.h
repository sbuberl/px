
#ifndef AST_H_
#define AST_H_

#include "SourcePosition.h"

#include <memory>

namespace px
{
    namespace ast
    {
        class Visitor;
        class BlockStatement;

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
            std::unique_ptr<BlockStatement> block;

            Module(const SourcePosition &pos, const Utf8String &file, std::unique_ptr<BlockStatement> b)
                : AST{ pos }, fileName{ fileName }, block{ std::move(b) }
            {
            }

            void *accept(Visitor &visitor) override;
        };
    }
}

#endif
