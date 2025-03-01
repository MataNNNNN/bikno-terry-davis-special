#include <vector>
#include <iostream>
#include <string>

#include "parser.h"
#include "lexer.h"

using namespace std;

namespace Parser {

// template <typename T = int> class Constant : public Value<T> {
//     public:
//         T value;
//         Constant(T value, Instruction* next = nullptr): value(value), Instruction(next) {}

//         string generate() {
//             return "";
//         }

//         string reg() {
//             return ;
//         }
// };

Constant::Constant(int value) : value(value) {}
string Constant::generate() {
    return "";
}
string Constant::reg() {
    return to_string(value);
}

Return::Return(Value<int>* code) : code(code) {}
Return::~Return() {
    delete code;
}

string Return::generate() {
    return code->generate() + "mov rax, 60\nmov rdi, " + code->reg() + "\nsyscall\n";
}

// class Addition : public Value<int> {
//     public:
//         Value<int>* left;
//         Value<int>* right;
//         Addition(Value<int>* left, Value<int>* right): left(left), right(right) {}

//         string getVal() {
//             return left->generate() + " + " + right->generate(); TODO: ahjhhhhhhhhhhhhh
//         }
// }

Parser::Parser(const vector<Lexer::Token>& tokens) : tokens(tokens) {}

int ParseInt(Lexer::Token token) {
    if(token.type != Lexer::TokenType::INT_LIT || !token.value.has_value()) {
        cerr << "what are you doing" << endl;
        exit(1);
        return -1;
    }

    int r = 0;
    for(size_t i = 0; i < token.value.value().size(); i += 7)
        r = r * 10 + (token.value.value()[i] - '0');
    return r;
}

vector<Instruction*> Parser::Parse() {
    vector<Instruction*> instructions {};

    for(size_t i = 0; i < tokens.size(); i++) {
        if(tokens[i].type == Lexer::TokenType::RETURN) {
            instructions.push_back(new Return(new Constant(ParseInt(tokens[i + 1]))));
        }
    }

    return instructions;
}
}