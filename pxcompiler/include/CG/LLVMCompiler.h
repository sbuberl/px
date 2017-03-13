
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

		class LLVMFunctionData : public OtherSymbolData
		{
		public:
			std::map<std::string, llvm::AllocaInst*> variables;
			virtual ~LLVMFunctionData() { }
		};

	    llvm::Type *pxTypeToLlvmType(Type *type);

	public:
		LLVMCompiler();
		void compile(ast::AST *ast);
		virtual void *visit(ast::BinaryOpExpression &f);
		virtual void *visit(ast::BlockStatement &s);
		virtual void *visit(ast::CastExpression &c);
		virtual void *visit(ast::DeclarationStatement &d);
		virtual void *visit(ast::ExpressionStatement &s);
		virtual void *visit(ast::FunctionDeclaration &f);
		virtual void *visit(ast::IntegerLiteral &i);
		virtual void *visit(ast::FloatLiteral &f);
		virtual void *visit(ast::ReturnStatement &s);
		virtual void *visit(ast::StringLiteral &s);
		virtual void *visit(ast::UnaryOpExpression &e);
		virtual void *visit(ast::VariableExpression &v);
	};

}

#endif /* LLVMCOMPILER_H_ */
