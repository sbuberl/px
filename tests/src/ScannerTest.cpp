#include <cassert>
#include <iostream>
#include "catch.hpp"
#include <Scanner.h>

TEST_CASE("Scanner position") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "1.23 + 1263");
    auto position = scanner.position();
    REQUIRE(position.fileName == name);
    REQUIRE(position.fileOffset == 0);
    REQUIRE(position.line == 1);
    REQUIRE(position.lineColumn == 1);
}

TEST_CASE("Scanner nextToken") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "1.23 + 1263");
    auto token = scanner.nextToken();
    px::SourcePosition position = scanner.position();
    REQUIRE(token.position == position);
    REQUIRE(token.type == px::TokenType::FLOAT);
    REQUIRE(token.str == "1.23");
    REQUIRE(token.suffixType == nullptr);
    REQUIRE(token.integerBase == 10);

    REQUIRE(position.fileName == name);
    REQUIRE(position.fileOffset == 0);
    REQUIRE(position.line == 1);
    REQUIRE(position.lineColumn == 1);
}

TEST_CASE("Scanner accept") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "1.23 + 1263");
    auto token = scanner.nextToken();
    bool passed = scanner.accept();
    px::SourcePosition position = scanner.position();
    REQUIRE(passed == true);
    REQUIRE(position.fileName == name);
    REQUIRE(position.fileOffset == 4);
    REQUIRE(position.line == 1);
    REQUIRE(position.lineColumn == 5);
}

TEST_CASE("Scanner accept type failed") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "1.23 + 1263");
    auto token = scanner.nextToken();
    bool passed = scanner.accept(px::TokenType::INTEGER);
    px::SourcePosition position = scanner.position();
    REQUIRE(passed == false);
    REQUIRE(position.fileName == name);
    REQUIRE(position.fileOffset == 0);
    REQUIRE(position.line == 1);
    REQUIRE(position.lineColumn == 1);
}

TEST_CASE("Scanner accept type") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "1.23 + 1263");
    auto token = scanner.nextToken();
    bool passed = scanner.accept(px::TokenType::FLOAT);
    px::SourcePosition position = scanner.position();
    REQUIRE(passed == true);
    REQUIRE(position.fileName == name);
    REQUIRE(position.fileOffset == 4);
    REQUIRE(position.line == 1);
    REQUIRE(position.lineColumn == 5);
}

TEST_CASE("Scanner accept text failed") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "1.23 + 1263");
    auto token = scanner.nextToken();
    bool passed = scanner.accept("1263");
    px::SourcePosition position = scanner.position();
    REQUIRE(passed == false);
    REQUIRE(position.fileName == name);
    REQUIRE(position.fileOffset == 0);
    REQUIRE(position.line == 1);
    REQUIRE(position.lineColumn == 1);
}

TEST_CASE("Scanner accept text") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "1.23 + 1263");
    auto token = scanner.nextToken();
    bool passed = scanner.accept("1.23");
    px::SourcePosition position = scanner.position();
    REQUIRE(passed == true);
    REQUIRE(position.fileName == name);
    REQUIRE(position.fileOffset == 4);
    REQUIRE(position.line == 1);
    REQUIRE(position.lineColumn == 5);
}

TEST_CASE("Scanner rewind") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "1.23 + 1263");
    auto token = scanner.nextToken();
    scanner.rewind();
    px::SourcePosition position = scanner.position();
    REQUIRE(position.fileName == name);
    REQUIRE(position.fileOffset == 0);
    REQUIRE(position.line == 1);
    REQUIRE(position.lineColumn == 1);
}

TEST_CASE("Scanner end of file") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "   ");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::END_FILE);
}

TEST_CASE("Scanner identifier") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "myName");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::IDENTIFIER);
}

TEST_CASE("Scanner int literal") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "112345");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::INTEGER);
}

TEST_CASE("Scanner float literal") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "112.896748");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::FLOAT);
}
TEST_CASE("Scanner char literal") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "'\\u263A'");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::CHAR);
}

TEST_CASE("Scanner string literal") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "\"こんにち\\u263Aは世界\"");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::STRING);
}

TEST_CASE("Scanner keyword abstract") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "abstract");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_ABSTRACT);
}

TEST_CASE("Scanner keyword break") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "break");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_BREAK);
}

TEST_CASE("Scanner keyword case") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "case");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_CASE);
}

TEST_CASE("Scanner keyword concept") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "concept");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_CONCEPT);
}

TEST_CASE("Scanner keyword continue") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "continue");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_CONTINUE);
}

TEST_CASE("Scanner keyword default") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "default");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_DEFAULT);
}

TEST_CASE("Scanner keyword do") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "do");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_DO);
}

TEST_CASE("Scanner keyword else") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "else");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_ELSE);
}

TEST_CASE("Scanner keyword extern") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "extern");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_EXTERN);
}

TEST_CASE("Scanner keyword false") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "false");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_FALSE);
}

TEST_CASE("Scanner keyword for") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "for");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_FOR);
}

TEST_CASE("Scanner keyword func") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "func");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_FUNC);
}

