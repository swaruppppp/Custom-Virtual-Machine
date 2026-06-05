#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

bool runSource(const std::string &source, bool showBytecode)
{
    try
    {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto ast = parser.parse();

        Compiler compiler;
        auto bytecode = compiler.compile(*ast);

        if (showBytecode)
        {
            std::cout << "--- Bytecode ---\n";
            std::cout << Compiler::disassemble(bytecode);
            std::cout << "--- Output ---\n";
        }

        VM vm;
        vm.run(bytecode);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool runFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: can't open '" << filename << "'" << std::endl;
        return false;
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return runSource(ss.str(), true);
}

void runREPL()
{
    std::cout << "CVM++ | Type 'exit' to quit, 'reset' to clear.\n\n";

    Compiler compiler;
    VM vm;
    std::string pending;
    int braces = 0;

    while (true)
    {
        std::cout << (braces == 0 ? ">> " : ".. ");

        std::string line;
        if (!std::getline(std::cin, line))
        {
            std::cout << "\n";
            break;
        }

        if (braces == 0 && (line == "exit" || line == "quit"))
            break;

        if (braces == 0 && line == "reset")
        {
            compiler = Compiler(); // Reset compiler variables
            vm = VM(); // Reset VM globals
            std::cout << "(cleared)\n";
            continue;
        }

        pending += line + "\n";

        for (char c : line)
        {
            if (c == '{') braces++;
            if (c == '}') braces--;
        }

        if (braces > 0)
            continue;
        braces = 0;

        if (pending.empty() || pending.find_first_not_of(" \t\n\r") == std::string::npos)
        {
            pending.clear();
            continue;
        }

        try
        {
            Lexer lexer(pending);
            auto tokens = lexer.tokenize();

            Parser parser(tokens);
            auto ast = parser.parse();

            auto bytecode = compiler.compile(*ast, false); // Compile new AST, keep variables
            vm.run(bytecode, false); // Run new bytecode, keep globals
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }

        pending.clear();
    }
}

int main(int argc, char *argv[])
{
    if (argc >= 2)
        return runFile(argv[1]) ? 0 : 1;
    runREPL();
    return 0;
}
