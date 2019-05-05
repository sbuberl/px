cmake_minimum_required(VERSION 3.12)
project(px)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM 6.0.0 REQUIRED CONFIG)
find_package(ICU 62.1 COMPONENTS io data test tu uc REQUIRED)

include_directories(compiler/include)
include_directories(tests/include)

add_executable(pxc
        compiler/include/AST/AST.h
        compiler/include/AST/Declaration.h
        compiler/include/AST/Expression.h
        compiler/include/AST/Literal.h
        compiler/include/AST/Statement.h
        compiler/include/AST/Visitor.h
        compiler/include/CG/CCompiler.h
        compiler/include/CG/ModuleData.h
        compiler/include/CG/LLVMCompiler.h
        compiler/include/ContextAnalyzer.h
        compiler/include/Error.h
        compiler/include/Parser.h
        compiler/include/Scanner.h
        compiler/include/Scope.h
        compiler/include/SourcePosition.h
        compiler/include/Symbol.h
        compiler/include/Token.h
        compiler/include/Utf8String.h
        compiler/src/AST/Declaration.cpp
        compiler/src/AST/Expression.cpp
        compiler/src/AST/Literal.cpp
        compiler/src/AST/Node.cpp
        compiler/src/AST/Statement.cpp
        compiler/src/CG/CCompiler.cpp
        compiler/src/CG/LLVMCompiler.cpp

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
        tests/src/Utf8StringTest.cpp)

target_link_libraries(tests LLVM ${ICU_LIBRARIES})
