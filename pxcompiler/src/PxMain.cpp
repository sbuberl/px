
#include "Parser.h"
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

    //std::cout << "Starting parsing of file " << argv[1] << std::endl;

    px::Parser parser(&globals);
    std::ifstream fis(argv[1]);
    px::ast::AST *ast = parser.parse(fis);
    fis.close();

    //std::cout << "Parsing Complete " << ast <<  std::endl;

    px::ContextAnalyzer analyzer(&globals);
    analyzer.analyze(ast);

    //std::cout << "Analysis Complete" << std::endl;

    px::LLVMCompiler compiler;
    compiler.compile(ast);

    delete ast;

    return 0;
}
