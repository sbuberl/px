
#ifndef SCANNER_H_
#define SCANNER_H_

#include <string>
#include <unordered_map>

#include "SourcePosition.h"
#include "Token.h"
#include "Utf8String.h"

namespace px {

    class Scanner
    {
    public:
        Scanner(const Utf8String &source);
        ~Scanner() = default;
        bool accept();
        bool accept(TokenType type);
        bool accept(const Utf8String &token);
        void rewind();
        Token &nextToken();
        TokenType scan();

    private:
        struct Position
        {
            SourcePosition location;
            Token token;

            Position(const Utf8Iterator &iter)
            {
            }
        };

        int32_t nextCharacter();

        static std::unordered_map<Utf8String, TokenType> keywords;
        Utf8String source;
        const size_t length;
        Position currentPos;
        Position peekPos;

    };

}

#endif
