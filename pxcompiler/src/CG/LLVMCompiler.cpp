
#include "CG/LLVMCompiler.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"

#include <functional>
#include <iostream>

namespace px
{
    static llvm::StructType* stringType;

    static llvm::StructType* getStringType(llvm::LLVMContext &context)
    {
        if (!stringType)
        {
            llvm::Type* runeLengthType = llvm::Type::getInt32Ty(context);
            llvm::Type* byteCount = llvm::Type::getInt32Ty(context);
            llvm::Type* utf8PointerType = llvm::Type::getInt8PtrTy(context);
            std::vector<llvm::Type*> values = { utf8PointerType , runeLengthType, byteCount };
            stringType = llvm::StructType::create(context, values, "string");
        }
        return stringType;
    }

    LLVMCompiler::LLVMCompiler(ScopeTree *tree) : scopeTree{ tree }, context{}, builder{ context }, currentFunction{ nullptr }
    {
        module = llvm::make_unique<llvm::Module>("test", context);
        currentScope = new LLVMScope{ tree->current(), nullptr };
    }

    llvm::Type *LLVMCompiler::pxTypeToLlvmType(Type *pxType)
    {
        if (pxType->isInt() || pxType->isUInt())
        {
            switch (pxType->size)
            {
                case 1:
                case 2:
                case 4:
                case 8:
                    return llvm::IntegerType::get(context, pxType->size * 8);
            }
        }
        else if (pxType->isFloat())
        {
            switch (pxType->size)
            {
                case 4:
                    return llvm::Type::getFloatTy(context);
                case 8:
                    return llvm::Type::getDoubleTy(context);
            }
        }
        else if (pxType->isVoid())
            return llvm::Type::getVoidTy(context);
        else if(pxType->isBool())
            return llvm::Type::getInt1Ty(context);
        else if (pxType->isChar())
            return llvm::Type::getInt32Ty(context);
        else if (pxType->isString())
            return getStringType(context);

        return nullptr;
    }

    void LLVMCompiler::compile(ast::AST& ast)
    {
        ast.accept(*this);
       /* std::string output;
        llvm::raw_string_ostream stringStream{ output };
        llvm::WriteBitcodeToFile(module.get(), stringStream);
        std::cout << stringStream.str(); */
    }

    void* LLVMCompiler::visit(ast::AssignmentStatement &a)
    {
        llvm::Value *expression = (llvm::Value*) a.expression->accept(*this);
        auto varScope = currentScope->findVariable(a.variableName);
        llvm::AllocaInst *memory = varScope->variables[a.variableName];
        builder.CreateStore(expression, memory);
        return nullptr;
    }

