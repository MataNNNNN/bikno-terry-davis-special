#include <iostream>
#include <fstream>
#include <format>

#include "parser.h"

using std::runtime_error, std::format;

int main(int argc, char* argv[]) {
    if(argc < 2)
        throw runtime_error("error in the sigma house not 2");

    ifstream file(argv[1]);
    if(!file)
        throw runtime_error("file not found");

    Lexer lexer(file);
    vector<Lexer::Token> tokens = lexer.Lex();
    std::cout << tokens.size() << std::endl;
    for (Lexer::Token token : tokens)
        token.print();

    std::cout << '\n' << std::endl;

    Parser parser(tokens);
    vector<unique_ptr<Instruction>> instruction = parser.Parse();
    std::ofstream out((argc >= 3) ? format("{}.s", argv[2]) : "a.out.s");
    ostringstream oss;

    out << "global _start\nsection .text";
    std::cout << instruction.size() << std::endl;
    for(int i = 0; i < instruction.size(); i++)
        instruction[i]->generate(oss) << '\n';
    
    string l = oss.str();
    std::cout << l << std::endl;
    out << l << "\nleave\nmov    rax, 60\nsyscall" << std::endl; //\nmov    rdi, 0

    if(argc >= 3)
        system(format("nasm -felf64 {0}.s -o {0}.o&& ld {0}.o -o {0}", argv[2]).c_str());
    return 0;
}