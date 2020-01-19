
#include <cassert>
#include <iostream>
#include "catch.hpp"
#include <Utf8String.h>

TEST_CASE("Ut8fString empty constructors") {
    px::Utf8String text{};

    REQUIRE(text.byteLength() == 0);
    REQUIRE(text.length() == 0);
    REQUIRE(text.bytesEnd() == text.bytesEnd());
}

TEST_CASE("Ut8fString single point constructor") {
    char c = 'A';
    px::Utf8String text{c};

    REQUIRE(text.byteLength() == 1);
    REQUIRE(text.length() == 1);
}

TEST_CASE("Ut8fString c string constructor") {
    const char *str = "hello world";
    px::Utf8String text{ str };

    REQUIRE(text.byteLength() == 11);
    REQUIRE(text.length() == 11);
}

TEST_CASE("Ut8fString std string constructor") {
    std::string str = "こんにちは世界";
    px::Utf8String text{ str };

    REQUIRE(text.byteLength() == 21);
    REQUIRE(text.length() == 7);
}

TEST_CASE("Ut8fString copy constructor") {
    std::string str = "こんにちは世界";
    px::Utf8String text1{ str };
    px::Utf8String text2{ text1 };

    REQUIRE(text1.byteLength() == 21);
    REQUIRE(text1.length() == 7);
    REQUIRE(text2.byteLength() == 21);
    REQUIRE(text2.length() == 7);
}

TEST_CASE("Ut8fString move constructor") {
    std::string str = "こんにちは世界";
    px::Utf8String text1{ str };
    px::Utf8String text2{ std::move(text1) };

    REQUIRE(text1.byteLength() == 0);
    REQUIRE(text1.length() == 0);
    REQUIRE(text2.byteLength() == 21);
    REQUIRE(text2.length() == 7);
}
/*
TEST_CASE("Ut8fString data") {
    std::string str = u8"こんにちは世界";

    std::cout << "Source:" << std::endl;
    for (auto iter1 = str.begin(); iter1 != str.end(); ++iter1)
        std::cout << *iter1 << std::endl;

    px::Utf8String text{ str };

    std::cout << "Ut8fString:" << std::endl;
    for (auto iter1 = text.bytesBegin(); iter1 != text.bytesEnd(); ++iter1)
        std::cout << *iter1 << std::endl;


    REQUIRE(std::equal(text.bytesBegin(), text.bytesEnd(), str.begin(), str.end()));
}*/

TEST_CASE("Ut8fString toString") {
    std::string str = u8"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο";
    px::Utf8String text1{ str };
    std::string str2 = text1.toString();

    REQUIRE(str == str2);
}

TEST_CASE("Ut8fString assign string") {
    std::string str = u8"Falsches Üben von Xylophonmusik quält jeden größeren Zwerg";
    px::Utf8String text1{ str };
    px::Utf8String text2;
    text2 = text1;
    REQUIRE(text1 == text2);
}

TEST_CASE("Ut8fString add string") {
    std::string hello = u8"こんにちは";
    std::string helloWorld = u8"こんにちは世界";
    px::Utf8String text1{ hello };
    px::Utf8String text2 = text1 + px::Utf8String{u8"世界"};
    REQUIRE(text2.toString() == helloWorld);
}

TEST_CASE("Ut8fString add assign codepoint") {
    std::string input = u8"世";
    std::string expected = u8"世界";
    px::Utf8String text{ input };
    text += 0x0000754C ;
    REQUIRE(text.toString() == expected);
}

TEST_CASE("Ut8fString add assign string") {
    std::string hello = u8"こんにちは";
    std::string helloWorld = u8"こんにちは世界";
    px::Utf8String text{hello};
    text += px::Utf8String{u8"世界"};
    REQUIRE(text.toString() == helloWorld);
}

TEST_CASE("Ut8fString index codepoint") {
    std::string input = u8"こんにちは世界";
    px::Utf8String text{ input };
    int32_t point = text[3];
    REQUIRE(point == 0x00003061);
}

TEST_CASE("Ut8fString set char at") {
    std::string input = u8"こんにちは世界";
    std::string expected = u8"こんAちは世界";
    px::Utf8String text{ input };
    px::Utf8String output{ expected };
    text.setCharAt(2, 'A');
    REQUIRE(text == output);
}

TEST_CASE("Ut8fString equals") {
    std::string input1 = u8"こんにちは世界";
    std::string input2 = u8"こんにちは世界";
    px::Utf8String text1{ input1 };
    px::Utf8String text2{ input2 };
    REQUIRE(text1 == text2);
}

TEST_CASE("Ut8fString not equal") {
    std::string input1 = u8"こんにちは世界";
    std::string input2 = u8"こんにち";
    px::Utf8String text1{ input1 };
    px::Utf8String text2{ input2 };
    REQUIRE(text1 != text2);
}

TEST_CASE("Ut8fString less than") {
    std::string input1 = u8"Falsches Üben von Xylophonmusik quält jeden größeren Zwerg";
    std::string input2 = u8"こんにちは世界";
    px::Utf8String text1{ input1 };
    px::Utf8String text2{ input2 };
    REQUIRE(text1 < text2);
}

TEST_CASE("Ut8fString greater than") {
    std::string input1 = u8"こんにちは世界";
    std::string input2 = u8"Falsches Üben von Xylophonmusik quält jeden größeren Zwerg";

    px::Utf8String text1{ input1 };
    px::Utf8String text2{ input2 };
    REQUIRE(text1 > text2);
}

TEST_CASE("Ut8fString clear") { ;
    std::string input = u8"Falsches Üben von Xylophonmusik quält jeden größeren Zwerg";
    px::Utf8String text{ input };
    text.clear();
    REQUIRE(text.byteLength() == 0);
    REQUIRE(text.length() == 0);
    REQUIRE(text.bytesEnd() == text.bytesEnd());
}

TEST_CASE("Ut8fString swap") {
    std::string input1 = u8"こんにちは世界";
    std::string input2 = u8"Falsches Üben von Xylophonmusik quält jeden größeren Zwerg";
    px::Utf8String text1{ input1 };
    px::Utf8String text2{ input2 };
    std::swap(text1, text2);

    REQUIRE(text1.toString() == input2);
    REQUIRE(text2.toString() == input1);
}

TEST_CASE("Ut8fString hash") {
    std::string input = u8"こんにちは";
    px::Utf8String text1{ input };
    px::Utf8String text2{ input };
    REQUIRE(text1.hash() == text2.hash());
}

TEST_CASE("Utf8Iterator") {
    std::string input = u8"こんにちは世界";
    px::Utf8String text{ input };
    px::Utf8Iterator iterator{ text };

    REQUIRE(*iterator == 12371);
    REQUIRE(iterator.hasNext());
    size_t index = 1;
    while(iterator.hasNext()) {
        iterator++;
        int32_t current = *iterator;
        int32_t textChar = text[index];
        REQUIRE(current == textChar);
        index++;
    }

    index--;
    while(iterator.hasPrevious()) {
        iterator--;
        index--;
        int32_t current = *iterator;
        int32_t textChar = text[index];
        REQUIRE(current == textChar);
    }

}
