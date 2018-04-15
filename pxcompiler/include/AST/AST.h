
#ifndef AST_H_
#define AST_H_

#include "SourcePosition.h"

#include <string>

namespace px
{
    namespace ast
    {
        class Visitor;

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
    }
}

#endif
