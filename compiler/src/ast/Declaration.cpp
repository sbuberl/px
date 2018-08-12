
#include <ast/Declaration.h>
#include <ast/Visitor.h>

namespace px
{
    namespace ast
    {
        void *ExternFunctionDeclaration::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *FunctionDeclaration::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *VariableDeclaration::accept(Visitor &visitor) { return visitor.visit(*this); }
    }
}
