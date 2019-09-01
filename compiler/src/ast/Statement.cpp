
#include <ast/Visitor.h>
#include <ast/Statement.h>

namespace px
{
    namespace ast
    {
        void *AssignmentStatement::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *BlockStatement::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *BreakStatement::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *ContinueStatement::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *DoWhileStatement::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *IfStatement::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *ExpressionStatement::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *ReturnStatement::accept(Visitor &visitor) { return visitor.visit(*this); }

        void *WhileStatement::accept(Visitor &visitor) { return visitor.visit(*this); }
    }
}
