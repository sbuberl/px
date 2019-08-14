#include <cassert>
#include <iostream>
#include "catch.hpp"
#include <Scope.h>

TEST_CASE("ScopeTree root") {
    px::ScopeTree tree{};

    REQUIRE(tree.root()== tree.current());
}

TEST_CASE("ScopeTree enter and end scope") {
    px::ScopeTree tree{};

    REQUIRE(tree.root()== tree.current());

    px::Scope* root = tree.current();
    px::Scope *child1 = new px::Scope{root};
    px::Scope *child2 = new px::Scope{root};

    px::SymbolTable *child1Symbols = child1->symbols();
    auto function = new px::Function{"funcA", {}, px::Type::VOID, false};
    child1Symbols->addSymbol(function);

    px::SymbolTable *child2Symbols = child2->symbols();
    auto variable = new px::Variable{"myFloat", px::Type::FLOAT32};
    auto function2 = new px::Function{"funcB", {}, px::Type::VOID, false};
    child2Symbols->addSymbol(variable);
    child2Symbols->addSymbol(function2);

    px::Scope *scope2 = tree.enterScope();
    REQUIRE(scope2->symbols()->getFunction("funcA") == function);
    tree.endScope();

    px::Scope *scope3 = tree.enterScope();
    REQUIRE(scope3->symbols()->getFunction("funcB") == function2);
    REQUIRE(scope3->symbols()->getVariable("myFloat") == variable);
    tree.endScope();
}