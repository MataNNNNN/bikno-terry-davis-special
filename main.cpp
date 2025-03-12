#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "lexer.h"
#include "parser.h"

using namespace std;

struct Wrapper { //????
    vector<Parser::Instruction*> data;
    ~Wrapper() {
        for (Parser::Instruction* instruction : data)
            delete instruction;
    }
};

int main(int argc, char* argv[]) {
    if(argc != 2) {
        cerr << "error in the sigma house not 2" << endl << argc << endl;
        return 1;
    }

    ifstream file(argv[1]);
    Lexer lexer(file);
    vector<Lexer::Token> tokens = lexer.Lex();
    cout << tokens.size() << endl;
    for (Lexer::Token token : tokens)
        token.print();

    cout << endl << endl;

    Parser::Parser parser(tokens);
    Wrapper wrapper {parser.Parse()};
    
    for(int i = 0; i < wrapper.data.size(); i++)
        cout << i << wrapper.data[i]->generate() << endl;
    cout << wrapper.data.size() << endl;
    return 0;
}