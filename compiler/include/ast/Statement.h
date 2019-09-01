#ifndef _PX_AST_STATEMENT_H_
#define _PX_AST_STATEMENT_H_

#include <ast/AST.h>
#include <ast/Expression.h>
#include <Symbol.h>

#include <vector>
#include <algorithm>

namespace px
{
    namespace ast
    {
        class Statement : public AST
        {
        protected:
            using AST::AST;
        };

        class AssignmentStatement : public Statement
        {
        public:
            const Utf8String variableName;
            std::unique_ptr<Expression> expression;
            TokenType opType;
            Type *variableType;

            AssignmentStatement(const SourcePosition &pos, const Utf8String &n, TokenType op, std::unique_ptr<Expression> e)
                : Statement{ NodeType::STMT_ASSIGN, pos }, variableName{ n }, opType{ op }, expression{ std::move(e) }, variableType{}
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class BlockStatement : public Statement
        {
        public:
            std::vector<std::unique_ptr<Statement>> statements;

            BlockStatement(const SourcePosition &pos)
                : Statement{ NodeType::STMT_BLOCK, pos }
            {
            }

            void addStatement(std::unique_ptr<Statement> statement)
            {
                statements.push_back(std::move(statement));
            }

            Statement& getLastStatement()
            {
                return *statements.back();
            }

            void *accept(Visitor &visitor) override;
        };

        class BreakStatement : public Statement
        {
        public:
            BreakStatement(const SourcePosition &pos)
                    : Statement{ NodeType::STMT_BREAK, pos }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class ContinueStatement : public Statement
        {
        public:
            ContinueStatement(const SourcePosition &pos)
                    : Statement{ NodeType::STMT_CONTINUE, pos }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class DoWhileStatement : public Statement
        {
        public:
            std::unique_ptr<Expression> condition;
            std::unique_ptr<Statement> body;

            DoWhileStatement(const SourcePosition &pos, std::unique_ptr<Expression> cond, std::unique_ptr<Statement> statement)
                : Statement{ NodeType::STMT_WHILE, pos }, condition{ std::move(cond) }, body{ std::move(statement) }
            {
            }

            void *accept(Visitor &visitor) override;

        };

        class ExpressionStatement : public Statement
        {
        public:
            std::unique_ptr<Expression> expression;

            ExpressionStatement(const SourcePosition &pos, std::unique_ptr<Expression> expr)
                : Statement{ NodeType::STMT_EXP, pos }, expression{ std::move(expr) }
            {
            }

            void *accept(Visitor &visitor) override;

        };

        class IfStatement : public Statement
        {
        public:
            std::unique_ptr<Expression> condition;
            std::unique_ptr<Statement> trueStatement;
            std::unique_ptr<Statement> elseStatement;

            IfStatement(const SourcePosition &pos, std::unique_ptr<Expression> cond, std::unique_ptr<Statement> trueMatch, std::unique_ptr<Statement> elseMatch)
                : Statement{ NodeType::STMT_IF, pos }, condition{ std::move(cond) }, trueStatement{ std::move(trueMatch) }, elseStatement{ std::move(elseMatch) }
            {
            }

            void *accept(Visitor &visitor) override;

        };

        class ReturnStatement : public Statement
        {
        public:
            std::unique_ptr<Expression> returnValue;

            ReturnStatement(const SourcePosition &pos) : ReturnStatement{ pos, nullptr } {}

            ReturnStatement(const SourcePosition &pos, std::unique_ptr<Expression> value)
                : Statement{ NodeType::STMT_RETURN, pos }, returnValue{ std::move(value) }
            {
            }

            void *accept(Visitor &visitor) override;

        };

        class WhileStatement : public Statement
        {
        public:
            std::unique_ptr<Expression> condition;
            std::unique_ptr<Statement> body;

            WhileStatement(const SourcePosition &pos, std::unique_ptr<Expression> cond, std::unique_ptr<Statement> statement)
                    : Statement{ NodeType::STMT_WHILE, pos }, condition{ std::move(cond) }, body{ std::move(statement) }
            {
            }

            void *accept(Visitor &visitor) override;

        };

    }
}

#endif