    void* LLVMCompiler::visit(ast::BinaryOpExpression &b)
    {
        llvm::Value *left = (llvm::Value*) b.left->accept(*this);
        llvm::Value *right = (llvm::Value*) b.right->accept(*this);
        px::Type *leftType = b.left->type;

        if ((leftType->isInt()))
        {
            switch (b.op)
            {
                case ast::BinaryOperator::ADD:      return builder.CreateAdd(left, right);
                case ast::BinaryOperator::SUB:      return builder.CreateSub(left, right);
                case ast::BinaryOperator::MUL:      return builder.CreateMul(left, right);
                case ast::BinaryOperator::DIV:      return builder.CreateSDiv(left, right);
                case ast::BinaryOperator::MOD:      return builder.CreateSRem(left, right);
                case ast::BinaryOperator::LSH:      return builder.CreateShl(left, right);
                case ast::BinaryOperator::RSH:      return builder.CreateAShr(left, right);
                case ast::BinaryOperator::BIT_AND:  return builder.CreateAnd(left, right);
                case ast::BinaryOperator::BIT_OR:   return builder.CreateOr(left, right);
                case ast::BinaryOperator::BIT_XOR:  return builder.CreateXor(left, right);
                case ast::BinaryOperator::EQ:       return builder.CreateICmpEQ(left, right);
                case ast::BinaryOperator::NE:       return builder.CreateICmpNE(left, right);
                case ast::BinaryOperator::LT:       return builder.CreateICmpSLT(left, right);
                case ast::BinaryOperator::LTE:      return builder.CreateICmpSLE(left, right);
                case ast::BinaryOperator::GT:       return builder.CreateICmpSGT(left, right);
                case ast::BinaryOperator::GTE:      return builder.CreateICmpSGE(left, right);
                default:	return nullptr;
            }
        }
        else if ((leftType->isUInt()))
        {
            switch (b.op)
            {
                case ast::BinaryOperator::ADD:      return builder.CreateAdd(left, right);
                case ast::BinaryOperator::SUB:      return builder.CreateSub(left, right);
                case ast::BinaryOperator::MUL:      return builder.CreateMul(left, right);
                case ast::BinaryOperator::DIV:      return builder.CreateUDiv(left, right);
                case ast::BinaryOperator::MOD:      return builder.CreateURem(left, right);
                case ast::BinaryOperator::LSH:      return builder.CreateShl(left, right);
                case ast::BinaryOperator::RSH:      return builder.CreateLShr(left, right);
                case ast::BinaryOperator::BIT_AND:  return builder.CreateAnd(left, right);
                case ast::BinaryOperator::BIT_OR:   return builder.CreateOr(left, right);
                case ast::BinaryOperator::BIT_XOR:  return builder.CreateXor(left, right);
                case ast::BinaryOperator::EQ:       return builder.CreateICmpEQ(left, right);
                case ast::BinaryOperator::NE:       return builder.CreateICmpNE(left, right);
                case ast::BinaryOperator::LT:       return builder.CreateICmpULT(left, right);
                case ast::BinaryOperator::LTE:      return builder.CreateICmpULE(left, right);
                case ast::BinaryOperator::GT:       return builder.CreateICmpUGT(left, right);
                case ast::BinaryOperator::GTE:      return builder.CreateICmpUGE(left, right);
                default:	return nullptr;
            }
        }
        else if ((leftType->isFloat()))
        {
            switch (b.op)
            {
                case ast::BinaryOperator::ADD:      return builder.CreateFAdd(left, right);
                case ast::BinaryOperator::SUB:      return builder.CreateFSub(left, right);
                case ast::BinaryOperator::MUL:      return builder.CreateFMul(left, right);
                case ast::BinaryOperator::DIV:      return builder.CreateFDiv(left, right);
                case ast::BinaryOperator::MOD:      return builder.CreateFRem(left, right);
                case ast::BinaryOperator::EQ:       return builder.CreateFCmpUEQ(left, right);
                case ast::BinaryOperator::NE:       return builder.CreateFCmpUNE(left, right);
                case ast::BinaryOperator::LT:       return builder.CreateFCmpULT(left, right);
                case ast::BinaryOperator::LTE:      return builder.CreateFCmpULE(left, right);
                case ast::BinaryOperator::GT:       return builder.CreateFCmpUGT(left, right);
                case ast::BinaryOperator::GTE:      return builder.CreateFCmpUGE(left, right);
                default:	return nullptr;
            }
        }
        else if ((leftType->isBool()))
        {
            switch (b.op)
            {
            case ast::BinaryOperator::BIT_AND:  return builder.CreateAnd(left, right);
            case ast::BinaryOperator::BIT_OR:   return builder.CreateOr(left, right);
            case ast::BinaryOperator::BIT_XOR:  return builder.CreateXor(left, right);
            case ast::BinaryOperator::EQ:       return builder.CreateICmpEQ(left, right);
            case ast::BinaryOperator::NE:       return builder.CreateICmpNE(left, right);
            case ast::BinaryOperator::LT:       return builder.CreateICmpSLT(left, right);
            case ast::BinaryOperator::LTE:      return builder.CreateICmpSLE(left, right);
            case ast::BinaryOperator::GT:       return builder.CreateICmpSGT(left, right);
            case ast::BinaryOperator::GTE:      return builder.CreateICmpSGE(left, right);
            default:	return nullptr;
            }
        }

        return nullptr;

    }

