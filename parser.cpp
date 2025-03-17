#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>

#include "parser.h"
#include "lexer.h"

using namespace std;

namespace Parser {
Value::Value(string reg): reg(reg) {}
Value::Value(int value): reg(to_string(value)) {}
string Value::getReg() const {
    return reg;
}

// Constant::Constant(string value): reg(value) {}
// Constant::Constant(int t) : value(to_string(t)) {}
// string Constant::reg() const {
//     return value;
// }

Return::Return(Value* code) : code(code) {}
Return::~Return() {
    delete code;
}

string Return::generate() const {
    return code->getReg();
}

Operator::Operator(Value* left, Value* right, Value* store = nullptr) : left(left), right(right), store(store), Value("") {} // TODO: idk
Operator::Operator(int left, int right, Value* store = nullptr) : left(new Value(left)), right(new Value(right)), store(store), Value("") {}
Operator::~Operator() {
    delete left;
    delete right;
    delete store;
}

string Operator::generate() const {
    string r = "";
    if(dynamic_cast<Instruction*>(left) != nullptr) {
        r += ((Instruction*)left)->generate();
    }
    if(dynamic_cast<Instruction*>(right) != nullptr) {
        r += ((Instruction*)right)->generate();
    }
    return r;
}

Addition::Addition(Value* left, Value* right, Value* store = nullptr): Operator(left, right, store) {}
Addition::Addition(int left, int right, Value* store = nullptr): Operator(left, right, store) {}

string Addition::generate() const {
    return Operator::generate() + "\nADD " + left->getReg() + ", " + right->getReg() + ", " + store->getReg();
}
string Addition::getReg() const {
    return "rax";
}

Subtraction::Subtraction(Value*left, Value* right, Value* store = nullptr): Operator(left, right, store) {}
Subtraction::Subtraction(int left, int right, Value* store = nullptr): Operator(left, right, store) {}

string Subtraction::generate() const {
    return Operator::generate() + "\nsub " + left->getReg() + ", " + right->getReg() + ", " + store->getReg();
}
string Subtraction::getReg() const {
    return "rax";
}

Multiplication::Multiplication(Value*left, Value* right, Value* store = nullptr): Operator(left, right, store) {}
Multiplication::Multiplication(int left, int right, Value* store = nullptr) : Operator(left, right, store) {}

string Multiplication::generate() const {
    return Operator::generate() + "\nimul " + left->getReg() + ", " + right->getReg() + ", " + store->getReg();
}
string Multiplication::getReg() const {
    return "rax";
}

Division::Division(Value*left, Value* right, Value* store = nullptr) : Operator(left, right, store) {}
Division::Division(int left, int right, Value* store = nullptr) : Operator(left, right, store) {}

string Division::generate() const {
    return Operator::generate() + "\nidk" + left->getReg() + ", " + right->getReg() + ", " + store->getReg();
}
string Division::getReg() const {
    return "rax";
}

Assignment::Assignment(Value* into, Value* value): into(into), value(value) {}
string Assignment::generate() const {

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
        r = r * 10 + token.value.value()[i] - '0';
    return r;
}

Value* parseExpression(const vector<Lexer::Token>& tokens, size_t& i);

Value* parseFactor(const vector<Lexer::Token>& tokens, size_t& i) {
    if(tokens[i].type == Lexer::TokenType::INT_LIT)
        return new Value(ParseInt(tokens[i++]));
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
    unordered_map<string, Value*> identifiers;
    int stack = 0;

    for(size_t i = 0; i < tokens.size(); i++) {
        switch (tokens[i].type) {
            case Lexer::TokenType::RETURN:
                instructions.push_back(new Return(parseExpression(tokens, ++i))); // make it type type
                break;
            case Lexer::TokenType::INT_TYPE:
                stack += ParseInt(tokens[i + 2]);
                identifiers.insert(pair(tokens[i += 3].value.value(), new Value("[rbp-" + to_string(stack) + "]")));
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
    instructions.insert(instructions.begin(), new Subtraction(new Value("rbp"), new Value(stack)));
    return instructions;
}
}