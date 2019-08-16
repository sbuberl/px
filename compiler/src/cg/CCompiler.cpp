#include "cg/CCompiler.h"

#include <unicode/ustdio.h>
#include <unicode/ustring.h>

#include <functional>
#include <iostream>
#include <Token.h>

namespace px
{

    CCompiler::CCompiler(ScopeTree *tree) : currentFunction{ nullptr }, scopeTree{ tree }, indentLevel{}
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

    void CCompiler::indent()
    {
        ++indentLevel;
    }

    void CCompiler::indent(ast::AST *node)
    {
        if(dynamic_cast<ast::BlockStatement*>(node) == nullptr)
        {
            indent();
        }
    }

    void CCompiler::unindent()
    {
        --indentLevel;
    }

    void CCompiler::unindent(ast::AST *node)
    {
        if(dynamic_cast<ast::BlockStatement*>(node) == nullptr)
        {
            unindent();
        }
    }

    void CCompiler::newLine()
    {
        code += "\n";
        code += std::string(indentLevel * 4, ' ');
    }

    void CCompiler::add(const Utf8String &text)
    {
        code += text;
    }

    void CCompiler::compile(ast::AST& ast)
    {
        ast.accept(*this);

        UFILE *out = u_get_stdout();
        int32_t length;
        UErrorCode errorCode = U_ZERO_ERROR;
        u_strFromUTF8(NULL, 0, &length, code.c_str(), code.byteLength(), &errorCode);
        errorCode = U_ZERO_ERROR;
        std::unique_ptr<UChar[]> utf16Message(new UChar[length + 2]);
        u_strFromUTF8(utf16Message.get(), length, &length, code.c_str(), code.byteLength(), &errorCode);
        utf16Message[length] = '\r';
        utf16Message[length + 1] = '\n';
        u_file_write(utf16Message.get(), length + 2, out);
    }

    void* CCompiler::visit(ast::AssignmentStatement &a)
    {
        auto symbolTable = currentScope->symbols();
        Variable *variable = symbolTable->getVariable(a.variableName);
        add(Utf8String{ variable->name + Token::getTokenName(TokenType::OP_ASSIGN)});
        a.expression->accept(*this);
        add(Token::getTokenName(TokenType::OP_END_STATEMENT));
    }

    void* CCompiler::visit(ast::BinaryOpExpression &b)
    {
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

        add(Utf8String{"("});
        b.left->accept(*this);
        add( Utf8String{" "} + opToken + " ");
        b.right->accept(*this);
        add( Utf8String{")"});
        return nullptr;
    }

    void* CCompiler::visit(ast::BlockStatement &s)
    {
        auto current = currentScope;
        currentScope = scopeTree->enterScope();

        add(Utf8String{"{"} );
        indent();
        for (auto const& statement : s.statements)
        {
            newLine();
            statement->accept(*this);

        }
        unindent();
        newLine();
        add(Utf8String{ "}" });
        scopeTree->endScope();
        currentScope = current;

        return nullptr;
    }

    void* CCompiler::visit(ast::BoolLiteral &b)
    {
        add( b.literal );
    }

