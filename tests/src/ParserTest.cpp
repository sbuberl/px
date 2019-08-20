#include <cassert>
#include <sstream>
#include "catch.hpp"
#include <Parser.h>

TEST_CASE("Parser function declare") {
    px::Utf8String name{"myModule.px"};
    px::Utf8String source{"func blah() : int32;"};
    std::string sourceString = source.toString();
    std::stringstream input{ sourceString };
    px::ErrorLog errors;
    px::Parser parser(&errors);
    auto module = parser.parse(name, input);
    auto firstStatement = (px::ast::FunctionDefinition*) module->statements[0].get();
    REQUIRE(firstStatement->nodeType == px::ast::NodeType::DECLARE_FUNC);
    REQUIRE(firstStatement->prototype->name == "blah");
    REQUIRE(firstStatement->prototype->returnTypeName == "int32");
    REQUIRE(!firstStatement->prototype->isExtern);
}

TEST_CASE("Parser function declare extern") {
    px::Utf8String name{"myModule.px"};
    px::Utf8String source{"extern func blah() : int32;"};
    std::string sourceString = source.toString();
    std::stringstream input{ sourceString };
    px::ErrorLog errors;
    px::Parser parser(&errors);
    auto module = parser.parse(name, input);
    auto firstStatement = (px::ast::FunctionDefinition*) module->statements[0].get();
    REQUIRE(firstStatement->nodeType == px::ast::NodeType::DECLARE_FUNC);
    REQUIRE(firstStatement->prototype->name == "blah");
    REQUIRE(firstStatement->prototype->returnTypeName == "int32");
    REQUIRE(firstStatement->prototype->isExtern);
}

TEST_CASE("Parser function definition empty") {
    px::Utf8String name{"myModule.px"};
    px::Utf8String source{"func blah() : void { }"};
    std::string sourceString = source.toString();
    std::stringstream input{ sourceString };
    px::ErrorLog errors;
    px::Parser parser(&errors);
    auto module = parser.parse(name, input);
    auto firstStatement = (px::ast::FunctionDefinition*) module->statements[0].get();
    REQUIRE(firstStatement->nodeType == px::ast::NodeType::DECLARE_FUNC_BODY);
    REQUIRE(firstStatement->prototype->name == "blah");
    REQUIRE(firstStatement->prototype->returnTypeName == "void");
}

TEST_CASE("Parser declare var") {
    px::Utf8String name{"myModule.px"};
    px::Utf8String source{"myVar: int64 = 10;"};
    std::string sourceString = source.toString();
    std::stringstream input{ sourceString };
    px::ErrorLog errors;
    px::Parser parser(&errors);
    auto module = parser.parse(name, input);
    auto firstStatement = (px::ast::VariableDeclaration*) module->statements[0].get();
    REQUIRE(firstStatement->nodeType == px::ast::NodeType::DECLARE_VAR);
    REQUIRE(firstStatement->name == "myVar");
    REQUIRE(firstStatement->typeName == "int64");
    REQUIRE(firstStatement->initialValue->nodeType == px::ast::NodeType::LITERAL_INT);
}

TEST_CASE("Parser var assign") {
    px::Utf8String name{"myModule.px"};
    px::Utf8String source{"x = 127.5;"};
    std::string sourceString = source.toString();
    std::stringstream input{ sourceString };
    px::ErrorLog errors;
    px::Parser parser(&errors);
    auto module = parser.parse(name, input);
    auto firstStatement = (px::ast::AssignmentStatement*) module->statements[0].get();
    REQUIRE(firstStatement->nodeType == px::ast::NodeType::STMT_ASSIGN);
    REQUIRE(firstStatement->variableName == "x");
    REQUIRE(firstStatement->expression->nodeType == px::ast::NodeType::LITERAL_FLOAT);
}

