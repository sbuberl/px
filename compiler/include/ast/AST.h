
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

        enum class NodeType
        {
            UNKNOWN,
            DECLARE_VAR,
            DECLARE_FUNC,
            DECLARE_FUNC_BODY,
            EXP_ARRAY_ACCESS,
            EXP_CAST,
            EXP_FUNC_CALL,
            EXP_BINARY_OP,
            EXP_TERNARY_OP,
            EXP_UNARY_OP,
            EXP_VAR_LOAD,
            LITERAL_ARRAY,
            LITERAL_BOOL,
            LITERAL_CHAR,
            LITERAL_FLOAT,
            LITERAL_INT,
            LITERAL_STRING,
            MODULE,
            STMT_ARRAY_INDEX_ASSIGN,
            STMT_ASSIGN,
            STMT_BLOCK,
            STMT_BREAK,
            STMT_CONTINUE,
            STMT_DO_WHILE,
            STMT_EXP,
            STMT_IF,
            STMT_RETURN,
            STMT_WHILE
        };
        class AST
        {
        public:
            const NodeType nodeType;
            const SourcePosition position;

            AST(NodeType type, const SourcePosition &pos) : nodeType{type}, position{ pos }
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
                : AST{ NodeType::MODULE, pos }, fileName{ file }
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
