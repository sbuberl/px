
#include "Scanner.h"

#include <unicode/uchar.h>

#define RETURN_OP(tok, length) \
do { \
    token = current; \
    peekPos.location.advance(length);	\
    return TokenType:: tok ; \
} while( false )

namespace px {

    std::unordered_map<Utf8String, TokenType> Scanner::keywords = {
        { "abstract", TokenType::KW_ABSTRACT},
        { "as", TokenType::KW_AS},
        { "break", TokenType::KW_BREAK},
        { "case", TokenType::KW_CASE},
        { "concept", TokenType::KW_CONCEPT},
        { "continue", TokenType::KW_CONTINUE},
        { "default", TokenType::KW_DEFAULT},
        { "do", TokenType::KW_DO},
        { "else", TokenType::KW_ELSE},
        { "false", TokenType::KW_FALSE},
        { "for", TokenType::KW_FOR},
        { "if", TokenType::KW_IF},
        { "implementation", TokenType::KW_IMPLEMENTATION},
        { "interface", TokenType::KW_INTERFACE},
        { "new", TokenType::KW_NEW},
        { "private", TokenType::KW_PRIVATE},
        { "protected", TokenType::KW_PROTECTED},
        { "public", TokenType::KW_PUBLIC},
        { "ref", TokenType::KW_REF},
        { "return", TokenType::KW_RETURN},
        { "state", TokenType::KW_STATE},
        { "switch", TokenType::KW_SWITCH},
        { "true", TokenType::KW_TRUE},
        { "value", TokenType::KW_VALUE},
        { "while", TokenType::KW_WHILE},
    };

    Scanner::Scanner(const Utf8String &source) : length{ source.length() }, peekPos{ source }, currentPos{ source }
    {
        this->source = source;
    }

    bool Scanner::accept()
    {
        currentPos = peekPos;
        return true;
    }

    bool Scanner::accept(TokenType type)
    {
        if (peekPos.token.type == type)
        {
            accept();
            return true;
        }
        else
            return false;
    }

    bool Scanner::accept(const Utf8String &str)
    {
        if (peekPos.token.str == str)
        {
            accept();
            return true;
        }
        else
            return false;
    }

    void Scanner::rewind()
    {
        peekPos = currentPos;
    }

    Token &Scanner::nextToken()
    {
        Token &token = peekPos.token;
        token.clear();
        token.type = scan();
        return token;
    }

    int32_t Scanner::nextCharacter()
    {
        peekPos.location.advance();
        return source[peekPos.location.fileOffset];
    }

