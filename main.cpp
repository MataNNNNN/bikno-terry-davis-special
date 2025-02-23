#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "lexer.h"

using namespace std;

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
    //TODO: ast time😎😎😎
    file.close();
    return 0;
}