TEST_CASE("Parser return void") {
    px::Utf8String name{"myModule.px"};
    px::Utf8String source{"return;"};
    std::string sourceString = source.toString();
    std::stringstream input{ sourceString };
    px::ErrorLog errors;
    px::Parser parser(&errors);
    auto module = parser.parse(name, input);
    auto firstStatement = (px::ast::ReturnStatement*) module->statements[0].get();
    REQUIRE(firstStatement->nodeType == px::ast::NodeType::STMT_RETURN);
    REQUIRE(firstStatement->returnValue.get() == nullptr);
}

TEST_CASE("Parser return") {
    px::Utf8String name{"myModule.px"};
    px::Utf8String source{"return \"Testing\";"};
    std::string sourceString = source.toString();
    std::stringstream input{ sourceString };
    px::ErrorLog errors;
    px::Parser parser(&errors);
    auto module = parser.parse(name, input);
    auto firstStatement = (px::ast::ReturnStatement*) module->statements[0].get();
    REQUIRE(firstStatement->nodeType == px::ast::NodeType::STMT_RETURN);
    REQUIRE(firstStatement->returnValue.get() != nullptr);
    REQUIRE(firstStatement->returnValue->nodeType == px::ast::NodeType::LITERAL_STRING);
}
TEST_CASE("Parser block empty") {
    px::Utf8String name{"myModule.px"};
    px::Utf8String source{"{ }"};
    std::string sourceString = source.toString();
    std::stringstream input{ sourceString };
    px::ErrorLog errors;
    px::Parser parser(&errors);
    auto module = parser.parse(name, input);
    auto firstStatement = (px::ast::BlockStatement*) module->statements[0].get();
    REQUIRE(firstStatement->nodeType == px::ast::NodeType::STMT_BLOCK);
    REQUIRE(firstStatement->statements.empty());
}

TEST_CASE("Parser block") {
    px::Utf8String name{"myModule.px"};
    px::Utf8String source{"{ x: char = 'A'; print(x); }"};
    std::string sourceString = source.toString();
    std::stringstream input{sourceString};
    px::ErrorLog errors;
    px::Parser parser(&errors);
    auto module = parser.parse(name, input);
    auto firstStatement = (px::ast::BlockStatement *) module->statements[0].get();
    REQUIRE(firstStatement->nodeType == px::ast::NodeType::STMT_BLOCK);
    REQUIRE(firstStatement->statements.size() == 2);
}

TEST_CASE("Parser if") {
    px::Utf8String name{"myModule.px"};
    px::Utf8String source{" if (y > 0) { x: char = 'A'; print(x); }"};
    std::string sourceString = source.toString();
    std::stringstream input{ sourceString };
    px::ErrorLog errors;
    px::Parser parser(&errors);
    auto module = parser.parse(name, input);
    auto firstStatement = (px::ast::IfStatement*) module->statements[0].get();
    REQUIRE(firstStatement->nodeType == px::ast::NodeType::STMT_IF);
    REQUIRE(firstStatement->condition->nodeType == px::ast::NodeType::EXP_BINARY_OP);
    REQUIRE(firstStatement->trueStatement->nodeType == px::ast::NodeType::STMT_BLOCK);
    REQUIRE(firstStatement->elseStatement.get() == nullptr);
}

TEST_CASE("Parser if else") {
    px::Utf8String name{"myModule.px"};
    px::Utf8String source{" if (y > 0) { x: char = 'A'; print(x); } else -x;"};
    std::string sourceString = source.toString();
    std::stringstream input{ sourceString };
    px::ErrorLog errors;
    px::Parser parser(&errors);
    auto module = parser.parse(name, input);
    auto firstStatement = (px::ast::IfStatement*) module->statements[0].get();
    REQUIRE(firstStatement->nodeType == px::ast::NodeType::STMT_IF);
    REQUIRE(firstStatement->condition->nodeType == px::ast::NodeType::EXP_BINARY_OP);
    REQUIRE(firstStatement->trueStatement->nodeType == px::ast::NodeType::STMT_BLOCK);
    REQUIRE(firstStatement->elseStatement->nodeType == px::ast::NodeType::STMT_EXP);
}
