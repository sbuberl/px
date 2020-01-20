
#include "Parser.h"
#include "Error.h"
#include "ContextAnalyzer.h"
#include "cg/CCompiler.h"
#include <iostream>
#include <fstream>

using namespace px;

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "No filename given" << std::endl;
        return -1;
    }

    //std::cout << "Building Symbol Table " << std::endl;

    for (size_t i = 1; i < argc; i++) {
        px::ScopeTree scopeTree;
        px::ErrorLog errors;

        const char* fileArg = argv[i];
        px::Utf8String fileName = fileArg;
        px::Parser parser{&errors};
        std::ifstream fis(fileArg);
        if(!fis.good()) {
            std::cerr << "File " << fileArg << " was not found" << std::endl;
            return -3;
        }
        std::unique_ptr<px::ast::Module> ast;
        try {
            ast = parser.parse(fileName, fis);
        }
        catch (const px::Error &) {
            errors.output();
            return -2;
        }

        px::ContextAnalyzer analyzer{ scopeTree.current(), &errors };
        analyzer.analyze(*ast);

        if (errors.count() > 0)
        {
            errors.output();
            return -2;
        }

        px::CCompiler compiler{ &scopeTree };
        compiler.compile(*ast);
    }

    return 0;
}
