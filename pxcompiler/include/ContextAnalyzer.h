
#ifndef CONTEXTANALYZER_H_
#define CONTEXTANALYZER_H_

#include <ast/Visitor.h>
#include <Error.h>
#include <Symbol.h>

namespace px {

    class ContextAnalyzer : public ast::Visitor
    {
    public:
        ContextAnalyzer(SymbolTable *globals, ErrorLog *errors);

        void analyze(ast::AST *ast);
        void *visit(ast::AssignmentExpression &a) override;
        void *visit(ast::BinaryOpExpression &e) override;
        void *visit(ast::BoolLiteral &b) override;
        void *visit(ast::BlockStatement &s) override;
        void *visit(ast::CharLiteral &c) override;
        void *visit(ast::CastExpression &f) override;
        void *visit(ast::DeclarationStatement &d) override;
        void *visit(ast::ExpressionStatement &s) override;
        void *visit(ast::FloatLiteral &f) override;
        void *visit(ast::FunctionDeclaration &f) override;
        void *visit(ast::IntegerLiteral &i) override;
        void *visit(ast::ReturnStatement &s) override;
        void *visit(ast::StringLiteral &s) override;
        void *visit(ast::TernaryOpExpression &t) override;
        void *visit(ast::UnaryOpExpression &e) override;
        void *visit(ast::VariableExpression &v) override;

    private:
        SymbolTable *_globals;
        SymbolTable *_currentScope;
        ErrorLog *errors;
    };

}

#endif
