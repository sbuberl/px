
#include "ContextAnalyzer.h"
#include "Token.h"

#include <iostream>
#include <functional>

namespace px
{
    ContextAnalyzer::ContextAnalyzer(Scope *rootScope, ErrorLog *log)
        : _currentScope{rootScope}, errors{log}, loopDepth{}
    {

    }

    void ContextAnalyzer::analyze(ast::AST &ast)
    {
        ast.accept(*this);
    }

    void ContextAnalyzer::checkAssignmentTypes(Variable *variable, std::unique_ptr<ast::Expression> &expression, const SourcePosition &start) {
        Type *varType = variable->type;
        Type *exprType = expression->type;
        if (varType == exprType)
            return;

        if (exprType->isVoid())
        {
            errors->addError(Error{start, Utf8String{"A value of type 'void' can not be assigned to variable of type '"} + varType->name + "'" });
        }
        else if (!exprType->isImpiciltyCastableTo(varType) && exprType != Type::UNKNOWN && varType != Type::UNKNOWN) {
            errors->addError(Error{start, Utf8String{"Can not implicitly convert from '"} + exprType->name + "' to '" +
                                          varType->name + "'"});
        }

        if (varType->isInt()) {
            if (exprType->isInt()) {
                if (varType->size > exprType->size) {
                    expression->type = varType;
                    expression = std::make_unique<ast::CastExpression>(start, varType->name, std::move(expression));
                    expression->accept(*this);
                }
            }
        } else if (varType->isUInt()) {
            if (exprType->isUInt()) {
                if (varType->size > exprType->size) {
                    expression->type = varType;
                    expression = std::make_unique<ast::CastExpression>(start, varType->name, std::move(expression));
                    expression->accept(*this);
                }
            }
        } else if (varType->isFloat()) {
            if (exprType->isFloat()) {
                if (varType->size > exprType->size) {
                    expression->type = varType;
                    expression = std::make_unique<ast::CastExpression>(start, varType->name, std::move(expression));
                    expression->accept(*this);
                }
            }
        }
    }

