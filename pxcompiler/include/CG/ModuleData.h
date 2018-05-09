
#ifndef MODULEDATA_H_
#define MODULEDATA_H_

#include "ast/Visitor.h"
#include "Scope.h"

#include <unordered_map>

#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"

namespace px {

    class ModuleData
    {
    public:
        ModuleData() : context{}
        {
            module = llvm::make_unique<llvm::Module>("main", context);
            addGlobalStructs();
        }

        void addStringStruct()
        {
            llvm::Type* runeLengthType = llvm::Type::getInt32Ty(context);
            llvm::Type* byteCount = llvm::Type::getInt32Ty(context);
            llvm::Type* utf8PointerType = llvm::Type::getInt8PtrTy(context);
            std::vector<llvm::Type*> values = { utf8PointerType , runeLengthType, byteCount };
            structTypes["string"] = llvm::StructType::create(context, values, "string");
        }

        void addGlobalStructs()
        {
            addStringStruct();
        }

        llvm::StructType* getStruct(const Utf8String &name)
        {
            auto symbol = structTypes.find(name);
            if (symbol != structTypes.end())
                return symbol->second;
            else
                return nullptr;
        }

        std::unordered_map<Utf8String, llvm::StructType*> structTypes;
        llvm::LLVMContext context;
        std::unique_ptr<llvm::Module> module;
    };

}

#endif
