
#include <cassert>
#include "catch.hpp"
#include <Utf8String.h>

TEST_CASE("Ut8fString supports empty constructors") {
    px::Utf8String text{};

    REQUIRE(text.byteLength() == 0);
    REQUIRE(text.length() == 0);
    REQUIRE(text.bytesEnd() == text.bytesEnd());
}

TEST_CASE("Ut8fString supports single point constructor") {
    char c = 'A';
    px::Utf8String text{c};

    REQUIRE(text.byteLength() == 1);
    REQUIRE(text.length() == 1);
}

TEST_CASE("Ut8fString supports c string constructor") {
    const char *str = "hello world";
    px::Utf8String text{ str };

    REQUIRE(text.byteLength() == 11);
    REQUIRE(text.length() == 11);
}

TEST_CASE("Ut8fString supports std string constructor") {
    std::string str = "こんにちは世界";
    px::Utf8String text{ str };

    REQUIRE(text.byteLength() == 21);
    REQUIRE(text.length() == 7);
}

TEST_CASE("Ut8fString supports copy constructor") {
    std::string str = "こんにちは世界";
    px::Utf8String text1{ str };
    px::Utf8String text2{ text1 };

    REQUIRE(text1.byteLength() == 21);
    REQUIRE(text1.length() == 7);
    REQUIRE(text2.byteLength() == 21);
    REQUIRE(text2.length() == 7);
}

TEST_CASE("Ut8fString supports move constructor") {
    std::string str = "こんにちは世界";
    px::Utf8String text1{ str };
    px::Utf8String text2{ std::move(text1) };

    REQUIRE(text1.byteLength() == 0);
    REQUIRE(text1.length() == 0);
    REQUIRE(text2.byteLength() == 21);
    REQUIRE(text2.length() == 7);
}