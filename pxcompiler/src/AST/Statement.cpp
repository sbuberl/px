
#include <ast/Visitor.h>
#include <ast/Statement.h>

namespace px
{
    namespace ast
    {
        void *BlockStatement::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *ExpressionStatement::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *ReturnStatement::accept(Visitor &visitor) { return visitor.visit(*this); }
    }
}
