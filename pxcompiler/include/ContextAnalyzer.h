
#ifndef CONTEXTANALYZER_H_
#define CONTEXTANALYZER_H_

#include <ast/Visitor.h>
#include <Error.h>
#include <Symbol.h>

namespace px {

    class ContextAnalyzer : public ast::Visitor
    {
    public:
        ContextAnalyzer(Scope *rootScope, ErrorLog *errors);

        void analyze(ast::AST *ast);
        void *visit(ast::AssignmentStatement &a) override;
        void *visit(ast::BinaryOpExpression &e) override;
        void *visit(ast::BoolLiteral &b) override;
        void *visit(ast::BlockStatement &s) override;
        void *visit(ast::CharLiteral &c) override;
        void *visit(ast::CastExpression &f) override;
        void *visit(ast::ExpressionStatement &s) override;
        void *visit(ast::FloatLiteral &f) override;
        void *visit(ast::FunctionDeclaration &f) override;
        void *visit(ast::IfStatement &i) override;
        void *visit(ast::IntegerLiteral &i) override;
        void *visit(ast::ReturnStatement &s) override;
        void *visit(ast::StringLiteral &s) override;
        void *visit(ast::TernaryOpExpression &t) override;
        void *visit(ast::UnaryOpExpression &e) override;
        void *visit(ast::VariableDeclaration &d) override;
        void *visit(ast::VariableExpression &v) override;

    private:
        void checkAssignmentTypes(Variable * variable, std::unique_ptr<ast::Expression>& expression, const SourcePosition & start);

        Scope *_currentScope;
        ErrorLog * const errors;
    };

}

#endif
