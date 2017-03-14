
#include "CG/LLVMCompiler.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Bitcode/ReaderWriter.h"

#include <functional>
#include <iostream>

namespace px
{
    LLVMCompiler::LLVMCompiler() : context(), builder(context), currentFunction(NULL)
    {
        module = llvm::make_unique<llvm::Module>("test", context);
    }

    llvm::Type *LLVMCompiler::pxTypeToLlvmType(Type *pxType)
    {
        if (pxType->isBuiltinInt())
        {
            switch (pxType->size)
            {
                case 1:
                    return llvm::Type::getInt8Ty(context);
                case 2:
                    return llvm::Type::getInt16Ty(context);
                case 4:
                    return llvm::Type::getInt32Ty(context);
                case 8:
                    return llvm::Type::getInt64Ty(context);
            }
        }
        else if (pxType->isBuiltinFloat())
        {
            switch (pxType->size)
            {
                case 4:
                    return llvm::Type::getFloatTy(context);
                case 8:
                    return llvm::Type::getDoubleTy(context);
            }
        }
        else if (pxType->isVoidType())
            return llvm::Type::getVoidTy(context);

        return NULL;
    }

    void LLVMCompiler::compile(ast::AST* ast)
    {
        ast->accept(*this);
        //llvm::WriteBitcodeToFile(module, std::cout);
    }

    void* LLVMCompiler::visit(ast::BinaryOpExpression &b)
    {
        llvm::Value *left = (llvm::Value*) b.left->accept(*this);
        llvm::Value *right = (llvm::Value*) b.right->accept(*this);

        llvm::Instruction::BinaryOps llvmOp;
        if ((b.type->flags & Type::BUILTIN_INT) == Type::BUILTIN_INT)
        {
            switch (b.op)
            {
                case ast::BinaryOperator::ADD:	llvmOp = llvm::Instruction::Add; break;
                case ast::BinaryOperator::SUB:	llvmOp = llvm::Instruction::Sub; break;
                case ast::BinaryOperator::MUL:	llvmOp = llvm::Instruction::Mul; break;
                case ast::BinaryOperator::DIV:	llvmOp = llvm::Instruction::SDiv; break;
                case ast::BinaryOperator::MOD:	llvmOp = llvm::Instruction::SRem; break;
                default:	return NULL;
            }
        }
        else if ((b.type->flags & Type::BUILTIN_FLOAT) == Type::BUILTIN_FLOAT)
        {
            switch (b.op)
            {
                case ast::BinaryOperator::ADD:	llvmOp = llvm::Instruction::FAdd; break;
                case ast::BinaryOperator::SUB:	llvmOp = llvm::Instruction::FSub; break;
                case ast::BinaryOperator::MUL:	llvmOp = llvm::Instruction::FMul; break;
                case ast::BinaryOperator::DIV:	llvmOp = llvm::Instruction::FDiv; break;
                case ast::BinaryOperator::MOD:	llvmOp = llvm::Instruction::FRem; break;
                default:	return NULL;
            }
        }
        else
            return NULL;

        return builder.CreateBinOp(llvmOp, left, right);
    }

    void* LLVMCompiler::visit(ast::BlockStatement &s)
    {
        std::vector<ast::Statement*>::const_iterator iter;
        for (iter = s.statements.begin(); iter != s.statements.end(); iter++)
        {
            (*iter)->accept(*this);
        }
        return NULL;
    }

    void* LLVMCompiler::visit(ast::CastExpression &e)
    {
        llvm::Value *value = (llvm::Value*) e.exp->accept(*this);
        Type *type = e.type, *origType = e.exp->type;

        if (type->isBuiltinFloat() && origType->isBuiltinInt())
        {
            return builder.CreateSIToFP(value, pxTypeToLlvmType(type));
        }
        else if (type->isBuiltinInt() && origType->isBuiltinFloat())
        {
            return builder.CreateFPToSI(value, pxTypeToLlvmType(type));
        }
        return NULL;
    }

    void* LLVMCompiler::visit(ast::DeclarationStatement &d)
    {
        if (d.initialValue != NULL)
        {
            LLVMFunctionData *funcData = (LLVMFunctionData*)currentFunction->data;
            llvm::Value *value = (llvm::Value*) d.initialValue->accept(*this);
            llvm::AllocaInst *memory = funcData->variables[d.name];
            return builder.CreateStore(value, memory);
        }
        return NULL;
    }

    void* LLVMCompiler::visit(ast::ExpressionStatement &s)
    {
        llvm::Value *value = (llvm::Value*) s.expression->accept(*this);
        return value;
    }

    void* LLVMCompiler::visit(ast::FunctionDeclaration &f)
    {
        Function *function = f.function;

        llvm::Type *RT = pxTypeToLlvmType(function->returnType);
        llvm::FunctionType *FT = llvm::FunctionType::get(RT, std::vector<llvm::Type*>(), false);
        llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, f.name.c_str(), module.get());
        llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, (f.name + ".0").c_str(), F);
        builder.SetInsertPoint(BB);

        LLVMFunctionData *data = new LLVMFunctionData();
        std::vector<Variable*> locals = function->symbols.getLocalVariables();
        for (std::vector<Variable*>::iterator variable = locals.begin(), end = locals.end(); variable != end; ++variable)
        {
            Variable *var = *variable;
            llvm::AllocaInst *varMemory = builder.CreateAlloca(pxTypeToLlvmType(var->type));
            varMemory->setName(var->name);
            data->variables[var->name] = varMemory;
        }
        function->data = data;

        Function *prevFunction = currentFunction;
        currentFunction = function;

        f.block->accept(*this);

        currentFunction = prevFunction;
        return F;
    }

    void* LLVMCompiler::visit(ast::IntegerLiteral &i)
    {
        return llvm::ConstantInt::get(builder.getInt32Ty(), i.value);
    }

    void* LLVMCompiler::visit(ast::FloatLiteral &f)
    {
        return llvm::ConstantFP::get(builder.getFloatTy(), f.value);
    }

    void* LLVMCompiler::visit(ast::ReturnStatement &s)
    {
        if (s.returnValue != NULL)
        {
            llvm::Value *result = (llvm::Value*) s.returnValue->accept(*this);
            return builder.CreateRet(result);
        }
        else
            return builder.CreateRetVoid();
    }

    void* LLVMCompiler::visit(ast::StringLiteral &s)
    {
        return NULL;
    }

    void* LLVMCompiler::visit(ast::UnaryOpExpression &e)
    {
        llvm::Value *value = (llvm::Value*) e.exp->accept(*this);
        if (e.type->isBuiltinInt())
        {
            switch (e.op)
            {
                case ast::UnaryOperator::NEG:
                    return builder.CreateSub(llvm::ConstantInt::get(value->getType(), 0), value);
                case ast::UnaryOperator::CMPL:
                    return builder.CreateXor(llvm::ConstantInt::get(value->getType(), ~0), value);
                default:
                    return NULL;
            }
        }
        else if (e.type->isBuiltinFloat())
        {
            switch (e.op)
            {
                case ast::UnaryOperator::NEG:
                    return builder.CreateSub(llvm::ConstantFP::get(value->getType(), 0.0), value);
                default:
                    return NULL;
            }
        }
        else
            return NULL;
    }

    void *LLVMCompiler::visit(ast::VariableExpression &v)
    {
        LLVMFunctionData *funcData = (LLVMFunctionData*)currentFunction->data;
        llvm::AllocaInst *memory = funcData->variables[v.variable];
        return builder.CreateLoad(memory);
    }
}
