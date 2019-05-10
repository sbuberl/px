#include <cassert>
#include <iostream>
#include "catch.hpp"
#include <SourcePosition.h>

TEST_CASE("SourcePosition constructor") {
    px::Utf8String name{"myModule.px"};
    px::SourcePosition position(name);

    REQUIRE(position.fileName == name);
    REQUIRE(position.fileOffset == 0);
    REQUIRE(position.line == 1);
    REQUIRE(position.lineColumn == 1);
}

TEST_CASE("SourcePosition advance") {
    px::Utf8String name{"myModule.px"};
    px::SourcePosition position(name);

    position.advance();
    position.advance();
    position.advance();
    position.advance();
    position.advance();

    REQUIRE(position.fileOffset == 5);
    REQUIRE(position.line == 1);
    REQUIRE(position.lineColumn == 6);
}

TEST_CASE("SourcePosition next line") {
    px::Utf8String name{"myModule.px"};
    px::SourcePosition position(name);

    position.advance();
    position.advance();
    position.advance();
    position.advance();
    position.advance();

    position.nextLine();

    position.advance();
    position.advance();

    REQUIRE(position.fileOffset == 7);
    REQUIRE(position.line == 2);
    REQUIRE(position.lineColumn == 3);
}

TEST_CASE("SourcePosition set location") {
    px::Utf8String name{"myModule.px"};
    px::SourcePosition position(name);

    position.advance();
    position.advance();
    position.advance();
    position.advance();
    position.advance();

    position.nextLine();

    position.advance();
    position.advance();

    px::SourcePosition position2(name);
    position2.setLocation(position);

    REQUIRE(position2.fileOffset == 7);
    REQUIRE(position2.line == 2);
    REQUIRE(position2.lineColumn == 3);
}