    void* LLVMCompiler::visit(ast::BlockStatement &s)
    {
        auto current = currentScope;
        auto llvmScope = new LLVMScope{ scopeTree->enterScope(), current };
        auto pxScope = llvmScope->scope;
        std::vector<Variable*> locals = pxScope->symbols()->getLocalVariables();
        for (auto variable : locals)
        {
            auto pxType = pxTypeToLlvmType(variable->type);
            llvm::AllocaInst *varMemory = builder.CreateAlloca(pxType);
            varMemory->setName(variable->name.toString());
            llvmScope->variables[variable->name] = varMemory;
        }

        currentScope = llvmScope;
        for (auto const& statement : s.statements)
        {
            statement->accept(*this);
        }
        scopeTree->endScope();
        currentScope = current;
        return nullptr;
    }

    void* LLVMCompiler::visit(ast::BoolLiteral &i)
    {
        return llvm::ConstantInt::get(builder.getInt1Ty(), i.value);
    }

    void* LLVMCompiler::visit(ast::CastExpression &e)
    {
        llvm::Value *value = (llvm::Value*) e.expression->accept(*this);
        Type *type = e.type, *origType = e.expression->type;

        if (type->isInt())
        {
            if (origType->isInt() || origType->isUInt())
            {
                if (type->size != origType->size)
                    return builder.CreateZExtOrTrunc(value, pxTypeToLlvmType(type));
                else
                    return value;
            }
            else if (origType->isFloat())
            {
                return builder.CreateFPToSI(value, pxTypeToLlvmType(type));
            }
        }
        else if (type->isUInt())
        {
            if(origType->isUInt() || origType->isInt())
            {
                if (type->size != origType->size)
                    return builder.CreateSExtOrTrunc(value, pxTypeToLlvmType(type));
                else
                    return value;
            }
            else if (origType->isFloat())
            {
                return builder.CreateFPToUI(value, pxTypeToLlvmType(type));
            }
        }
        else if (type->isFloat())
        {
            if(origType->isFloat())
            {
                if (type->size < origType->size)
                    return builder.CreateFPTrunc(value, pxTypeToLlvmType(type));
                else if (type->size > origType->size)
                    return builder.CreateFPExt(value, pxTypeToLlvmType(type));
                else
                    return value;
            }
            else if (origType->isInt())
            {
                return builder.CreateSIToFP(value, pxTypeToLlvmType(type));
            }
            else if (origType->isUInt())
            {
                return builder.CreateUIToFP(value, pxTypeToLlvmType(type));
            }
        }

        return nullptr;
    }

    void* LLVMCompiler::visit(ast::CharLiteral &c)
    {
        return llvm::ConstantInt::get(builder.getInt32Ty(), c.literal[0]);
    }

    void* LLVMCompiler::visit(ast::ExpressionStatement &s)
    {
        llvm::Value *value = (llvm::Value*) s.expression->accept(*this);
        return value;
    }

    void* LLVMCompiler::visit(ast::FloatLiteral &f)
    {
        return llvm::ConstantFP::get(pxTypeToLlvmType(f.type), f.value);
    }

