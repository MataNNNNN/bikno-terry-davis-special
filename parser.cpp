#include <vector>
#include <iostream>
#include <string>

#include "parser.h"
#include "lexer.h"

using namespace std;

namespace Parser {
template <typename M> Constant<M>::Constant(M value) : value(value) {}
template <typename M> string Constant<M>::generate() {
    return to_string((int)value);
}
template <typename M> string Constant<M>::reg() {
    return to_string((int)value);
}
// string Constant::reg() {
//     return to_string(value);
// }

Return::Return(Value<int>* code) : code(code) {}
Return::~Return() {
    delete code;
}

string Return::generate() {
    return code->generate() + "mov rax, 60\nmov rdi, " + code->reg() + "\nsyscall\n";
}

Operator::Operator(Value<int>* left, Value<int>* right) : left(left), right(right) {}
Operator::Operator(int left, int right) : left(new Constant(left)), right(new Constant(right)) {}
Operator::~Operator() {
    delete left;
    delete right;
}

Addition::Addition(Value<int>*left, Value<int>* right) : Operator(left, right) {}
Addition::Addition(int left, int right) : Operator(left, right) {}

string Addition::generate() {
    return "+\n" + left->generate() + " " + right->generate();
}
string Addition::reg() {
    return "no no yet";
}

Subtraction::Subtraction(Value<int>*left, Value<int>* right) : Operator(left, right) {}
Subtraction::Subtraction(int left, int right) : Operator(left, right) {}

string Subtraction::generate() {
    return "-\n" + left->generate() + " " + right->generate();
}
string Subtraction::reg() {
    return "no no yet";
}

Multiplication::Multiplication(Value<int>*left, Value<int>* right) : Operator(left, right) {}
Multiplication::Multiplication(int left, int right) : Operator(left, right) {}

string Multiplication::generate() {
    return "*\n" + left->generate() + " " + right->generate();
}
string Multiplication::reg() {
    return "no no yet";
}

Division::Division(Value<int>*left, Value<int>* right) : Operator(left, right) {}
Division::Division(int left, int right) : Operator(left, right) {}

string Division::generate() {
    return "/\n" + left->generate() + " " + right->generate();;
}
string Division::reg() {
    return "no no yet";
}

Parser::Parser(const vector<Lexer::Token>& tokens) : tokens(tokens) {}

int ParseInt(const Lexer::Token& token) {
    if(token.type != Lexer::TokenType::INT_LIT || !token.value.has_value()) {
        cerr << "not an int big dog" << endl;
        exit(1);
        return -1;
    }

    int r = 0;
    for(size_t i = 0; i < token.value.value().size(); i += 7)
        r = r * 10 + (token.value.value()[i] - '0');
    return r;
}

Value<int>* parseExpression(const vector<Lexer::Token>& tokens, size_t& i);

Value<int>* parseFactor(const vector<Lexer::Token>& tokens, size_t& i) {
    if(tokens[i].type == Lexer::TokenType::INT_LIT)
        return new Constant(ParseInt(tokens[i++]));
    
    if(tokens[i].type == Lexer::TokenType::OPEN_PAREN) {
        Value<int>* r = parseExpression(tokens, ++i);
        i++;
        return r;
    }
    cerr << (int)tokens[i].type << endl;
    exit(1);
    return nullptr;
}

Value<int>* parseTerm(const vector<Lexer::Token>& tokens, size_t& i) {
    Value<int>* node = parseFactor(tokens, i);

    while(tokens[i].type == Lexer::TokenType::MULTIPLICATION || tokens[i].type == Lexer::TokenType::DIVISION)
        node = Lexer::TokenType::MULTIPLICATION == tokens[i].type ? (Operator*)new Multiplication(node, parseFactor(tokens, ++i)) : (Operator*)new Division(node, parseFactor(tokens, ++i));
    
    return node;
}

Value<int>* parseExpression(const vector<Lexer::Token>& tokens, size_t& i) {
    Value<int>* node = parseTerm(tokens, i);
    
    while(tokens[i].type == Lexer::TokenType::ADDITION || tokens[i].type == Lexer::TokenType::SUBTRACTION)
        node = Lexer::TokenType::ADDITION == tokens[i].type ? (Operator*)new Addition(node, parseTerm(tokens, ++i)) : (Operator*)new Subtraction(node, parseTerm(tokens, ++i));

    return node;
}

vector<Instruction*> Parser::Parse() {
    vector<Instruction*> instructions {};

    for(size_t i = 0; i < tokens.size(); i++) {
        if(tokens[i].type == Lexer::TokenType::RETURN) {
            instructions.push_back(new Return(parseExpression(tokens, ++i))); // make it type type
        } else if(tokens[i].type == Lexer::TokenType::INT_TYPE) {
            i += 2;
            int size = ParseInt(tokens[i]);
            
        }
    }

    return instructions;
}
}