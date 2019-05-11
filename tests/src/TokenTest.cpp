#include <cassert>
#include <iostream>
#include "catch.hpp"
#include <Token.h>

TEST_CASE("Token pos constructor") {
    px::Utf8String name{"myModule.px"};
    px::SourcePosition position(name);
    px::Token token(position);

    REQUIRE(token.position == position);
    REQUIRE(token.type == px::TokenType::BAD);
    REQUIRE(token.str == "");
    REQUIRE(token.suffixType == nullptr);
    REQUIRE(token.integerBase == 10);
}

TEST_CASE("Token token constructor") {
    px::Utf8String name{"myModule.px"};
    px::SourcePosition position(name);
    px::Token token(position, px::TokenType::INTEGER, "76587");

    REQUIRE(token.position == position);
    REQUIRE(token.type == px::TokenType::INTEGER);
    REQUIRE(token.str == "76587");
    REQUIRE(token.suffixType == nullptr);
    REQUIRE(token.integerBase == 10);
}


TEST_CASE("Token clear") {
    px::Utf8String name{"myModule.px"};
    px::SourcePosition position(name);
    px::Token token(position, px::TokenType::INTEGER, "76587");
    token.clear();

    REQUIRE(token.position == position);
    REQUIRE(token.type == px::TokenType::BAD);
    REQUIRE(token.str == "");
    REQUIRE(token.suffixType == nullptr);
    REQUIRE(token.integerBase == 10);
}

TEST_CASE("Token getTokenName") {
    auto literalStringName = px::Token::getTokenName(px::TokenType::STRING);
    auto andName = px::Token::getTokenName(px::TokenType::OP_ADD);
    auto kwAsName = px::Token::getTokenName(px::TokenType::KW_AS);

    REQUIRE(literalStringName == "string literal");
    REQUIRE(andName == "+");
    REQUIRE(kwAsName == "as");
}
