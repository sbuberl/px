cmake_minimum_required(VERSION 3.12)
project(px)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM 6.0.0 REQUIRED CONFIG)
find_package(ICU 62.1 COMPONENTS io data test tu uc REQUIRED)

include_directories(compiler/include)
include_directories(tests/include)

add_executable(pxc
        compiler/include/ast/AST.h
        compiler/include/ast/Declaration.h
        compiler/include/ast/Expression.h
        compiler/include/ast/Literal.h
        compiler/include/ast/Statement.h
        compiler/include/ast/Visitor.h
        compiler/include/cg/CCompiler.h
        compiler/include/cg/ModuleData.h
        compiler/include/cg/LLVMCompiler.h
        compiler/include/ContextAnalyzer.h
        compiler/include/Error.h
        compiler/include/Parser.h
        compiler/include/Scanner.h
        compiler/include/Scope.h
        compiler/include/SourcePosition.h
        compiler/include/Symbol.h
        compiler/include/Token.h
        compiler/include/Utf8String.h
        compiler/src/ast/Declaration.cpp
        compiler/src/ast/Expression.cpp
        compiler/src/ast/Literal.cpp
        compiler/src/ast/Node.cpp
        compiler/src/ast/Statement.cpp
        compiler/src/cg/CCompiler.cpp
        compiler/src/cg/LLVMCompiler.cpp

        compiler/src/ContextAnalyzer.cpp
        compiler/src/Parser.cpp
        compiler/src/PxMain.cpp
        compiler/src/Scanner.cpp
        compiler/src/Symbol.cpp
        compiler/src/Token.cpp)

target_link_libraries(pxc LLVM ${ICU_LIBRARIES})

add_library(pxruntime STATIC compiler/src/runtime/PxRuntime.cpp)

add_executable(tests
        tests/src/TestMain.cpp
        tests/src/ScannerTest.cpp
        tests/src/ScopeTest.cpp
        tests/src/ScopeTreeTest.cpp
        tests/src/SourcePositionTest.cpp
        tests/src/SymbolTableTest.cpp
        tests/src/TokenTest.cpp
        tests/src/Utf8StringTest.cpp

        compiler/src/Scanner.cpp
        compiler/src/Symbol.cpp
        compiler/src/Token.cpp)

target_link_libraries(tests LLVM ${ICU_LIBRARIES})
