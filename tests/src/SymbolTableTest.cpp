#include <cassert>
#include <iostream>
#include "catch.hpp"
#include "Symbol.h"

TEST_CASE("Scanner get parent") {
    px::SymbolTable parent{};
    px::SymbolTable child{&parent};

    REQUIRE(parent.getParent() == nullptr);
    REQUIRE(child.getParent() == &parent);
}

TEST_CASE("Scanner get symbol") {
    px::SymbolTable table;
    auto variable = new px::Variable{"myFloat", px::Type::FLOAT32};
    auto function = new px::Function{"funcA", {}, px::Type::VOID, false};
    table.addSymbol(variable);
    table.addSymbol(new px::Type{*px::Type::STRING});
    table.addSymbol(function);

    REQUIRE(table.getSymbol("blah") == nullptr);
    REQUIRE(table.getSymbol("myFloat") == variable);
    REQUIRE(table.getSymbol("funcA") == function);
    REQUIRE(table.getSymbol("string") != nullptr);
}

TEST_CASE("Scanner get variable") {
    px::SymbolTable table;
    auto variable = new px::Variable{"myFloat", px::Type::FLOAT32};
    auto function = new px::Function{"funcA", {}, px::Type::VOID, false};
    table.addSymbol(variable);
    table.addSymbol(new px::Type{*px::Type::STRING});
    table.addSymbol(function);

    REQUIRE(table.getVariable("blah") == nullptr);
    REQUIRE(table.getVariable("myFloat") == variable);
    REQUIRE(table.getVariable("funcA") == nullptr);
    REQUIRE(table.getVariable("string") == nullptr);
}

TEST_CASE("Scanner get function") {
    px::SymbolTable table;
    auto variable = new px::Variable{"myFloat", px::Type::FLOAT32};
    auto function = new px::Function{"funcA", {}, px::Type::VOID, false};
    table.addSymbol(variable);
    table.addSymbol(new px::Type{*px::Type::STRING});
    table.addSymbol(function);

    REQUIRE(table.getFunction("blah") == nullptr);
    REQUIRE(table.getFunction("myFloat") == nullptr);
    REQUIRE(table.getFunction("funcA") == function);
    REQUIRE(table.getFunction("string") == nullptr);
}

TEST_CASE("Scanner get type") {
    px::SymbolTable table;
    auto variable = new px::Variable{"myFloat", px::Type::FLOAT32};
    auto function = new px::Function{"funcA", {}, px::Type::VOID, false};
    table.addSymbol(variable);
    table.addSymbol(new px::Type{*px::Type::STRING});
    table.addSymbol(function);

    REQUIRE(table.getType("blah") == nullptr);
    REQUIRE(table.getType("myFloat") == nullptr);
    REQUIRE(table.getType("funcA") == nullptr);
    REQUIRE(table.getType("string") != nullptr);
}

TEST_CASE("Scanner get local variables") {
    px::SymbolTable table;
    auto myFloat = new px::Variable{"myFloat", px::Type::FLOAT32};
    auto myInt = new px::Variable{"myInt", px::Type::INT64};
    auto myChar = new px::Variable{"myChar", px::Type::CHAR};
    auto function = new px::Function{"funcA", {}, px::Type::VOID, false};
    table.addSymbol(myFloat);
    table.addSymbol(new px::Type{*px::Type::STRING});
    table.addSymbol(myInt);
    table.addSymbol(function);
    table.addSymbol(myChar);

    auto result = table.getLocalVariables();

    REQUIRE(std::count(result.begin(), result.end(), myFloat) == 1);
    REQUIRE(std::count(result.begin(), result.end(), myInt) == 1);
    REQUIRE(std::count(result.begin(), result.end(), myChar) == 1);
}