    void* LLVMCompiler::visit(ast::FunctionDeclaration &f)
    {
        Function *function = f.function;

        llvm::Type *RT = pxTypeToLlvmType(function->returnType);
        llvm::FunctionType *FT = llvm::FunctionType::get(RT, std::vector<llvm::Type*>(), false);
        llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, f.name.c_str(), module.get());
        llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, (f.name.toString() + ".0").c_str(), F);
        builder.SetInsertPoint(BB);

        function->data.reset(new LLVMFunctionData(F));

        Function *prevFunction = currentFunction;
        llvm::BasicBlock *prevBlock = currentBlock;
        currentFunction = function;
        currentBlock = BB;

        f.block->accept(*this);

        currentFunction = prevFunction;
        currentBlock = prevBlock;

        // F is a pointer to a Function instance
        std::string output;
        llvm::raw_string_ostream stringStream{ output };
        for (llvm::inst_iterator I = llvm::inst_begin(F), E = llvm::inst_end(F); I != E; ++I)
        {
            I->print(stringStream);
            stringStream << "\n";
        }

        std::cout << stringStream.str();
        return F;
    }

    void * LLVMCompiler::visit(ast::IfStatement & i)
    {
        llvm::Value *condition = (llvm::Value*) i.condition->accept(*this);

        LLVMFunctionData *funcData = (LLVMFunctionData*) currentFunction->data.get();
        llvm::Function *function = funcData->function;
        auto thenBlock = llvm::BasicBlock::Create(context, "", function);
        llvm::BasicBlock *elseBlock = nullptr;
        auto endBlock = llvm::BasicBlock::Create(context, "", function);

        if (i.elseStatement)
        {
            elseBlock = llvm::BasicBlock::Create(context, "", function);
            llvm::BranchInst::Create(thenBlock, elseBlock, condition, currentBlock);
        }
        else
        {
            llvm::BranchInst::Create(thenBlock, endBlock, condition, currentBlock);
        }

        currentBlock = thenBlock;
        i.trueStatement->accept(*this);

        if (!currentBlock->getTerminator())
            builder.CreateBr(endBlock);

        if (i.elseStatement) {
            elseBlock->moveAfter(currentBlock);
            currentBlock = elseBlock;
            i.elseStatement->accept(*this);
            if (!currentBlock->getTerminator())
                builder.CreateBr(endBlock);

            endBlock->moveAfter(currentBlock);
            currentBlock = endBlock;
        }
        return nullptr;
    }

    void* LLVMCompiler::visit(ast::IntegerLiteral &i)
    {
        return llvm::ConstantInt::get(pxTypeToLlvmType(i.type), i.value);
    }

    void * LLVMCompiler::visit(ast::Module & m)
    {
        m.block->accept(*this);
        return nullptr;
    }

    void* LLVMCompiler::visit(ast::ReturnStatement &s)
    {
        if (s.returnValue != nullptr)
        {
            llvm::Value *result = (llvm::Value*) s.returnValue->accept(*this);
            return builder.CreateRet(result);
        }
        else
            return builder.CreateRetVoid();
    }

    void* LLVMCompiler::visit(ast::StringLiteral &s)
    {
        const Utf8String& literal = s.literal;
        llvm::Constant *runeCount = llvm::ConstantInt::get(builder.getInt32Ty(), literal.length());
        llvm::Constant *byteCount = llvm::ConstantInt::get(builder.getInt32Ty(), literal.byteLength());
        llvm::Constant *stringConstant = static_cast<llvm::Constant*>(builder.CreateGlobalStringPtr(literal.toString()));

        std::vector<llvm::Constant*> args = { stringConstant, runeCount, byteCount };
        return llvm::ConstantStruct::get(getStringType(context), args);
    }

    void* LLVMCompiler::visit(ast::TernaryOpExpression &t)
    {
        llvm::Value *condition = (llvm::Value*) t.condition->accept(*this);
        llvm::Value *trueExpr = (llvm::Value*) t.trueExpr->accept(*this);
        llvm::Value *falseExpr = (llvm::Value*) t.falseExpr->accept(*this);
        return builder.CreateSelect(condition, trueExpr, falseExpr);
    }

    void* LLVMCompiler::visit(ast::UnaryOpExpression &e)
    {
        llvm::Value *value = (llvm::Value*) e.expression->accept(*this);
        if (e.type->isInt() || e.type->isUInt())
        {
            switch (e.op)
            {
                case ast::UnaryOperator::NEG:
                    return builder.CreateNeg(value);
                case ast::UnaryOperator::CMPL:
                    return builder.CreateNot(value);
                default:
                    return nullptr;
            }
        }
        else if (e.type->isFloat())
        {
            switch (e.op)
            {
                case ast::UnaryOperator::NEG:
                    return builder.CreateFNeg(value);
                default:
                    return nullptr;
            }
        }
        else
            return nullptr;
    }

    void* LLVMCompiler::visit(ast::VariableDeclaration &d)
    {
        if (d.initialValue != nullptr)
        {
            llvm::Value *value = (llvm::Value*) d.initialValue->accept(*this);
            auto varScope = currentScope->findVariable(d.name);
            llvm::AllocaInst *memory = varScope->variables[d.name];
            return builder.CreateStore(value, memory);
        }
        return nullptr;
    }

    void *LLVMCompiler::visit(ast::VariableExpression &v)
    {
        auto varScope = currentScope->findVariable(v.variable);
        llvm::AllocaInst *memory = varScope->variables[v.variable];
        return builder.CreateLoad(memory);
    }
}
