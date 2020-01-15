
#ifndef _PX_SYMBOL_H_
#define _PX_SYMBOL_H_

#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include "Utf8String.h"

namespace px
{
    class Function;
    class Type;
    class Variable;

    class OtherSymbolData
    {
    protected:
        OtherSymbolData() = default;
    public:
        virtual ~OtherSymbolData() = default;
    };

    enum class SymbolType
    {
        UNKNOWN = 0,
        TYPE,
        FUNCTION,
        VARIABLE
    };

    class Symbol
    {
    public:

        const Utf8String name;
        const SymbolType symbolType;
        std::unique_ptr<OtherSymbolData> data;

        virtual ~Symbol() = default;

    protected:

        Symbol(const Utf8String &n, SymbolType t)
            : name(n), symbolType(t), data(nullptr)
        {
        }

    };

    class Type : public Symbol
    {
    public:
        static Type * const UNKNOWN;
        static Type * const OBJECT;
        static Type * const VOID;
        static Type * const BOOL;
        static Type * const INT8;
        static Type * const INT16;
        static Type * const INT32;
        static Type * const INT64;
        static Type * const UINT8;
        static Type * const UINT16;
        static Type * const UINT32;
        static Type * const UINT64;
        static Type * const FLOAT32;
        static Type * const FLOAT64;
        static Type * const CHAR;
        static Type * const STRING;

        enum TypeFlags : uint32_t
        {
            NONE = 0,
            BUILTIN = 0x01,
            BUILTIN_BOOL = 0x02 | BUILTIN,
            BUILTIN_INT = 0x04 | BUILTIN,
            BUILTIN_UINT = 0x08 | BUILTIN,
            BUILTIN_FLOAT = 0x10 | BUILTIN,
            BUILTIN_CHAR = 0x20 | BUILTIN,
            BUILTIN_STRING = 0x40 | BUILTIN,
            BUILTIN_VOID = 0x80 | BUILTIN,
            BUILTIN_ARRAY = 0x100 | BUILTIN,
            ABSTRACT = 0x100,
            SEALED = 0x200,
        };

        Type(const Utf8String &n, Type *p, size_t s, unsigned int f)
            : Symbol{ n, SymbolType::TYPE }, parent{ p }, size{ s }, flags{ f }
        {
        }

        Type(const Type &other)
            : Type{ other.name, other.parent, other.size, other.flags }
        {
        }

        bool inherits(Type *t) const
        {
            return t != nullptr && parent != nullptr && (t == parent || parent->inherits(t));
        }

        bool isCastableTo(Type *other) const
        {
            if (this == other && other != Type::VOID)
            {
                return true;
            }
            else if ((isInt() || isUInt() || isFloat()) && (other->isInt() || other->isUInt() || other->isFloat()))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool isImpiciltyCastableTo(Type *other) const
        {
            if (this == other && other != Type::VOID)
            {
                return true;
            }
            else if ((isInt() && other->isInt()) || (isUInt() && other->isUInt()) || (isFloat() && other->isFloat()))
            {
                if (other->size > size)
                    return true;
            }

            return false;
        }

        bool is_a(Type *t) const
        {
            return this == t || inherits(t);
        }

        bool isBuiltin(unsigned int builtinFlag) const
        {
            return (flags & builtinFlag) == builtinFlag;
        }

        bool isBuiltin() const
        {
            return (flags & BUILTIN) == BUILTIN;
        }

        bool isVoid() const
        {
            return isBuiltin(BUILTIN_VOID);
        }

        bool isBool() const
        {
            return isBuiltin(BUILTIN_BOOL);
        }

        bool isFloat() const
        {
            return isBuiltin(BUILTIN_FLOAT);
        }

        bool isInt() const
        {
            return isBuiltin(BUILTIN_INT);
        }

        bool isUInt() const
        {
            return isBuiltin(BUILTIN_UINT);
        }

        bool isChar() const
        {
            return isBuiltin(BUILTIN_CHAR);
        }

        bool isString() const
        {
            return isBuiltin(BUILTIN_STRING);
        }

        bool isArray() const
        {
            return isBuiltin(BUILTIN_ARRAY);
        }

        Type * const parent;
        const size_t size;
        const unsigned int flags;

    };