    void* ContextAnalyzer::visit(ast::ArrayIndexReference &a)
    {
        a.array->accept(*this);
        a.index->accept(*this);

        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::ArrayLiteral &a)
    {
        for (auto &value : a.values)
        {
            value->accept(*this);
        }

        if (a.values.empty() == false) {
            auto firstType = a.values[0]->type;
            size_t elementCount = a.values.size();
            for(size_t i = 1; i < elementCount; ++i)
            {
                auto currentType = a.values[i]->type;
                if (!currentType->isImpiciltyCastableTo(firstType))
                {
                    errors->addError(Error{ a.position, Utf8String{ "Can not have an array literal with types '"} + firstType->name + "' and a variable of type" + currentType->name + "' without a cast" });
                }
            }
            auto tempType = new ArrayType(firstType, elementCount);
            Type *type = _currentScope->symbols()->getType(tempType->name);
            if (type == nullptr) {
                auto rootScope = _currentScope->root();
                auto rootSymbols = rootScope->symbols();
                rootSymbols->addSymbol(tempType);
                a.type = tempType;
            } else {
                a.type = type;
            }
        }
        else {
            a.type = new ArrayType(Type::UNKNOWN, 0);
        }

        return nullptr;
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

        TokenType opType = a.opType;
        Type *variableType = variable->type;
        Type *expressionType = a.expression->type;

        a.variableType = variableType;

        switch(opType)
        {
            case TokenType::OP_ADD:
            case TokenType::OP_SUB:
            case TokenType::OP_STAR:
            case TokenType::OP_DIV:
            case TokenType::OP_MOD:
                if (!expressionType->isImpiciltyCastableTo(variableType))
                {
                    errors->addError(Error{ a.position, Utf8String{ "Can not perform assignment '"} + Token::getTokenName(opType) + "' between an expression of type '"  + expressionType->name + "' and a variable of type" + variableType->name + "'" });
                }
                break;
            case TokenType::OP_ASSIGN_BIT_AND:
            case TokenType::OP_ASSIGN_BIT_OR:
            case TokenType::OP_ASSIGN_BIT_XOR:
            case TokenType::OP_ASSIGN_LEFT_SHIFT:
            case TokenType::OP_ASSIGN_RIGHT_SHIFT:
                if( !variableType->isInt() && !variableType->isUInt())
                    errors->addError(Error{ a.position, Utf8String{ "Can not perform assignment operator '"} + Token::getTokenName(a.opType) + "' on a variable of type '"  + variableType->name + "'" });
                if( !expressionType->isInt() && !expressionType->isUInt())
                    errors->addError(Error{ a.position, Utf8String{ "Can not perform assignment operator '"} + Token::getTokenName(a.opType) + "' with an expression of type '"  + expressionType->name + "'" });

        }

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
        TokenType opType = b.token;

        if (!leftType->isImpiciltyCastableTo(rightType) && !rightType->isImpiciltyCastableTo(leftType))
        {
            errors->addError(Error{ leftPosition, Utf8String{ "Can not perform binary '"} + Token::getTokenName(opType) + "' between '"  + leftType->name + "' and '" + rightType->name + "'" });
        }

        if (b.op >= ast::BinaryOperator::OR && b.op <= ast::BinaryOperator::NE)
        {
            b.type = Type::BOOL;
            if(b.op == ast::BinaryOperator::OR || b.op == ast::BinaryOperator::AND) {
                if( leftType != Type::BOOL )
                {
                    errors->addError(Error{ leftPosition, Utf8String{ "Left side of binary expression with '"} + Token::getTokenName(opType) + "' must be a bool" });
                }
                if( rightType != Type::BOOL )
                {
                    errors->addError(Error{ rightPosition, Utf8String{ "Right side of binary expression with '"} + Token::getTokenName(opType) + "' must be a bool" });
                }
            }
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

    void* ContextAnalyzer::visit(ast::BreakStatement &b)
    {
        if (loopDepth == 0)
        {
            errors->addError(Error{ b.position, "Can perform a break outside of a loop" });
        }
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

    void* ContextAnalyzer::visit(ast::ContinueStatement &c)
    {
        if (loopDepth == 0)
        {
            errors->addError(Error{ c.position, "Can perform a continue outside of a loop" });
        }
        return nullptr;
    }

    void * ContextAnalyzer::visit(ast::DoWhileStatement &d)
    {
        d.condition->accept(*this);
        if (!d.condition->type->isBool())
        {
            errors->addError(Error{ d.position, Utf8String{ "do..while condition must be of type bool" } });
            return nullptr;
        }

        ++loopDepth;
        d.body->accept(*this);
        --loopDepth;
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

    void * ContextAnalyzer::visit(ast::FunctionCallExpression &f)
    {
        auto currentSymbols = _currentScope->symbols();
        Function *function = currentSymbols->template getSymbol<Function>(f.functionName, SymbolType::FUNCTION);
        if (function == nullptr) {
            errors->addError(Error{f.position, Utf8String{"Function "} + f.functionName + " was not found"});
            return nullptr;
        }

        f.function = function;
        f.type = function->returnType;

        if (function->parameters.size() != f.arguments.size()) {
            errors->addError(Error{f.position,
                                   Utf8String{"Invalid number of arguments given to function "} + f.functionName});
        }

        for (auto &arg : f.arguments) {
            arg->accept(*this);
        }

        return nullptr;
    }

    void * ContextAnalyzer::visit(ast::FunctionDeclaration &f)
    {
        auto current = _currentScope;
        auto currentSymbols = current->symbols();
        auto prototype = *f.prototype;
        Type *returnType = currentSymbols->getType(prototype.returnTypeName);
        if (returnType == nullptr)
        {
            errors->addError(Error{ f.position, Utf8String{ "Return type " } + prototype.returnTypeName + " was not found" });
        }
        std::vector<Variable *> parameters;
        for (ast::Parameter param : prototype.parameters)
        {
            Type *paramType = currentSymbols->getType(param.typeName);
            if (paramType == nullptr)
            {
                errors->addError(Error{ f.position, Utf8String{ "Function parameter type " } + param.typeName + " was not found" });
            }
            Variable *parameter = new Variable{ param.name, paramType };
            parameters.push_back(parameter);
        }
        Function *function = new Function{ prototype.name, parameters, returnType, prototype.isExtern };
        f.function = function;
        currentSymbols->addSymbol(function);
        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::FunctionDefinition &f)
    {
        auto current = _currentScope;
        auto currentSymbols = current->symbols();
        auto prototype = *f.prototype;

        Function *function = currentSymbols->template getSymbol<Function>(prototype.name, SymbolType::FUNCTION);
        if(function == nullptr) {
            Type *returnType = currentSymbols->getType(prototype.returnTypeName);
            if (returnType == nullptr) {
                errors->addError(
                        Error{f.position, Utf8String{"Return type "} + prototype.returnTypeName + " was not found"});
            }
            std::vector<Variable *> parameters;
            for (ast::Parameter param : prototype.parameters) {
                Type *paramType = currentSymbols->getType(param.typeName);
                if (paramType == nullptr) {
                    errors->addError(Error{f.position,
                                           Utf8String{"Function parameter type "} + param.typeName + " was not found"});
                }
                Variable *parameter = new Variable{param.name, paramType};
                parameters.push_back(parameter);
            }

            function = new Function{prototype.name, parameters, returnType, false, true};
            currentSymbols->addSymbol(function);
        }
        else
        {
            function->declared = true;
        }

        if(function->returnType == Type::VOID) {
            auto &lastStatement = f.block->getLastStatement();
            f.block->addStatement(std::make_unique<ast::ReturnStatement>(lastStatement.position));
        }

        f.function = function;
        auto newScope = new Scope(current);
        _currentScope = newScope;
        auto newSymbols = newScope->symbols();
        for (auto &param : function->parameters)
            newSymbols->addSymbol(param);
        for(auto &statement : f.block->statements)
            statement->accept(*this);
        _currentScope = current;
        
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

    void* ContextAnalyzer::visit(ast::Module &m)
    {
        auto current = _currentScope;
        auto newScope = new Scope(current);
        _currentScope = newScope;
        for (auto &statement : m.statements)
        {
            statement->accept(*this);
        }
        _currentScope = current;
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

        auto trueType = t.trueExpr->type;
        auto falseType = t.falseExpr->type;
        if(trueType == falseType) {
            t.type = trueType;
        }
        else if( trueType->isImpiciltyCastableTo(falseType))
        {
            t.trueExpr = std::make_unique<ast::CastExpression>(t.trueExpr->position, falseType->name, std::move(t.trueExpr));
        }
        else if( falseType->isImpiciltyCastableTo(trueType))
        {
            t.falseExpr = std::make_unique<ast::CastExpression>(t.falseExpr->position, trueType->name, std::move(t.falseExpr));
        }

        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::UnaryOpExpression &e)
    {
        e.expression->accept(*this);

        TokenType opType = e.token;
        switch (e.op)
        {
            case ast::UnaryOperator::NEG:
            case ast::UnaryOperator::CMPL:
                if (!e.expression->type->isInt() && !e.expression->type->isUInt() && !e.expression->type->isFloat())
                {
                    errors->addError(Error{ e.position, Utf8String{ "Unary operator '" } + Token::getTokenName(opType) + "' is only allowed with numeric expressions" } );
                    return nullptr;
                }
                e.type = e.expression->type;
                break;
            case ast::UnaryOperator::NOT:
                if (!e.expression->type->isBool())
                {
                    errors->addError(Error{ e.position, Utf8String{ "Unary operator '" } + Token::getTokenName(opType) + "' is only allowed with boolean expressions" } );
                    return nullptr;
                }
                e.type = Type::BOOL;
        }
        return nullptr;
    }

    void* ContextAnalyzer::visit(ast::VariableDeclaration &d) {
        auto symbols = _currentScope->symbols();

        auto typeName = d.typeName;
        Type *type;
        if (d.arraySize) {
            Type *baseType = symbols->getType(typeName);
            typeName += Utf8String("[") + std::to_string(*d.arraySize) + "]";
            type = symbols->getType(typeName);
            if (type == nullptr) {
                auto rootScope = _currentScope->root();
                auto rootSymbols = rootScope->symbols();
                type = new ArrayType(baseType, *d.arraySize);
                rootSymbols->addSymbol(type);
            }
        } else {
            type = symbols->getType(typeName);
            if (type == nullptr) {
                errors->addError(Error{d.position, Utf8String{"Type "} + typeName + " was not found"});
                return nullptr;
            }
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

    void * ContextAnalyzer::visit(ast::WhileStatement &w)
    {
        w.condition->accept(*this);
        if (!w.condition->type->isBool())
        {
            errors->addError(Error{ w.position, Utf8String{ "while condition must be of type bool" } });
            return nullptr;
        }

        ++loopDepth;
        w.body->accept(*this);;
        --loopDepth;
        return nullptr;
    }


}
