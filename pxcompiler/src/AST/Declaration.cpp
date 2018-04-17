
#include <ast/AST.h>
#include <ast/Visitor.h>

namespace px
{
    namespace ast
    {
        void *FunctionDeclaration::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *VariableDeclaration::accept(Visitor &visitor) { return visitor.visit(*this); }
    }
}
