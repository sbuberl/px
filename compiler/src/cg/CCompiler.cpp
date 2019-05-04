#include "cg/CCompiler.h"

#include <unicode/ustdio.h>
#include <unicode/ustring.h>

#include <functional>
#include <iostream>
#include <Token.h>

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

    void CCompiler::compile(ast::AST& ast) {
        Utf8String *code = (Utf8String*)ast.accept(*this);

        UFILE *out = u_get_stdout();
        int32_t length;
        UErrorCode errorCode = U_ZERO_ERROR;
        u_strFromUTF8(NULL, 0, &length, code->c_str(), code->byteLength(), &errorCode);
        errorCode = U_ZERO_ERROR;
        std::unique_ptr<UChar[]> utf16Message(new UChar[length + 2]);
        u_strFromUTF8(utf16Message.get(), length, &length, code->c_str(), code->byteLength(), &errorCode);
        utf16Message[length] = '\r';
        utf16Message[length + 1] = '\n';
        u_file_write(utf16Message.get(), length + 2, out);
    }

    void* CCompiler::visit(ast::AssignmentStatement &a)
    {
        Utf8String *expression = (Utf8String*) a.expression->accept(*this);
        auto symbolTable = currentScope->symbols();
        Variable *variable = symbolTable->getVariable(a.variableName);
        return new Utf8String{ variable->name + Token::getTokenName(TokenType::OP_ASSIGN) + *expression + Token::getTokenName(TokenType::OP_END_STATEMENT) + "\n"};
    }

    void* CCompiler::visit(ast::BinaryOpExpression &b)
    {
        Utf8String *left = (Utf8String*) b.left->accept(*this);
        Utf8String *right = (Utf8String*) b.right->accept(*this);
        px::Type *leftType = b.left->type;
        Utf8String opToken;
        if (leftType->isInt() || leftType->isUInt())
        {
            switch (b.op)
            {
                case ast::BinaryOperator::ADD:      opToken = "+"; break;
                case ast::BinaryOperator::SUB:      opToken = "-"; break;
                case ast::BinaryOperator::MUL:      opToken = "*"; break;
                case ast::BinaryOperator::DIV:      opToken = "/"; break;
                case ast::BinaryOperator::MOD:      opToken = "%"; break;
                case ast::BinaryOperator::LSH:      opToken = "<<"; break;
                case ast::BinaryOperator::RSH:      opToken = ">>"; break;
                case ast::BinaryOperator::BIT_AND:  opToken = "&"; break;
                case ast::BinaryOperator::BIT_OR:   opToken = "|"; break;
                case ast::BinaryOperator::BIT_XOR:  opToken = "^"; break;
                case ast::BinaryOperator::EQ:       opToken = "=="; break;
                case ast::BinaryOperator::NE:       opToken = "!="; break;
                case ast::BinaryOperator::LT:       opToken = "<"; break;
                case ast::BinaryOperator::LTE:      opToken = "<="; break;
                case ast::BinaryOperator::GT:       opToken = ">"; break;
                case ast::BinaryOperator::GTE:      opToken = ">="; break;
                default:	return nullptr;
            }
        }
        else if ((leftType->isFloat()))
        {
            switch (b.op)
            {
                case ast::BinaryOperator::ADD:      opToken = "+"; break;
                case ast::BinaryOperator::SUB:      opToken = "-"; break;
                case ast::BinaryOperator::MUL:      opToken = "*"; break;
                case ast::BinaryOperator::DIV:      opToken = "/"; break;
                //    case ast::BinaryOperator::MOD:      opToken = "%"; break;
                case ast::BinaryOperator::EQ:       opToken = "=="; break;
                case ast::BinaryOperator::NE:       opToken = "!="; break;
                case ast::BinaryOperator::LT:       opToken = "<"; break;
                case ast::BinaryOperator::LTE:      opToken = "<="; break;
                case ast::BinaryOperator::GT:       opToken = ">"; break;
                case ast::BinaryOperator::GTE:      opToken = ">="; break;
                default:	return nullptr;
            }
        }
        else if ((leftType->isBool()))
        {
            switch (b.op)
            {
                case ast::BinaryOperator::BIT_AND:  opToken = "&"; break;
                case ast::BinaryOperator::BIT_OR:   opToken = "|"; break;
                case ast::BinaryOperator::BIT_XOR:  opToken = "^"; break;
                case ast::BinaryOperator::EQ:       opToken = "=="; break;
                case ast::BinaryOperator::NE:       opToken = "!="; break;
                case ast::BinaryOperator::LT:       opToken = "<"; break;
                case ast::BinaryOperator::LTE:      opToken = "<="; break;
                case ast::BinaryOperator::GT:       opToken = ">"; break;
                case ast::BinaryOperator::GTE:      opToken = ">="; break;
                default:	return nullptr;
            }
        }

        return new Utf8String{ Utf8String{"("} + *left + " " + opToken + " " + *right + ")" };

    }

    void* CCompiler::visit(ast::BlockStatement &s)
    {
        auto current = currentScope;
        currentScope = scopeTree->enterScope();
//        std::vector<Variable*> locals = currentScope->symbols()->getLocalVariables();
//        for (auto variable : locals)
//        {
//            auto pxType = pxTypeToCType(variable->type);
//            llvm::AllocaInst *varMemory = builder.CreateAlloca(pxType);
//            varMemory->setName(variable->name.toString());
//            llvmScope->variables[variable->name] = varMemory;
//        }

        Utf8String blockCode;
        for (auto const& statement : s.statements)
        {
            blockCode += *(Utf8String *) statement->accept(*this);
        }

        scopeTree->endScope();
        currentScope = current;
        return new Utf8String{ Utf8String{"{\n"} + blockCode + "\n}\n" };
    }

    void* CCompiler::visit(ast::BoolLiteral &b)
    {
        return new Utf8String{ b.literal };
    }

    void* CCompiler::visit(ast::CastExpression &e) {
        Utf8String *value = (Utf8String *) e.expression->accept(*this);
        Type *type = e.type, *origType = e.expression->type;
        Utf8String newTypeName = pxTypeToCType(type);
        bool doCast = false;

        if (type->isInt()) {
            if (origType->isInt() || origType->isUInt()) {
                if (type->size != origType->size)
                    doCast = true;
            } else if (origType->isFloat()) {
                doCast = true;
            }
        } else if (type->isUInt()) {
            if (origType->isUInt() || origType->isInt()) {
                if (type->size != origType->size)
                    doCast = true;
            } else if (origType->isFloat()) {
                doCast = true;
            }
        } else if (type->isFloat()) {
            if (origType->isFloat()) {
                if (type->size != origType->size)
                    doCast = true;
            } else if (origType->isInt() || origType->isUInt()) {
                doCast = true;
            }
        }

        if (doCast) {
            return new Utf8String{Utf8String{"("} + newTypeName + ")" + *value};
        } else {
            return value;
        }
    }

    void* CCompiler::visit(ast::CharLiteral &c)
    {
        return new Utf8String{Utf8String{"'"} + c.literal + Utf8String{"'"} };
    }

    void* CCompiler::visit(ast::ExpressionStatement &e)
    {
        Utf8String *expression = (Utf8String*) e.expression->accept(*this);
        return new Utf8String{ *expression + Token::getTokenName(TokenType::OP_END_STATEMENT) + "\n" };
    }

    void* CCompiler::visit(ast::ExternFunctionDeclaration & e)
    {
        Utf8String prototype = buildFunctionProto(e.function);

        return new Utf8String{ Utf8String{"extern "} + prototype};;
    }

    void* CCompiler::visit(ast::FloatLiteral &f)
    {
        return new Utf8String{ f.literal };
    }

    void* CCompiler::visit(ast::FunctionCallExpression &f)
    {
        Function *pxFunction = f.function;
        Utf8String name = pxFunction->name;
        Utf8String argsText;
        int a = 0, end = f.arguments.size();

        if (pxFunction->isExtern == false && pxFunction->declared == false) {
            toPreDeclare += buildFunctionProto(pxFunction);
        }

        for (auto &arg : f.arguments)
        {
            Utf8String *argValue = (Utf8String*) arg->accept(*this);
            argsText += *argValue;
            if(++a < end) {
                argsText += ", ";
            }
        }

        return new Utf8String{ name + "(" + argsText + ")"};
    }

    void* CCompiler::visit(ast::FunctionDeclaration &f)
    {
        Function *function = f.function;

        Utf8String RT = pxTypeToCType(function->returnType);
        Utf8String argsText;
        int a = 0, end = function->parameters.size();
        for (const Variable *arg : function->parameters)
        {
            Utf8String argType = pxTypeToCType(arg->type);
            argsText += argType + " " + arg->name;
            if(++a < end) {
                argsText += ", ";
            }
        }

        Function *prevFunction = currentFunction;
        currentFunction = function;

        Utf8String *blockCode = (Utf8String*) f.block->accept(*this);

        currentFunction = prevFunction;

        return new Utf8String{ RT + " " + function->name + "(" + argsText + ")" + "\n" + *blockCode };;
    }

    void* CCompiler::visit(ast::IfStatement & i)
    {
        Utf8String *condition = (Utf8String*) i.condition->accept(*this);

        Utf8String *trueCode = (Utf8String*) i.trueStatement->accept(*this);

        Utf8String elseStmt;
        if (i.elseStatement) {
            Utf8String *elseCode = (Utf8String*) i.elseStatement->accept(*this);
            elseStmt = Utf8String{"else\n"} + *elseCode + "\n";
        }
        return new Utf8String{ Utf8String{"if ("} + *condition + ")\n" + *trueCode + "\n" + elseStmt};
    }

    void* CCompiler::visit(ast::IntegerLiteral &i)
    {
        return new Utf8String{ std::to_string(i.value) };
    }

    void * CCompiler::visit(ast::Module & m)
    {
        auto current = currentScope;
        currentScope = scopeTree->enterScope();

        Utf8String moduleCode = generateIncludes();

        Utf8String statementCode;
        for (auto const& statement : m.statements)
        {
            statementCode += *(Utf8String *) statement->accept(*this);
        }

        scopeTree->endScope();
        currentScope = current;

        moduleCode += toPreDeclare;
        moduleCode += statementCode;
        return new Utf8String{ moduleCode };
    }

    void* CCompiler::visit(ast::ReturnStatement &s)
    {
        if (s.returnValue != nullptr)
        {
            Utf8String *result = (Utf8String*) s.returnValue->accept(*this);
            return new Utf8String{ Utf8String{"return "} + *result + ";\n" };
        }
        else
            return new Utf8String{ "return;\n" };
    }


    void* CCompiler::visit(ast::StringLiteral &s)
    {
        return new Utf8String{ Utf8String{"u8\""} + s.literal + Utf8String{"\""} };
    }

    void* CCompiler::visit(ast::TernaryOpExpression &t)
    {
        Utf8String *condition = (Utf8String*) t.condition->accept(*this);
        Utf8String *trueExpr = (Utf8String*) t.trueExpr->accept(*this);
        Utf8String *falseExpr = (Utf8String*) t.falseExpr->accept(*this);
        return new Utf8String{ *condition + " ? " + *trueExpr + " : " + *falseExpr };
    }

    void* CCompiler::visit(ast::UnaryOpExpression &e)
    {
        Utf8String *value = (Utf8String*) e.expression->accept(*this);
        Utf8String opToken;
        if (e.type->isInt() || e.type->isUInt())
        {
            switch (e.op)
            {
                case ast::UnaryOperator::NEG:
                    opToken = "-";
                    break;
                case ast::UnaryOperator::CMPL:
                    opToken = "~";
                    break;
                default:
                    return nullptr;
            }
        }
        else if (e.type->isFloat())
        {
            switch (e.op)
            {
                case ast::UnaryOperator::NEG:
                    opToken = "-";
                    break;
                default:
                    return nullptr;
            }
        }
        else {
            return nullptr;
        }
        return new Utf8String{ opToken + *value };
    }

    void* CCompiler::visit(ast::VariableDeclaration &v)
    {
        auto symbolTable = currentScope->symbols();
        auto pxType = symbolTable->getType(v.typeName);
        Utf8String cTypeName = pxTypeToCType(pxType);
        Utf8String declaration(cTypeName + " " + v.name);
        if (v.initialValue != nullptr) {
            Utf8String *value = (Utf8String *) v.initialValue->accept(*this);
            declaration += Token::getTokenName(TokenType::OP_ASSIGN) + *value;
        }
        return new Utf8String{ declaration + Token::getTokenName(TokenType::OP_END_STATEMENT) + "\n" };
    }

    void* CCompiler::visit(ast::VariableExpression &v)
    {
        return new Utf8String{ v.variable };
    }

    Utf8String CCompiler::generateIncludes() {
        return "#include <stdint.h>\n#include <stdbool.h>\n\n";
    }

    Utf8String CCompiler::buildFunctionProto(Function *function) {
        Utf8String RT = pxTypeToCType(function->returnType);
        Utf8String argsText;
        int a = 0, end = function->parameters.size();
        for (const Variable *arg : function->parameters)
        {
            Utf8String argType = pxTypeToCType(arg->type);
            argsText += argType + " " + arg->name;
            if(++a < end)
            {
                argsText += ", ";
            }
        }
        return RT + " " + function->name + "(" + argsText + ");\n";
    }
}
