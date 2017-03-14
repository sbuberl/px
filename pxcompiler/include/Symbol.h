
#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>

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

        const std::string name;
        const SymbolType symbolType;
        OtherSymbolData *data;

        virtual ~Symbol()
        {
            delete data;
        }

    protected:

        Symbol(const std::string &name, SymbolType type)
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
        static Type * const FLOAT32;
        static Type * const FLOAT64;
        static Type * const STRING;

        enum TypeFlags
        {
            NONE = 0,
            BUILTIN = 0x08,
            BUILTIN_BOOL = 0x09,
            BUILTIN_INT,
            BUILTIN_FLOAT,
            BUILTIN_STRING,
            BUILTIN_VOID,
            ABSTRACT = 0x10,
            SEALED = 0x20,
        };

        Type(std::string n, Type *p, unsigned int s, unsigned int f)
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

        bool isVoidType() const
        {
            return isBuiltin(BUILTIN_VOID);
        }

        bool isBoolType() const
        {
            return isBuiltin(BUILTIN_BOOL);
        }

        bool isBuiltinFloat() const
        {
            return isBuiltin(BUILTIN_FLOAT);
        }

        bool isBuiltinInt() const
        {
            return isBuiltin(BUILTIN_INT);
        }

        bool isStringType() const
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

        Symbol* getSymbol(const std::string &name, bool localsOnly = false) const
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
        T* getSymbol(const std::string &name, SymbolType type, bool localsOnly = false) const
        {
            auto entry = _symbols.find(name);
            if (entry != _symbols.end() && entry->second->symbolType == type)
                return (T*)entry->second;
            else if (!localsOnly && _parent != nullptr)
                return _parent->getSymbol<T>(name, type);
            else
                return nullptr;
        }

        Type* getType(std::string name, bool localsOnly = false) const
        {
            return getSymbol<Type>(name, SymbolType::TYPE, localsOnly);
        }

        Variable* getVariable(std::string name, bool localsOnly = false) const
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
            addSymbol(Type::FLOAT32);
            addSymbol(Type::FLOAT64);
        }

    private:
        std::unordered_map<std::string, Symbol*> _symbols;
        SymbolTable * const _parent;
    };

    class Function : public Symbol
    {
    public:
        Type * returnType;
        SymbolTable symbols;

        Function(std::string func, Type *retType, SymbolTable *parentScope)
            : Symbol(func, SymbolType::FUNCTION), returnType(retType), symbols(parentScope)
        {
        }
    };

    class Variable : public Symbol
    {
    public:
        Type * type;
        Variable(std::string var, Type *t)
            : Symbol(var, SymbolType::VARIABLE), type(t)
        {
        }
    };
}

#endif
