
#ifndef SYMBOL_H_
#define SYMBOL_H_

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
        VARIABLE,
        PARAMETER
    };

    class Symbol
    {
    public:

        const Utf8String name;
        const SymbolType symbolType;
        OtherSymbolData *data;

        virtual ~Symbol()
        {
            delete data;
        }

    protected:

        Symbol(const Utf8String &name, SymbolType type)
            : name(name), symbolType(type), data(nullptr)
        {
        }

    };

    class Type : public Symbol
    {
    public:
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
            ABSTRACT = 0x100,
            SEALED = 0x200,
        };

        Type(const Utf8String &n, Type *p, unsigned int s, unsigned int f)
            : Symbol(n, SymbolType::TYPE), parent(p), size(s), flags(f)
        {
        }

        virtual ~Type() = default;

        bool inherits(Type *t) const
        {
            return t != nullptr && parent != nullptr && (t == parent || parent->inherits(t));
        }

        bool is_a(Type *t) const
        {
            return this == t || inherits(t);
        }

        bool isBuiltin(int builtinFlag) const
        {
            return (flags & builtinFlag) == builtinFlag;
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
            return isBuiltin(BUILTIN_INT);
        }

        bool isChar() const
        {
            return isBuiltin(BUILTIN_CHAR);
        }

        bool isString() const
        {
            return isBuiltin(BUILTIN_STRING);
        }

        Type * const parent;
        const unsigned int size;
        const unsigned int flags;

    };

    class SymbolTable
    {
    public:
        SymbolTable(SymbolTable *parent = nullptr) : _parent(parent)
        {
        }

        SymbolTable::~SymbolTable()
        {
            for (auto entry : _symbols)
            {
                delete entry.second;
            }
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
        SymbolTable symbols;

        Function(const Utf8String &func, Type *retType, SymbolTable *parentScope)
            : Symbol(func, SymbolType::FUNCTION), returnType(retType), symbols(parentScope)
        {
        }
    };

    class Variable : public Symbol
    {
    public:
        Type * type;
        Variable(const Utf8String &var, Type *t)
            : Symbol(var, SymbolType::VARIABLE), type(t)
        {
        }
    };
}

#endif