    class ArrayType : public Type
    {
    public:
        ArrayType(Type *element,  size_t length)
                : Type{ element->name + "[" + std::to_string(length) + "]", nullptr, element->size * length, BUILTIN_ARRAY}, elementType{ element }, count{ length }
        {
        }

        ArrayType(const ArrayType &other)
                : ArrayType{other.elementType,  other.count}
        {
        }


        Type * const elementType;
        const size_t count;
    };
    class SymbolTable
    {
    public:
        SymbolTable(SymbolTable *parent = nullptr) : _parent{ parent }
        {
        }

        ~SymbolTable()
        {
            for (auto entry : _symbols)
            {
                if(entry.second->symbolType == SymbolType::TYPE){
                    Type *type = (Type*) entry.second;
                    if(type->isBuiltin())  {
                        continue;
                    }
                }
                delete entry.second;
            }
        }

        SymbolTable* getParent()
        {
            return _parent;
        }

        void addSymbol(Symbol *symbol)
        {
            _symbols[symbol->name] = symbol;
        }

        Symbol* getSymbol(const Utf8String &name, bool localsOnly = false) const
        {
            auto symbol = _symbols.find(name);
            if (symbol != _symbols.end())
                return symbol->second;
            else if (!localsOnly && _parent != nullptr)
                return _parent->getSymbol(name);
            else
                return nullptr;
        }

        template<typename T>
        T* getSymbol(const Utf8String &name, SymbolType type, bool localsOnly = false) const
        {
            auto entry = _symbols.find(name);
            if (entry != _symbols.end() && entry->second->symbolType == type)
                return (T*)entry->second;
            else if (!localsOnly && _parent != nullptr)
                return _parent->getSymbol<T>(name, type);
            else
                return nullptr;
        }

        Type* getType(const Utf8String &name, bool localsOnly = false) const
        {
            return getSymbol<Type>(name, SymbolType::TYPE, localsOnly);
        }

        Variable* getVariable(const Utf8String &name, bool localsOnly = false) const
        {
            return getSymbol<Variable>(name, SymbolType::VARIABLE, localsOnly);
        }

        Function* getFunction(const Utf8String &name, bool localsOnly = false) const
        {
            return getSymbol<Function>(name, SymbolType::FUNCTION, localsOnly);
        }

        std::vector<Variable*> getLocalVariables() const
        {
            std::vector<Variable*> variables;
            for (auto &local : _symbols)
            {
                if (local.second->symbolType == SymbolType::VARIABLE)
                    variables.push_back((Variable*)local.second);
            }
            return variables;
        }

        void addGlobals()
        {
            addSymbol(Type::OBJECT);
            addSymbol(Type::VOID);
            addSymbol(Type::BOOL);
            addSymbol(Type::INT8);
            addSymbol(Type::INT16);
            addSymbol(Type::INT32);
            addSymbol(Type::INT64);
            addSymbol(Type::UINT8);
            addSymbol(Type::UINT16);
            addSymbol(Type::UINT32);
            addSymbol(Type::UINT64);
            addSymbol(Type::FLOAT32);
            addSymbol(Type::FLOAT64);
            addSymbol(Type::CHAR);
            addSymbol(Type::STRING);
        }

    private:
        std::unordered_map<Utf8String, Symbol*> _symbols;
        SymbolTable * const _parent;
    };

    class Function : public Symbol
    {
    public:
        Type * returnType;
        std::vector<Variable*> parameters;
        bool declared;
        bool isExtern;

        Function(const Utf8String &func, const std::vector<Variable*> &params, Type *retType, bool ext, bool declare = false)
            : Symbol{ func, SymbolType::FUNCTION }, returnType {retType}, parameters{ params }, declared(declare), isExtern{ext}
        {
        }
    };

    class Variable : public Symbol
    {
    public:
        Type * type;
        Variable(const Utf8String &var, Type *t)
            : Symbol{ var, SymbolType::VARIABLE }, type{ t }
        {
        }
    };
}

#endif
