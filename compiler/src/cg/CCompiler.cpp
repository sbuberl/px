#include "cg/CCompiler.h"

#include <functional>
#include <iostream>

namespace px
{

    CCompiler::CCompiler(ScopeTree *tree) : currentFunction{ nullptr }, scopeTree{ tree }
    {
        currentScope = tree->current();
    }

    Utf8String CCompiler::pxTypeToCType(Type *pxType)
    {
        if (pxType->isInt())
        {
            switch (pxType->size)
            {
            case 1:
                return "int8_t";
            case 2:
                return "int16_t";
            case 4:
                return "int32_t";
            case 8:
                return "int64_t";
            }
        }
        else if (pxType->isUInt())
        {
            switch (pxType->size)
            {
            case 1:
                return "uint8_t";
            case 2:
                return "uint16_t";
            case 4:
                return "uint32_t";
            case 8:
                return "uint64_t";
            }
        }
        else if (pxType->isFloat())
        {
            switch (pxType->size)
            {
            case 4:
                return "float";
            case 8:
                return "double";
            }
        }
        else if (pxType->isVoid())
            return "void";
        else if (pxType->isBool())
            return "bool";
        else if (pxType->isChar())
            return "int32_t";
        else if (pxType->isString())
            return "PxString";

        return "";
    }


    void* CCompiler::visit(ast::AssignmentStatement &a)
    {
        Utf8String *expression = (Utf8String*) a.expression->accept(*this);
        auto symbolTable = currentScope->symbols();
        Variable *variable = symbolTable->getVariable(a.variableName);
        return new Utf8String(variable->name + " = " + *expression + ";");
    }

    void* CCompiler::visit(ast::BoolLiteral &b)
    {
        return new Utf8String(b.literal);
    }

    void* CCompiler::visit(ast::CharLiteral &c)
    {
        return new Utf8String(Utf8String("u8'") + c.literal + Utf8String("'") );
    }

    void* CCompiler::visit(ast::ExpressionStatement &e)
    {
        Utf8String *expression = (Utf8String*) e.expression->accept(*this);
        return new Utf8String(*expression + ";");
    }

    void* CCompiler::visit(ast::FloatLiteral &f)
    {
        return new Utf8String(f.literal);
    }

    void* CCompiler::visit(ast::IntegerLiteral &i)
    {
        return new Utf8String(i.literal);
    }

    void* CCompiler::visit(ast::StringLiteral &s)
    {
        return new Utf8String(Utf8String("u8\"") + s.literal + Utf8String("\"") );
    }

    void* CCompiler::visit(ast::VariableDeclaration &v)
    {
        auto symbolTable = currentScope->symbols();
        auto pxType = symbolTable->getType(v.typeName);
        Utf8String cTypeName = pxTypeToCType(pxType);
        Utf8String declaration(cTypeName + " " + v.name);
        if (v.initialValue != nullptr) {
            Utf8String *value = (Utf8String *) v.initialValue->accept(*this);
            declaration += Utf8String(" = ") + *value + Utf8String(" = ");
        }
        return new Utf8String(std::move(declaration));
    }

    void* CCompiler::visit(ast::VariableExpression &v)
    {
        return new Utf8String(v.variable);
    }
}
