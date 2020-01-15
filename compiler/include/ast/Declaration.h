
#ifndef _PX_AST_DECLARATION_H_
#define _PX_AST_DECLARATION_H_

#include <ast/AST.h>
#include <ast/Statement.h>

namespace px
{
    namespace ast
    {
        class Parameter
        {
        public:
            const Utf8String name;
            const Utf8String typeName;

            Parameter(const Utf8String &func, const Utf8String &ty)
                : name{ func }, typeName{ ty }
            {
            }
        };

        class FunctionPrototype
        {
        public:
            const Utf8String name;
            const Utf8String returnTypeName;
            std::vector<Parameter> parameters;
            bool isExtern;

            FunctionPrototype(const Utf8String &fname, const Utf8String &retTypeName, const std::vector<Parameter> &params, bool ext)
                : name{ fname }, returnTypeName{ retTypeName }, parameters{ params }, isExtern{ ext }
            {
            }
        };

        class FunctionDeclaration : public Statement
        {
        public:
            std::unique_ptr<FunctionPrototype> prototype;
            Function *function;

            FunctionDeclaration(const SourcePosition &pos, std::unique_ptr<FunctionPrototype> proto)
               : Statement{ NodeType::DECLARE_FUNC, pos }, prototype{ std::move(proto) }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class FunctionDefinition : public Statement
        {
        public:
            std::unique_ptr<FunctionPrototype> prototype;
            std::unique_ptr<BlockStatement> block;
            Function *function;

            FunctionDefinition(const SourcePosition &pos, std::unique_ptr<FunctionPrototype> proto, std::unique_ptr<BlockStatement> stmts)
                : Statement{ NodeType::DECLARE_FUNC_BODY, pos }, prototype{ std::move(proto) }, block{ std::move(stmts) }, function{ }
            {
            }

            void *accept(Visitor &visitor) override;
        };

        class VariableDeclaration : public Statement
        {
        public:
            const Utf8String typeName;
            const Utf8String name;
            std::unique_ptr<Expression> initialValue;
            int64_t *arraySize;

            VariableDeclaration(const SourcePosition &pos, const Utf8String &t, const Utf8String &n, std::unique_ptr<Expression> value,  int64_t *array)
                : Statement{ NodeType::DECLARE_VAR, pos }, typeName{ t }, name{ n }, initialValue{ std::move(value) }, arraySize{ array }
            {
            }

            void *accept(Visitor &visitor) override;
        };
    }
}

#endif
