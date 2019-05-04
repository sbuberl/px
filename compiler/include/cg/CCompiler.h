
#ifndef _PX_CG_CCOMPILER_H_
#define _PX_CG_CCOMPILER_H_

#include "ast/Visitor.h"
#include "Scope.h"
#include "Utf8String.h"

#include <unordered_map>

namespace px {

    class CCompiler : public ast::Visitor
    {
    public:
        CCompiler(ScopeTree * scopeTree);
        void compile(ast::AST &ast);
        void *visit(ast::AssignmentStatement &a) override;
        void *visit(ast::BinaryOpExpression &e) override;
        void *visit(ast::BoolLiteral &b) override;
        void *visit(ast::BlockStatement &s) override;
        void *visit(ast::CharLiteral &c) override;
        void *visit(ast::CastExpression &f) override;
        void *visit(ast::ExpressionStatement &s) override;
        void *visit(ast::ExternFunctionDeclaration &e) override;
        void *visit(ast::FloatLiteral &f) override;
        void *visit(ast::FunctionCallExpression &f) override;
        void *visit(ast::FunctionDeclaration &f) override;
        void *visit(ast::IfStatement &i) override;
        void *visit(ast::IntegerLiteral &i) override;
        void *visit(ast::Module &m) override;
        void *visit(ast::ReturnStatement &s) override;
        void *visit(ast::StringLiteral &s) override;
        void *visit(ast::TernaryOpExpression &t) override;
        void *visit(ast::UnaryOpExpression &e) override;
        void *visit(ast::VariableDeclaration &d) override;
        void *visit(ast::VariableExpression &v) override;

    private:
        static Utf8String pxTypeToCType(Type *type);
        Utf8String generateIncludes();
        Utf8String generateStringDecl();
        Utf8String buildFunctionProto(Function *function);

        Utf8String toPreDeclare;
        px::Function *currentFunction;
        px::Scope *currentScope;
        px::ScopeTree * const scopeTree;
    };

}

#endif

