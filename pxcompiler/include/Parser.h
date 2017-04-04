
#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>
#include <ast/AST.h>
#include <ast/Expression.h>
#include <ast/Statement.h>
#include "Scanner.h"

namespace px {

    class Parser
    {
    public:
        Parser(SymbolTable *symbols);
        ~Parser() = default;
        ast::AST *parse(std::istream &in);

    private:
        Token currentToken;
        Scanner *scanner;
        SymbolTable * const symbols;

        void accept();
        bool accept(TokenType type);
        bool accept(const Utf8String &token);
        void expect(TokenType type);
        void expect(const Utf8String &token);
        void rewind();

        int getPrecedence(TokenType type);
        ast::BinaryOperator getBinaryOp(TokenType type);

        std::unique_ptr<ast::Statement> parseStatement();
        std::unique_ptr<ast::Statement> parseExpressionStatement();
        std::unique_ptr<ast::Statement> parseReturnStatement();
        std::unique_ptr<ast::Statement> parseVariableDeclaration();
        std::unique_ptr<ast::Expression> parseExpression();
        std::unique_ptr<ast::Expression> parseAssignment();
        std::unique_ptr<ast::Expression> parseBinary(int precedence);
        std::unique_ptr<ast::Expression> parseUnary();
        std::unique_ptr<ast::Expression> parseValue();
    };

}

#endif