TEST_CASE("Scanner keyword if") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "if");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_IF);
}

TEST_CASE("Scanner keyword implementation") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "implementation");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_IMPLEMENTATION);
}

TEST_CASE("Scanner keyword interface") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "interface");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_INTERFACE);
}

TEST_CASE("Scanner keyword new") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "new");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_NEW);
}

TEST_CASE("Scanner keyword private") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "private");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_PRIVATE);
}

TEST_CASE("Scanner keyword protected") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "protected");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_PROTECTED);
}

TEST_CASE("Scanner keyword public") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "public");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_PUBLIC);
}

TEST_CASE("Scanner keyword ref") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "ref");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_REF);
}

TEST_CASE("Scanner keyword return") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "return");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_RETURN);
}

TEST_CASE("Scanner keyword state") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "state");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_STATE);
}

TEST_CASE("Scanner keyword switch") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "switch");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_SWITCH);
}

TEST_CASE("Scanner keyword true") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "true");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_TRUE);
}

TEST_CASE("Scanner keyword value") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "value");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_VALUE);
}

TEST_CASE("Scanner keyword while") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "while");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::KW_WHILE);
}

TEST_CASE("Scanner operator left paren") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "(");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::LPAREN);
}

TEST_CASE("Scanner operator right paren") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, ")");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::RPAREN);
}

TEST_CASE("Scanner operator left bracket") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "{");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::LBRACKET);
}

TEST_CASE("Scanner operator right bracket") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "}");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::RBRACKET);
}

TEST_CASE("Scanner operator add") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "+");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_ADD);
}

TEST_CASE("Scanner operator and") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "&&");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_AND);
}

TEST_CASE("Scanner operator unicode and") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "∧");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_AND);
}

TEST_CASE("Scanner operator assign") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "=");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_ASSIGN);
}

TEST_CASE("Scanner operator bit and") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "&");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_BIT_AND);
}

TEST_CASE("Scanner operator bit and unicode") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "∩");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_BIT_AND);
}

TEST_CASE("Scanner operator bit or") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "|");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_BIT_OR);
}

TEST_CASE("Scanner operator bit or unicode") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "∪");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_BIT_OR);
}

TEST_CASE("Scanner operator bit xor") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "^");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_BIT_XOR);
}

TEST_CASE("Scanner operator colon") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, ":");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_COLON);
}

TEST_CASE("Scanner operator comma") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, ",");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_COMMA);
}

TEST_CASE("Scanner operator compl") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "~");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_COMPL);
}

TEST_CASE("Scanner operator div") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "/");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_DIV);
}

TEST_CASE("Scanner operator div unicode 1") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "÷");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_DIV);
}

TEST_CASE("Scanner operator div unicode 2") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "⁄");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_DIV);
}

TEST_CASE("Scanner operator dot") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, ".");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_DOT);
}

TEST_CASE("Scanner operator end statement") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, ";");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_END_STATEMENT);
}

TEST_CASE("Scanner operator equals") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "==");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_EQUALS);
}

TEST_CASE("Scanner operator greater") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, ">");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_GREATER);
}

TEST_CASE("Scanner operator greater or equal") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, ">=");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_GREATER_OR_EQUAL);
}

TEST_CASE("Scanner operator greater or equal unicode") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "≥");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_GREATER_OR_EQUAL);
}

TEST_CASE("Scanner operator left shift") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "<<");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_LEFT_SHIFT);
}

TEST_CASE("Scanner operator left shift unicode") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "≪");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_LEFT_SHIFT);
}

TEST_CASE("Scanner operator less") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "<");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_LESS);
}

TEST_CASE("Scanner operator less or equal") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "<=");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_LESS_OR_EQUAL);
}

TEST_CASE("Scanner operator less or equal unicode") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "≤");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_LESS_OR_EQUAL);
}

TEST_CASE("Scanner mod") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "%");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_MOD);
}

TEST_CASE("Scanner not") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "!");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_NOT);
}

TEST_CASE("Scanner not unicode") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "¬");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_NOT);
}

TEST_CASE("Scanner not equal") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "!=");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_NOT_EQUAL);
}

TEST_CASE("Scanner not equal unicode") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "≠");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_NOT_EQUAL);
}

TEST_CASE("Scanner or") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "||");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_OR);
}

TEST_CASE("Scanner or unicode") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "∨");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_OR);
}

TEST_CASE("Scanner question") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "?");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_QUESTION);
}

TEST_CASE("Scanner right shift") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, ">>");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_RIGHT_SHIFT);
}

TEST_CASE("Scanner right shift unicode") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "≫");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_RIGHT_SHIFT);
}
TEST_CASE("Scanner star") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "*");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_STAR);
}

TEST_CASE("Scanner star unicode") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "×");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_STAR);
}

TEST_CASE("Scanner subtract") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "-");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_SUB);
}

TEST_CASE("Scanner subtract unicode") {
    px::Utf8String name{"myModule.px"};
    px::Scanner scanner(name, "−");
    auto token = scanner.nextToken();
    REQUIRE(token.type == px::TokenType::OP_SUB);
}
