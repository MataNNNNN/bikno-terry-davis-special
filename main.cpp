#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "lexer.h"
#include "parser.h"

using namespace std;

int main(int argc, char* argv[]) {
    if(argc != 2) {
        throw runtime_error("error in the sigma house not 2");
        return 1;
    }

    ifstream file(argv[1]);
    if(!file) {
        throw runtime_error("file not found");
        return 1;
    }

    Lexer lexer(file);
    vector<Lexer::Token> tokens = lexer.Lex();
    cout << tokens.size() << endl;
    for (Lexer::Token token : tokens)
        token.print();

    cout << endl << endl;

    Parser::Parser parser(tokens);
    vector<unique_ptr<Parser::Instruction>> instruction = parser.Parse();
    ofstream out("a.out.s");
    out << "global _start\nsection .text\n_start:";
    cout << instruction.size() << endl;
    for(int i = 0; i < instruction.size(); i++) {
        string s = instruction[i]->generate();
        cout << i << s << endl;
        out << s << '\n';
    }
    out << "\nmov    rax, 60\nmov    rdi, 0\nsyscall";
    // system("nasm -felf64 a.out.s -o a.out.o && ld a.out.o -o a.out");
    return 0;
}