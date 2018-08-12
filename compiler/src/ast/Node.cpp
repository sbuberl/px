#include <ast/AST.h>
#include <ast/Visitor.h>

namespace px
{
    namespace ast
    {
        void *Module::accept(Visitor &visitor) { return visitor.visit(*this); }
    }
}
