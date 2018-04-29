
#ifndef LLVMCOMPILER_H_
#define LLVMCOMPILER_H_

#include <ast/Visitor.h>
#include <Symbol.h>

#include <map>
#include <string>

#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"

namespace px {

    class LLVMCompiler : public ast::Visitor
    {
        llvm::LLVMContext context;
        llvm::IRBuilder<> builder;
        std::unique_ptr<llvm::Module> module;
        px::Function *currentFunction;
        llvm::BasicBlock *currentBlock;

        class LLVMFunctionData : public OtherSymbolData
        {
        public:
            std::map<Utf8String, llvm::AllocaInst*> variables;
            llvm::Function *llvmFunction;
            virtual ~LLVMFunctionData() {}
        };

        llvm::Type *pxTypeToLlvmType(Type *type);

    public:
        LLVMCompiler();
        void compile(ast::AST *ast);
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
    };

}

#endif
