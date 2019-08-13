#include <cassert>
#include <iostream>
#include "catch.hpp"
#include <Scope.h>

TEST_CASE("Scope parent") {
    px::Scope parent{};
    px::Scope child{&parent};

    REQUIRE(parent.parent() == nullptr);
    REQUIRE(child.parent() == &parent);
}

TEST_CASE("Scope get symbol table") {
    px::Scope scope;
    px::SymbolTable *symbols = scope.symbols();
    REQUIRE(symbols != nullptr);
    REQUIRE(symbols->getSymbol("string") != nullptr);
}

TEST_CASE("Scope enter and leave scope") {
    px::Scope scope;
    px::Scope child1{&scope};
    px::Scope child2{&scope};
    px::Scope child3{&scope};
    auto variable = new px::Variable{"myFloat", px::Type::FLOAT32};
    auto function = new px::Function{"funcA", {}, px::Type::VOID, false};
    child1.symbols()->addSymbol(variable);
    child2.symbols()->addSymbol(function);
    child3.symbols()->addSymbol(new px::Type{*px::Type::STRING});

    px::Scope *current = scope.enterScope();
    auto currentSymbols = current->symbols();
    REQUIRE(currentSymbols->getVariable("myFloat") == variable);
    REQUIRE(currentSymbols->getFunction("funcA") == nullptr);
    scope.leaveScope();

    current = scope.enterScope();
    currentSymbols = current->symbols();
    REQUIRE(currentSymbols->getVariable("myFloat") == nullptr);
    REQUIRE(currentSymbols->getFunction("funcA") == function);
    scope.leaveScope();

    current = scope.enterScope();
    currentSymbols = current->symbols();
    REQUIRE(currentSymbols->getVariable("myFloat") == nullptr);
    REQUIRE(currentSymbols->getFunction("funcA") == nullptr);
    scope.leaveScope();
}
