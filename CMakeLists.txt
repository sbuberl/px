cmake_minimum_required(VERSION 3.12)
project(px)

set(CMAKE_CXX_STANDARD 17)

# Code Coverage Configuration
add_library(coverage_config INTERFACE)

option(CODE_COVERAGE "Enable coverage reporting" OFF)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # Add required flags (GCC & LLVM/Clang)
  target_compile_options(coverage_config INTERFACE
    -O0        # no optimization
    -g         # generate debug info
    --coverage # sets all required flags
  )
  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    target_link_options(coverage_config INTERFACE --coverage)
  else()
    target_link_libraries(coverage_config INTERFACE --coverage)
  endif()
endif(CODE_COVERAGE)

find_package(ICU 60.2 COMPONENTS io data tu uc REQUIRED)

include_directories(compiler/include)
include_directories(runtime/include)
include_directories(tests/include)

add_executable(pxc
        compiler/include/ast/AST.h
        compiler/include/ast/Declaration.h
        compiler/include/ast/Expression.h
        compiler/include/ast/Literal.h
        compiler/include/ast/Statement.h
        compiler/include/ast/Visitor.h
        compiler/include/cg/CCompiler.h
        compiler/include/ContextAnalyzer.h
        compiler/include/Error.h
        compiler/include/IO.h
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

        compiler/src/ContextAnalyzer.cpp
        compiler/src/Parser.cpp
        compiler/src/PxMain.cpp
        compiler/src/Scanner.cpp
        compiler/src/Symbol.cpp
        compiler/src/Token.cpp)

target_link_libraries(pxc coverage_config ${ICU_LIBRARIES})

add_library(pxruntime STATIC runtime/src/PxRuntime.cpp)

add_executable(tests
        tests/src/TestMain.cpp
        tests/src/ParserTest.cpp
        tests/src/ScannerTest.cpp
        tests/src/ScopeTest.cpp
        tests/src/ScopeTreeTest.cpp
        tests/src/SourcePositionTest.cpp
        tests/src/SymbolTableTest.cpp
        tests/src/TokenTest.cpp
        tests/src/Utf8StringTest.cpp

        compiler/src/ast/Declaration.cpp
        compiler/src/ast/Expression.cpp
        compiler/src/ast/Literal.cpp
        compiler/src/ast/Node.cpp
        compiler/src/ast/Statement.cpp
        compiler/src/Parser.cpp
        compiler/src/Scanner.cpp
        compiler/src/Symbol.cpp
        compiler/src/Token.cpp)

target_link_libraries(tests ${ICU_LIBRARIES})

add_test(NAME pxc_test COMMAND tests)

