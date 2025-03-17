#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>

#include "parser.h"
#include "lexer.h"

using namespace std;

namespace Parser {
Constant::Constant(string value): value(value) {}
Constant::Constant(int t) : value(to_string(t)) {}
string Constant::reg() {
    return value;
}

Return::Return(Value* code) : code(code) {}
Return::~Return() {
    delete code;
}

string Return::generate() {
    return code->reg();
}

Operator::Operator(Value* left, Value* right, Value* store = nullptr) : left(left), right(right), store(store) {}
Operator::Operator(int left, int right, Value* store = nullptr) : left(new Constant(left)), right(new Constant(right)), store(store) {}
Operator::~Operator() {
    delete left;
    delete right;
    delete store;
}

Addition::Addition(Value* left, Value* right, Value* store = nullptr) : Operator(left, right, store) {}
Addition::Addition(int left, int right, Value* store = nullptr) : Operator(left, right, store) {}

string Addition::generate() {
    return "\nadd " + left->reg() + ", " + right->reg();
}
string Addition::reg() {
    return "no no yet";
}

Subtraction::Subtraction(Value*left, Value* right) : Operator(left, right) {}
Subtraction::Subtraction(int left, int right) : Operator(left, right) {}

string Subtraction::generate() {
    return "-\n" + left->generate() + " " + right->generate();
}
string Subtraction::reg() {
    return "no no yet";
}

Multiplication::Multiplication(Value*left, Value* right) : Operator(left, right) {}
Multiplication::Multiplication(int left, int right) : Operator(left, right) {}

string Multiplication::generate() {
    return left->generate() + "\n" + right->generate() + "\nimul " + left->reg() + ", " + right->reg() + ", " + left->reg();
}
string Multiplication::reg() {
    return "rax";
}

Division::Division(Value*left, Value* right) : Operator(left, right) {}
Division::Division(int left, int right) : Operator(left, right) {}

string Division::generate() {
    return "/\n" + left->generate() + " " + right->generate();
}
string Division::reg() {
    return "no no yet";
}

Parser::Parser(const vector<Lexer::Token>& tokens) : tokens(tokens) {}

class Assignment : public Operator {
    public: 
        Assignment(Value* into, Value* value): Operator(into, right) {}
        string generate() override {
            return "\n" + right->generate() + "\nmov " + left->reg() + ", " + right->reg();
        }
        string reg() override {
            return "should remove ts";
        }
};

int ParseInt(const Lexer::Token& token) {
    if(token.type != Lexer::TokenType::INT_LIT || !token.value.has_value()) {
        cerr << "not an int big dog" << endl;
        exit(1);
        return -1;
    }

    int r = 0;
    for(size_t i = 0; i < token.value.value().size(); i += 7)
        r = r * 10 + token.value.value()[i] - '0';
    return r;
}

Value* parseExpression(const vector<Lexer::Token>& tokens, size_t& i);

Value* parseFactor(const vector<Lexer::Token>& tokens, size_t& i) {
    if(tokens[i].type == Lexer::TokenType::INT_LIT)
        return new Constant(ParseInt(tokens[i++]));
    else if(tokens[i].type == Lexer::TokenType::IDENTIFIER) {
        cerr << "unimplemented" << endl;
        exit(1);
    }
    
    if(tokens[i].type == Lexer::TokenType::OPEN_PAREN) {
        Value* r = parseExpression(tokens, ++i);
        i++;
        return r;
    }
    cerr << (int)tokens[i].type << endl;
    exit(1);
    return nullptr;
}

Value* parseTerm(const vector<Lexer::Token>& tokens, size_t& i) {
    Value* node = parseFactor(tokens, i);

    while(tokens[i].type == Lexer::TokenType::MULTIPLICATION || tokens[i].type == Lexer::TokenType::DIVISION)
        node = Lexer::TokenType::MULTIPLICATION == tokens[i].type ? (Operator*)new Multiplication(node, parseFactor(tokens, ++i)) : (Operator*)new Division(node, parseFactor(tokens, ++i));
    
    return node;
}

Value* parseExpression(const vector<Lexer::Token>& tokens, size_t& i) {
    Value* node = parseTerm(tokens, i);
    
    while(tokens[i].type == Lexer::TokenType::ADDITION || tokens[i].type == Lexer::TokenType::SUBTRACTION)
        node = Lexer::TokenType::ADDITION == tokens[i].type ? (Value*)new Addition(node, parseTerm(tokens, ++i)) : (Value*)new Subtraction(node, parseTerm(tokens, ++i));

    return node;
}

vector<Instruction*> Parser::Parse() { //what did i do to my constants
    vector<Instruction*> instructions {};
    unordered_map<string, Constant*> identifiers;
    int stack = 0;

    for(size_t i = 0; i < tokens.size(); i++) {
        switch (tokens[i].type) {
            case Lexer::TokenType::RETURN:
                instructions.push_back(new Return(parseExpression(tokens, ++i))); // make it type type
                break;
            case Lexer::TokenType::INT_TYPE:
                stack += ParseInt(tokens[i + 2]);
                identifiers.insert(pair(tokens[i += 3].value.value(), new Constant("[rbp-" + to_string(stack) + "]")));
                break;
            case Lexer::TokenType::ASSIGNMENT:
                instructions.push_back(new Assignment(identifiers[tokens[i-1].value.value()], parseExpression(tokens, ++i)));
                break;
            default:
                cerr << "unimplemented" << endl;
                exit(1);
                break;
        }
    }
    instructions.insert(instructions.begin(), new Subtraction(new Constant("rbp"), new Constant(stack)));
    return instructions;
}
}