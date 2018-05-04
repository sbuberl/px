
#include "ContextAnalyzer.h"

#include <iostream>
#include <functional>

namespace px
{
    ContextAnalyzer::ContextAnalyzer(Scope *rootScope, ErrorLog *log)
        : _currentScope(rootScope), errors(log)
    {

    }

    void ContextAnalyzer::analyze(ast::AST &ast)
    {
        ast.accept(*this);
    }

    void ContextAnalyzer::checkAssignmentTypes(Variable *variable, std::unique_ptr<ast::Expression> &expression, const SourcePosition &start)
    {
        Type *varType = variable->type;
        Type *exprType = expression->type;
        if (varType == exprType)
            return;

        if (!exprType->isImpiciltyCastableTo(varType) && exprType != Type::UNKNOWN && varType!= Type::UNKNOWN)
        {
            errors->addError(Error{ start, Utf8String{ "Can not implicitly convert from '" } + exprType->name + "' to '" + varType->name + "'" });
        }

        if (varType->isInt())
        {
            if (exprType->isInt())
            {
                if (varType->size > exprType->size)
                {
                    expression->type = varType;
                    expression = std::make_unique<ast::CastExpression>(start, varType->name, std::move(expression));
                    expression->accept(*this);
                }
            }
        }
        else if (varType->isUInt())
        {
            if (exprType->isUInt())
            {
                if (varType->size > exprType->size)
                {
                    expression->type = varType;
                    expression = std::make_unique<ast::CastExpression>(start, varType->name, std::move(expression));
                    expression->accept(*this);
                }
            }
        }
        else if (varType->isFloat())
        {
            if (exprType->isFloat())
            {
                if (varType->size > exprType->size)
                {
                    expression->type = varType;
                    expression = std::make_unique<ast::CastExpression>(start, varType->name, std::move(expression));
                    expression->accept(*this);
                }
            }
        }

    }
    void* ContextAnalyzer::visit(ast::AssignmentStatement &a)
    {
        auto symbols = _currentScope->symbols();
        Variable *variable = symbols->getVariable(a.variableName);
        if (variable == nullptr)
        {
            errors->addError(Error{ a.position, Utf8String{ "Variable " } + a.variableName + " is not declared in the current scope" });
            return nullptr;
        }
        a.expression->accept(*this);
        checkAssignmentTypes(variable, a.expression, a.position);
        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::BinaryOpExpression &b)
    {
        b.left->accept(*this);
        b.right->accept(*this);

        Type *leftType = b.left->type;
        Type *rightType = b.right->type;

        SourcePosition leftPosition = b.left->position;
        SourcePosition rightPosition = b.right->position;

        unsigned int combinedFlags = leftType->flags & leftType->flags;

        if (!leftType->isImpiciltyCastableTo(rightType) && !rightType->isImpiciltyCastableTo(leftType))
        {
            errors->addError(Error{ leftPosition, Utf8String{ "Can not perform binary operators between '" }  + leftType->name + "' and '" + rightType->name + "'" });
        }

        if (b.op >= ast::BinaryOperator::OR && b.op <= ast::BinaryOperator::NE)
        {
            b.type = Type::BOOL;
        }
        else if (combinedFlags & Type::BUILTIN)
        {
            if (leftType == rightType)
            {
                b.type = leftType;
            }
            else if ((leftType->isInt() && rightType->isInt()) || (leftType->isUInt() && rightType->isUInt())
                || (leftType->isFloat() && rightType->isFloat()))
            {
                if (leftType->size > rightType->size)
                {
                    b.type = leftType;
                    b.right = std::make_unique<ast::CastExpression>(rightPosition, leftType->name, std::move(b.right));
                    b.right->accept(*this);
                }
                else if (leftType->size < rightType->size)
                {
                    b.type = rightType;
                    b.left = std::make_unique<ast::CastExpression>(leftPosition, rightType->name, std::move(b.left));
                    b.left->accept(*this);
                }
            }
        }

        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::BlockStatement &s)
    {
        auto current = _currentScope;
        auto newScope = new Scope(current);
        _currentScope = newScope;
        for (auto &statement : s.statements)
        {
            statement->accept(*this);
        }
        _currentScope = current;
        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::BoolLiteral &b)
    {
        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::CastExpression &c)
    {
        c.expression->accept(*this);

        Type *originalType = c.expression->type;
        auto symbols = _currentScope->symbols();
        Type *castTo = symbols->getType(c.newTypeName);
        if (castTo == nullptr)
        {
            errors->addError(Error{ c.position, Utf8String{ "Type " } +c.newTypeName + " was not found" });
            return nullptr;
        }

        c.type = castTo;

        if (!originalType->isCastableTo(castTo))
        {
            errors->addError(Error{ c.position, Utf8String{ "Can not convert from '" }  + originalType->name + "' to '" + castTo->name + "'" });
        }

        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::CharLiteral &c)
    {
        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::ExpressionStatement &s)
    {
        s.expression->accept(*this);
        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::FloatLiteral &f)
    {
        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::FunctionDeclaration &f)
    {
        auto currentSymbols = _currentScope->symbols();
        Type *returnType = currentSymbols->getType(f.returnTypeName);
        if (returnType == nullptr)
        {
            errors->addError(Error{ f.position, Utf8String{ "Return type " } + f.returnTypeName + " was not found" });
            return nullptr;
        }
        Function *function = new Function{ f.name, returnType };
        f.function = function;
        currentSymbols->addSymbol(function);
        f.block->accept(*this);
        return nullptr;
    }