    TokenType Scanner::scan()
    {
        int32_t current;
        Utf8String &token = peekPos.token.str;

        if (peekPos.location.fileOffset >= length)
            return TokenType::END_FILE;

        current = source[peekPos.location.fileOffset];
        while (u_isWhitespace(current))
        {
            if (current == '\n')
            {
                peekPos.location.nextLine();
            }
            current = nextCharacter();

        }

        if (u_isdigit(current))
        {
            int currentDigit = u_digit(current, 10);
            if (currentDigit == 0)
            {
                int32_t next = nextCharacter();
                switch (next)
                {
                    case 'x':
                        current = nextCharacter();
                        while (u_isxdigit(current))
                        {
                            token += current;
                            current = nextCharacter();
                        }
                        return TokenType::HEX_INT;
                    case 'b':
                        current = nextCharacter();
                        currentDigit = u_digit(current, 10);
                        while (currentDigit == 0 || currentDigit == 1)
                        {
                            token += current;
                            current = nextCharacter();
                            currentDigit = u_digit(current, 10);
                        }
                        return TokenType::BINARY_INT;
                    case 'o':
                        current = nextCharacter();
                        currentDigit = u_digit(current, 10);
                        while (currentDigit >= 0 && currentDigit <= 7)
                        {
                            token += current;
                            current = nextCharacter();
                            currentDigit = u_digit(current, 10);
                        }
                        return TokenType::OCTAL_INT;
                    default:
                        token += current;
                        current = next;
                        break;
                }
            }

            while (u_isdigit(current))
            {
                token += current;
                current = nextCharacter();
            }

            bool isFloat = false;
            if (current == '.')
            {
                do
                {
                    token += current;
                    current = nextCharacter();
                } while (u_isdigit(current));

                isFloat = true;
            }

            if(current == '_')
            {
                current = nextCharacter();
                switch (current)
                {
                    case 'i':
                        current = nextCharacter();
                        currentDigit = u_digit(current, 10);
                        switch (currentDigit)
                        {
                            case 8:
                                peekPos.token.suffixType = Type::INT8;
                                current = nextCharacter();
                                break;
                            case 1:
                                current = nextCharacter();
                                currentDigit = u_digit(current, 10);
                                if (currentDigit == 6)
                                {
                                    peekPos.token.suffixType = Type::INT16;
                                    current = nextCharacter();
                                }
                                break;
                            case 3:
                                current = nextCharacter();
                                currentDigit = u_digit(current, 10);
                                if (currentDigit == 2)
                                {
                                    peekPos.token.suffixType = Type::INT32;
                                    current = nextCharacter();
                                }
                                break;
                            case 6:
                                current = nextCharacter();
                                currentDigit = u_digit(current, 10);
                                if (currentDigit == 4)
                                {
                                    peekPos.token.suffixType = Type::INT64;
                                    current = nextCharacter();
                                }
                                break;
                        }
                        break;
                    case 'u':
                        current = nextCharacter();
                        currentDigit = u_digit(current, 10);
                        switch (currentDigit)
                        {
                            case 8:
                                peekPos.token.suffixType = Type::UINT8;
                                current = nextCharacter();
                                break;
                            case 1:
                                current = nextCharacter();
                                currentDigit = u_digit(current, 10);
                                if (currentDigit == 6)
                                {
                                    peekPos.token.suffixType = Type::UINT16;
                                    current = nextCharacter();
                                }
                                break;
                            case 3:
                                current = nextCharacter();
                                currentDigit = u_digit(current, 10);
                                if (currentDigit == 2)
                                {
                                    peekPos.token.suffixType = Type::UINT32;
                                    current = nextCharacter();
                                }
                                break;
                            case 6:
                                current = nextCharacter();
                                currentDigit = u_digit(current, 10);
                                if (currentDigit == 4)
                                {
                                    peekPos.token.suffixType = Type::UINT64;
                                    current = nextCharacter();
                                }
                                break;
                        }
                        break;
                    case 'f':
                        current = nextCharacter();
                        currentDigit = u_digit(current, 10);
                        switch (currentDigit)
                        {
                            case 3:
                                current = nextCharacter();
                                currentDigit = u_digit(current, 10);
                                if (currentDigit == 2)
                                {
                                    peekPos.token.suffixType = Type::FLOAT32;
                                    current = nextCharacter();
                                }
                                break;
                            case 6:
                                current = nextCharacter();
                                currentDigit = u_digit(current, 10);
                                if (currentDigit == 4)
                                {
                                    peekPos.token.suffixType = Type::FLOAT64;
                                    current = nextCharacter();
                                }
                                break;
                        }
                        break;
                }
            }
            return isFloat ? TokenType::FLOAT : TokenType::INTEGER;
        }
        else if (u_isalpha(current))
        {
            do
            {
                token += current;
                current = nextCharacter();
            } while (u_isalnum(current) || current == '_');


            auto it = keywords.find(token);
            if (it != keywords.end())
            {
                return it->second;
            }

            return TokenType::IDENTIFIER;
        }
        else if (current == '\'')
        {
            current = nextCharacter();
            if (current != '\'')
            {
                if (current == '\\')
                {
                    scanCharEscape(token);
                }
                else
                {
                    token += current;
                    current = nextCharacter();
                }
            }

            nextCharacter();

            return TokenType::CHAR;
        }
        else if (current == '"')
        {
            current = nextCharacter();
            while (current != '"')
            {
                if (current == '\\')
                {
                    scanCharEscape(token);
                }
                else
                {
                    token += current;
                    current = nextCharacter();
                }
            }

            nextCharacter();

            return TokenType::STRING;
        }
        else
        {
            switch (current)
            {
                case '(':	RETURN_OP(LPAREN, 1);
                case ')':	RETURN_OP(RPAREN, 1);
                case '{':	RETURN_OP(LBRACKET, 1);
                case '}':	RETURN_OP(RBRACKET, 1);
                case ';':	RETURN_OP(OP_END_STATEMENT, 1);
                case ',':	RETURN_OP(OP_COMMA, 1);
                case '.':	RETURN_OP(OP_DOT, 1);
                case '+':	RETURN_OP(OP_ADD, 1);
                case '-':	RETURN_OP(OP_SUB, 1);
                case '*':	RETURN_OP(OP_STAR, 1);
                case '/':	RETURN_OP(OP_DIV, 1);
                case '%':	RETURN_OP(OP_MOD, 1);
                case '~':	RETURN_OP(OP_COMPL, 1);
                case '^':	RETURN_OP(OP_BIT_XOR, 1);
                case '?':	RETURN_OP(OP_QUESTION, 1);
                case ':':	RETURN_OP(OP_COLON, 1);
                case '=':
                {
                    char next = source[peekPos.location.fileOffset + 1];
                    if (next == '=')
                        RETURN_OP(OP_EQUALS, 2);
                    else
                        RETURN_OP(OP_ASSIGN, 1);
                }
                case '!':
                {
                    char next = source[peekPos.location.fileOffset + 1];
                    if (next == '=')
                        RETURN_OP(OP_NOT_EQUAL, 2);
                    else
                        RETURN_OP(OP_NOT, 1);
                }
                case '<':
                {
                    char next = source[peekPos.location.fileOffset + 1];
                    if (next == '<')
                        RETURN_OP(OP_LEFT_SHIFT, 2);
                    else if (next == '=')
                        RETURN_OP(OP_LESS_OR_EQUAL, 2);
                    else
                        RETURN_OP(OP_LESS, 1);
                }
                case '>':
                {
                    char next = source[peekPos.location.fileOffset + 1];
                    if (next == '>')
                        RETURN_OP(OP_RIGHT_SHIFT, 2);
                    else if (next == '=')
                        RETURN_OP(OP_GREATER_OR_EQUAL, 2);
                    else
                        RETURN_OP(OP_GREATER, 1);
                }
                case '&':
                {
                    char next = source[peekPos.location.fileOffset + 1];
                    if (next == '&')
                        RETURN_OP(OP_AND, 2);
                    else
                        RETURN_OP(OP_BIT_AND, 1);
                }
                case '|':
                {
                    char next = source[peekPos.location.fileOffset + 1];
                    if (next == '|')
                        RETURN_OP(OP_OR, 2);
                    else
                        RETURN_OP(OP_BIT_OR, 1);
                }
            }
        }

        return TokenType::BAD;
    }

    void Scanner::scanCharEscape(Utf8String &token)
    {
        int32_t current = nextCharacter();
        switch (current)
        {
            case '0':
                token += '\0';
                break;
            case 'a':
                token += '\a';
                break;
            case 'b':
                token += '\b';
                break;
            case 't':
                token += '\t';
                break;
            case 'n':
                token += '\n';
                break;
            case 'v':
                token += '\v';
                break;
            case 'f':
                token += '\f';
                break;
            case 'r':
                token += '\f';
                break;
            case '"':
                token += '"';
                break;
            case 'u':
                scanCharCodePoint(token, 4);
                break;
            case 'U':
                scanCharCodePoint(token, 8);
                break;
        }
    }

    void Scanner::scanCharCodePoint(Utf8String &token, unsigned int length)
    {
        Utf8String codePointHex;
        for (unsigned int i = 0; i < length; ++i)
        {
            int32_t current = nextCharacter();
            if (u_isxdigit(current))
            {
                codePointHex += current;
            }
        }
        int32_t codePoint = std::stoi(codePointHex.toString(), nullptr, 16);
        token += codePoint;
    }
}
