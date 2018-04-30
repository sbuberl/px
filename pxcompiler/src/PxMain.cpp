
#include "Parser.h"
#include "Error.h"
#include "ContextAnalyzer.h"
#include "CG/LLVMCompiler.h"
#include <iostream>
#include <fstream>
using namespace px;

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "No filename given" << std::endl;
        return -1;
    }

    //std::cout << "Building Symbol Table " << std::endl;

    px::SymbolTable globals;
    globals.addGlobals();

    px::ErrorLog errors;

    px::Utf8String fileName = argv[1];
    px::Parser parser(&errors);
    std::ifstream fis(argv[1]);
    px::ast::AST *ast = nullptr;
    try
    {
        ast = parser.parse(fileName, fis);
    }
    catch (const px::Error &error)
    {
        errors.output();
        return -2;
    }

    px::ContextAnalyzer analyzer(&globals, &errors);
    analyzer.analyze(ast);

    if (errors.count() > 0)
    {
        errors.output();
        return -2;
    }

    px::LLVMCompiler compiler;
    compiler.compile(ast);

    delete ast;

    return 0;
}