    void * ContextAnalyzer::visit(ast::IfStatement & i)
    {
        i.condition->accept(*this);
        if (!i.condition->type->isBool())
        {
            errors->addError(Error{ i.position, Utf8String{ "If condition must be of type bool" } });
            return nullptr;
        }

        i.trueStatement->accept(*this);
        if (i.elseStatement)
            i.elseStatement->accept(*this);

        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::IntegerLiteral &i)
    {
        return nullptr;
    }

    void * ContextAnalyzer::visit(ast::Module & m)
    {
        m.block->accept(*this);
        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::ReturnStatement &s)
    {
        if (s.returnValue != nullptr)
            s.returnValue->accept(*this);
        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::StringLiteral &s)
    {
        return nullptr;
    }

    void *ContextAnalyzer::visit(ast::TernaryOpExpression &t)
    {
        t.condition->accept(*this);
        if (!t.condition->type->isBool())
        {
            errors->addError(Error{ t.position, Utf8String{ "Ternary condition must be of type bool" } } );
            return nullptr;
        }

        t.trueExpr->accept(*this);
        t.falseExpr->accept(*this);

        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::UnaryOpExpression &e)
    {
        e.expression->accept(*this);
        e.type = e.expression->type;
        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::VariableDeclaration &d)
    {
        auto symbols = _currentScope->symbols();
        Type *type = symbols->getType(d.typeName);
        if (type == nullptr)
        {
            errors->addError(Error{ d.position, Utf8String{ "Type " } + d.typeName + " was not found" });
            return nullptr;
        }
        if (symbols->getVariable(d.name, true) != nullptr)
        {
            errors->addError(Error{ d.position, Utf8String{ "Variable " } + d.name + " already delcared in the current scope" });
            return nullptr;
        }
        auto variable = new Variable{ d.name, type };
        symbols->addSymbol(variable);
        if (d.initialValue)
        {
            d.initialValue->accept(*this);
            checkAssignmentTypes(variable, d.initialValue, d.position);
        }
        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::VariableExpression &v)
    {
        auto symbols = _currentScope->symbols();
        Variable *variable = symbols->getVariable(v.variable);
        if (variable == nullptr)
        {
            errors->addError(Error{ v.position, Utf8String{ "Variable " } + v.variable + " is not declared in the current scope" });
            return nullptr;
        }
        v.type = variable->type;
        return nullptr;
    }


}
