#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <stack>

#include "parser.h"
#include "lexer.h"

using namespace std;

namespace Parser {
Value::Value(string reg): reg(reg) {}
Value::Value(int value): reg(to_string(value)) {}
void Value::Free() {
    delete this;
}
string Value::getReg() const {
    return reg;
}

// Constant::Constant(string value): reg(value) {}
// Constant::Constant(int t) : value(to_string(t)) {}
// string Constant::reg() const {
//     return value;
// }

Return::Return(Value* code) : code(code) {}

string Return::generate() const {
    return Assignment(&Register::rdi, code).generate() + "\nmov    rax, 60\nsyscall";
}

Operator::Operator(Value* left, Value* right, Value* store) : left(left), right(right), store(store), Value("") {} // TODO: idk
Operator::Operator(int left, int right, Value* store) : left(new Value(left)), right(new Value(right)), store(store), Value("") {}
Operator::~Operator() {
    left->Free();
    right->Free();
    // delete store; i dont tink
}

string Operator::generate() const {
    string r = "";
    if(dynamic_cast<Operator*>(left) != nullptr) {
        r += ((Operator*)left)->generate();
    }
    if(dynamic_cast<Operator*>(right) != nullptr) {
        r += ((Operator*)right)->generate();
    }
    return r;
}

Addition::Addition(Value* left, Value* right, Value* store): Operator(left, right, store) {}
Addition::Addition(int left, int right, Value* store): Operator(left, right, store) {}

string Addition::generate() const {
    ostringstream oss;
    oss << Operator::generate() << "\nadd   ";
    if(store)
        oss << store->getReg() << ", ";
    oss << left->getReg() << ", " << right->getReg();
    return oss.str();
}

string Addition::getReg() const {
    return store->getReg();
}

Subtraction::Subtraction(Value*left, Value* right, Value* store): Operator(left, right, store) {}
Subtraction::Subtraction(int left, int right, Value* store): Operator(left, right, store) {}

string Subtraction::generate() const {
    ostringstream oss;
    oss << Operator::generate() << "\nsub   ";
    if(store)
        oss << store->getReg() << ", ";
    oss << left->getReg() << ", " << right->getReg();
    return oss.str();
}
string Subtraction::getReg() const {
    return store->getReg();
}

Multiplication::Multiplication(Value*left, Value* right, Value* store): Operator(left, right, store) {}
Multiplication::Multiplication(int left, int right, Value* store) : Operator(left, right, store) {}

string Multiplication::generate() const {
    ostringstream oss;
    oss << Operator::generate() << "\nimul   ";
    if(store)
        oss << store->getReg() << ", ";
    oss << left->getReg() << ", " << right->getReg();
    return oss.str();
}
string Multiplication::getReg() const {
    return store->getReg();
}

Division::Division(Value* left, Value* right, Value* store) : Operator(left, right, store) {}
Division::Division(int left, int right, Value* store) : Operator(left, right, store) {}

string Division::generate() const { //division works differently
    ostringstream oss;
    if(dynamic_cast<Operator*>(right) != nullptr)
        oss << ((Operator*)right)->generate();
    oss << Assignment(&Register::rax, left).generate() << "\ndiv    " << right->getReg();
    // if(store)
    //     oss << Assignment(store, new Value("rax")).generate();
    return oss.str();
}
string Division::getReg() const {
    return Register::rax.getReg();
}

Reminder::Reminder(Value* left, Value* right, Value* store): Operator(left, right, store) {}

string Reminder::generate() const {
    ostringstream oss;
    if(dynamic_cast<Operator*>(right) != nullptr)
        oss << ((Operator*)right)->generate();
    oss << Assignment(&Register::rax, left).generate() << "\ndiv    " << right->getReg();
    // if(store)
    //     oss << Assignment(store, new Value("rdx")).generate();
    return oss.str();
}
string Reminder::getReg() const {
    return Register::rdi.getReg();
}

Assignment::Assignment(Value* into, Value* value): into(into), value(value) {}
string Assignment::generate() const {
    string r = "";
    if(dynamic_cast<Operator*>(value) != nullptr)
        r = ((Operator*)value)->generate();

    return r + "\nmov   " + into->getReg() + ", " + value->getReg();
}
Assignment::~Assignment() {
    value->Free();
}

Parser::Parser(const vector<Lexer::Token>& tokens) : tokens(tokens) {}

void Register::Free() {
    registers.push(this);
}
Value* Register::Get() {
    Value* val = registers.top();
    registers.pop();
    return val;
}
void Register::Init() {
    registers.push(&rbx);
    registers.push(&r10);
    registers.push(&r11);
    registers.push(&r12);
    registers.push(&r13);
    registers.push(&r14);
    registers.push(&r15);
}

stack<Value*> Register::registers; //TODO: maybe remove
Value Register::rbp("rbp"), Register::rax("rax"), Register::rdi("rdi"), Register::rbx("rbx"), Register::r10("r10"), Register::r11("r11"), Register::r12("12"), Register::r13("r13"), Register::r14("r14"), Register::r15("r15");

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
    switch (tokens[i].type) {
        case Lexer::TokenType::INT_LIT:
            return new Value(ParseInt(tokens[i++]));
        case Lexer::TokenType::IDENTIFIER:
            cerr << "unimplemented1" << endl;
            exit(1);
            return nullptr;
        case Lexer::TokenType::OPEN_PAREN:
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
        node = Lexer::TokenType::MULTIPLICATION == tokens[i].type ? (Operator*)new Multiplication(node, parseFactor(tokens, ++i), Register::Get()) : (Operator*)new Division(node, parseFactor(tokens, ++i), Register::Get());
    
    return node;
}

Value* parseExpression(const vector<Lexer::Token>& tokens, size_t& i) {
    Value* node = parseTerm(tokens, i);
    
    while(tokens[i].type == Lexer::TokenType::ADDITION || tokens[i].type == Lexer::TokenType::SUBTRACTION)
    node = Lexer::TokenType::ADDITION == tokens[i].type ? (Value*)new Addition(node, parseTerm(tokens, ++i), Register::Get()) : (Value*)new Subtraction(node, parseTerm(tokens, ++i), Register::Get());

    return node;
}

vector<Instruction*> Parser::Parse() { //what did i do to my constants
    Register::Init();
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
                cerr << "unimplemented2" << endl;
                break;
        }
    }
    if(stack)
        instructions.insert(instructions.begin(), new Subtraction(&Register::rbp, new Value(stack)));
    return instructions;
}
}