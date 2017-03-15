
#include "Scanner.h"
#include <cctype>

#define RETURN_OP(tok, length) \
do { \
    token = current; \
    peekPos.location.advance(length);	\
    return TokenType:: tok ; \
} while( false )

namespace px {

    std::unordered_map<std::string, TokenType> Scanner::keywords = {
        {"abstract", TokenType::KW_ABSTRACT},
        {"as", TokenType::KW_AS},
        {"break", TokenType::KW_BREAK},
        {"case", TokenType::KW_CASE},
        {"concept", TokenType::KW_CONCEPT},
        {"continue", TokenType::KW_CONTINUE},
        {"default", TokenType::KW_DEFAULT},
        {"do", TokenType::KW_DO},
        {"else", TokenType::KW_ELSE},
        {"false", TokenType::KW_FALSE},
        {"for", TokenType::KW_FOR},
        {"if", TokenType::KW_IF},
        {"implementation", TokenType::KW_IMPLEMENTATION},
        {"interface", TokenType::KW_INTERFACE},
        {"new", TokenType::KW_NEW},
        {"private", TokenType::KW_PRIVATE},
        {"protected", TokenType::KW_PROTECTED},
        {"public", TokenType::KW_PUBLIC},
        {"ref", TokenType::KW_REF},
        {"return", TokenType::KW_RETURN},
        {"state", TokenType::KW_STATE},
        {"switch", TokenType::KW_SWITCH},
        {"true", TokenType::KW_TRUE},
        {"value", TokenType::KW_VALUE},
        {"while", TokenType::KW_WHILE},
    };

    Scanner::Scanner(const std::string &source) : length(source.length())
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

    bool Scanner::accept(const std::string &str)
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

    char Scanner::nextCharacter()
    {
        peekPos.location.advance();
        return source[peekPos.location.fileOffset];
    }

    TokenType Scanner::scan()
    {
        char current;
        std::string &token = peekPos.token.str;

        if (peekPos.location.fileOffset >= length)
            return TokenType::END_FILE;

        current = source[peekPos.location.fileOffset];
        while (isspace(current))
        {
            if (current == '\n')
            {
                peekPos.location.nextLine();
            }
            current = nextCharacter();
        }

        if (isdigit(current))
        {
            if (current == '0')
            {
                char next = source[peekPos.location.fileOffset + 1];
                switch (next)
                {
                    case 'x':
                        nextCharacter();
                        current = nextCharacter();
                        while (std::isxdigit(current))
                        {
                            token.push_back(current);
                            current = nextCharacter();
                        }
                        return TokenType::HEX_INT;
                    case 'b':
                        nextCharacter();
                        current = nextCharacter();
                        while ( current == '0' || current == '1' )
                        {
                            token.push_back(current);
                            current = nextCharacter();
                        }
                        return TokenType::BINARY_INT;
                    case 'o':
                        nextCharacter();
                        current = nextCharacter();
                        while (current >= '0' && current <= '7')
                        {
                            token.push_back(current);
                            current = nextCharacter();
                        }
                        return TokenType::OCTAL_INT;
                    default:
                        break;
                }
            }

            do
            {
                token.push_back(current);
                current = nextCharacter();
            } while (isdigit(current));

            if (current == '.')
            {
                do
                {
                    token.push_back(current);
                    current = nextCharacter();
                } while (isdigit(current));

                return TokenType::FLOAT;
            }
            else
                return TokenType::INTEGER;
        }
        else if (isalpha(current))
        {
            do
            {
                token.push_back(current);
                current = nextCharacter();
            } while (isalnum(current) || current == '_');


            auto it = keywords.find(token);
            if (it != keywords.end())
            {
                return it->second;
            }

            return TokenType::IDENTIFIER;
        }
        else if (current == '"')
        {
            current = nextCharacter();
            while (current != '"')
            {
                token.push_back(current);
                current = nextCharacter();
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
                        RETURN_OP(OP_BIT_XOR, 1);
                }
            }
        }

        return TokenType::BAD;
    }
}