    void* CCompiler::visit(ast::CastExpression &e) {
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
            add(Utf8String{"("} + newTypeName + ") " );
            e.expression->accept(*this);
        }
        return nullptr;
    }

    void* CCompiler::visit(ast::CharLiteral &c)
    {
        add(Utf8String{"'"} + c.literal + "'" );
        return nullptr;
    }

    void* CCompiler::visit(ast::ExpressionStatement &e)
    {
        e.expression->accept(*this);
        add(Token::getTokenName(TokenType::OP_END_STATEMENT) );
        return nullptr;
    }

    void* CCompiler::visit(ast::FloatLiteral &f)
    {
        add( f.literal );
        return nullptr;
    }

    void* CCompiler::visit(ast::FunctionCallExpression &f)
    {
        Function *pxFunction = f.function;
        Utf8String name = pxFunction->name;
        int a = 0, end = f.arguments.size();

        add(Utf8String{ name + "("});
        for (auto &arg : f.arguments)
        {
            arg->accept(*this);
            if(++a < end) {
                add(", ");
            }
        }

        add(Utf8String{")"});
        return nullptr;
    }

    void* CCompiler::visit(ast::FunctionDeclaration & e)
    {
        add(buildFunctionProto(e.function));
        return nullptr;
    }

    void* CCompiler::visit(ast::FunctionDefinition &f)
    {
        Function *function = f.function;
        Utf8String RT = pxTypeToCType(function->returnType);
        int a = 0, end = function->parameters.size();

        add(Utf8String{ RT + " " + function->name + "("});
        for (const Variable *arg : function->parameters)
        {
            Utf8String argType = pxTypeToCType(arg->type);
            add(argType + " " + arg->name);
            if(++a < end) {
                add(", ");
            }
        }

        add(Utf8String{ ")"});
        Function *prevFunction = currentFunction;
        currentFunction = function;

        newLine();
        f.block->accept(*this);

        currentFunction = prevFunction;
        return nullptr;
    }

    void* CCompiler::visit(ast::IfStatement & i)
    {
        add(Utf8String{"if ("} );
        i.condition->accept(*this);
        add(Utf8String{")"} );
        newLine();

        indent(i.trueStatement.get());

        i.trueStatement->accept(*this);

        unindent(i.trueStatement.get());

        if (i.elseStatement) {
            newLine();
            add(Utf8String{"else"});
            indent(i.elseStatement.get());
            newLine();
            i.elseStatement->accept(*this);
            unindent(i.elseStatement.get());
        }
        return nullptr;
    }

    void* CCompiler::visit(ast::IntegerLiteral &i)
    {
        add(Utf8String{ std::to_string(i.value) });
    }

    void * CCompiler::visit(ast::Module & m)
    {
        auto current = currentScope;
        currentScope = scopeTree->enterScope();

        code = generateIncludes();
        add(toPreDeclare);
        add(generateStringDecl());

        Utf8String statementCode;
        for (auto const& statement : m.statements)
        {
            statement->accept(*this);
            newLine();
        }

        scopeTree->endScope();
        currentScope = current;

        return nullptr;
    }

    void* CCompiler::visit(ast::ReturnStatement &s)
    {
        if (s.returnValue != nullptr)
        {
            add(Utf8String{"return "});
            s.returnValue->accept(*this);
        }
        else
            add(Utf8String{ "return"});
        add(Token::getTokenName(TokenType::OP_END_STATEMENT) );
    }

    void* CCompiler::visit(ast::StringLiteral &s)
    {
        Utf8String literal = s.literal;
        add( Utf8String{"{ "} + "u8\"" + literal + "\", " + std::to_string(literal.length())  + ", " +  std::to_string(literal.byteLength()) + " }");
        return nullptr;
    }

    void* CCompiler::visit(ast::TernaryOpExpression &t)
    {
        t.condition->accept(*this);
        add(Utf8String{" ? " });
        Utf8String *trueExpr = (Utf8String*) t.trueExpr->accept(*this);
        add(Utf8String{" : " });
        Utf8String *falseExpr = (Utf8String*) t.falseExpr->accept(*this);
        return nullptr;
    }

    void* CCompiler::visit(ast::UnaryOpExpression &e)
    {
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
        else if (e.type->isBool())
        {
            switch (e.op)
            {
                case ast::UnaryOperator::NOT:
                    opToken = "!";
                    break;
                default:
                    return nullptr;
            }
        }
        else {
            return nullptr;
        }

        add( Utf8String{ opToken  });
        e.expression->accept(*this);
        return nullptr;
    }

    void* CCompiler::visit(ast::VariableDeclaration &v)
    {
        auto symbolTable = currentScope->symbols();
        auto pxType = symbolTable->getType(v.typeName);
        Utf8String cTypeName = pxTypeToCType(pxType);
        add(cTypeName + " " + v.name);
        if (v.initialValue != nullptr) {
            add(Token::getTokenName(TokenType::OP_ASSIGN));
            v.initialValue->accept(*this);
        }
        add(Token::getTokenName(TokenType::OP_END_STATEMENT));
    }

    void* CCompiler::visit(ast::VariableExpression &v)
    {
        add( v.variable );
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
        Utf8String flags;
        if(function->isExtern)
        {
            flags += "extern ";
        }
        return flags + RT + " " + function->name + "(" + argsText + ");\n";
    }

    Utf8String CCompiler::generateStringDecl() {
        return "typedef struct _PxString\n{int8_t *bytes;\nintptr_t length;\nintptr_t byteLength;\n} PxString;\n\n";
    }
}
