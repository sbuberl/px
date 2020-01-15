
#include <ast/Visitor.h>
#include <ast/Literal.h>

namespace px
{
    namespace ast
    {
        void *ArrayLiteral::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *BoolLiteral::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *CharLiteral::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *IntegerLiteral::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *FloatLiteral::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *StringLiteral::accept(Visitor &visitor) { return visitor.visit(*this); }
    }
}

