#include <iostream>
#include <unordered_map>
#include <stack>
#include <cassert>

#include "parser.h"
#include "lexer.h"

using namespace std;

namespace Parser {
class PlainASM : Instruction {
    public:
        string s;
        PlainASM(string s) : s(s) {}

        string generate() const override {
            return s;
        }
};

class Value {
    public:
        virtual string getRef() const = 0;
};

class Constant : public Value {
    public:
        int val;
        Constant(int val): val(val) {};
        string getRef() const override {
            return to_string(val);
        }
};

class Register : public Value {
    public:
        static const shared_ptr<Register> registers[9];
        static Register rax, rdi, rsi, rdx, rbp, rsp, rbx, r8, r9, r10, r11, r12, r13, r14, r15;
        static const shared_ptr<Register> get() {
            for(int i = 0; i < 10; i++)
                if(registers[i].unique())
                    return registers[i];
            throw runtime_error("no registers left");
        }

        const string reg;
        Register(const string reg): reg(reg) {}
        string getRef() const override {
            return reg;
        }
};
Register Register::rax("rax"), Register::rdi("rdi"), Register::rsi("rsi"), Register::rdx("rdx"), Register::rbp("rbp"), Register::rsp("rsp"), Register::rbx("rbx"), Register::r8("r8"), Register::r9("r9"), Register::r10("r10"), Register::r11("r11"), Register::r12("r12"), Register::r13("r13"), Register::r14("r14"), Register::r15("r15");
const shared_ptr<Register> Register::registers[9] = {
    make_shared<Register>(Register::r8),
    make_shared<Register>(Register::rbx),
    make_shared<Register>(Register::r9),
    make_shared<Register>(Register::r10),
    make_shared<Register>(Register::r11),
    make_shared<Register>(Register::r12),
    make_shared<Register>(Register::r13),
    make_shared<Register>(Register::r14),
    make_shared<Register>(Register::r15)
};

class Address : public Value {
    public:
        static const char* lengths[4];

        int i;
        const char* size;

        Address(int i, const char* size): i(i), size(size) {}
        Address(int i, int size): i(i), size(lengths[size-1]) {}
        string getRef() const override {
            return size + ("PTR [rbp-" + to_string(i)) + "]";
        }

        ~Address() {
            cout << i << " " << size << endl;
        }
};
const char* Address::lengths[4] {
    "BYTE", //1
    "WORD", //2
    "DWORD", //4
    "QWORD" //8
};

class Operator : public Value {
    public:
        shared_ptr<Value> left, right;
        const Value* store;

        Operator(shared_ptr<Value> left, shared_ptr<Value> right): left(move(left)), right(move(right)) {}

        virtual string generate(const Value* store) = 0;
        string getRef() const override {
            return store->getRef();
        }
};

int ParseInt(const Lexer::Token& token) {
    if(token.type != Lexer::TokenType::INT_LIT || !token.value.has_value()) {
        throw runtime_error("expected integer");
        exit(1);
        return -1;
    }

    int r = 0;
    for(size_t i = 0; i < token.value.value().size(); i += 7)
        r = r * 10 + token.value.value()[i] - '0';
    return r;
}

class Assignment : public Instruction {
    public:
        const Value* into;
        shared_ptr<Value> value; //TODO: retink

        Assignment(const Value* into, shared_ptr<Value> value): into(into), value(value) {}
        string generate() const override {
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(value))
                return op->generate(into);
            return "\nmov    " + into->getRef() + ", " + value->getRef();
        }
};

class Return : public Instruction {
    public:
        const shared_ptr<Value> code;

        Return(const shared_ptr<Value> code): code(code) {}
        string generate() const override {
            return Assignment(&Register::rax, code).generate();// + "\npop rbp\nret\n";
        }
};

class Addition : public Operator {
    public:
        Addition(shared_ptr<Value> left, shared_ptr<Value> right): Operator(left, right) {}

        string generate(const Value* store) override {
            ostringstream oss;
            this->store = store;
            assert(store && "fucked up");
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(left))
                oss << op->generate(store);
            else if(store != left.get())
                oss << Assignment(store, left).generate();

            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(right))
                oss << op->generate(Register::get().get());
            oss << "\nadd    " << store->getRef() << ", " << right->getRef();
            return oss.str();
        }
};

class Subtraction : public Operator {
    public:
        Subtraction(shared_ptr<Value> left, shared_ptr<Value> right): Operator(left, right) {}

        string generate(const Value* store) override {
            ostringstream oss;
            this->store = store;

            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(left))
                oss << op->generate(store);
            else if(store != left.get())
                oss << Assignment(store, left).generate();

            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(right))
                oss << op->generate(Register::get().get());
            oss << "\nsub    " << store->getRef() << ", " << right->getRef();
            return oss.str();
        }
};

class Multiplication : public Operator {
    public:
        Multiplication(shared_ptr<Value> left, shared_ptr<Value> right): Operator(left, right) {}

        string generate(const Value* store) override {
            ostringstream oss;
            this->store = store;
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(left))
                oss << op->generate(store);
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(right))
                oss << op->generate(Register::get().get());
            oss << Assignment(store, left).generate() << "\nimul   " << store->getRef() << ", " << right->getRef();
            return oss.str();
        }
};

class Division : public Operator {
    public:
        Division(shared_ptr<Value> left, shared_ptr<Value> right) : Operator(left, right) {}

        string generate(const Value* store) override {
            ostringstream oss;
            this->store = store;

            shared_ptr<Value> rax = Register::get(), rdx = Register::get();
            oss << "\nmov    " << rax->getRef() << ", rax\nmov    " << rdx->getRef() << ", rdx\nxor    rdx, rdx"; //replace with assignments
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(left))
                oss << op->generate(&Register::rax);
            else
                oss << Assignment(&Register::rax, left).generate();
            shared_ptr<Register> r = Register::get();
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(right))
                oss << op->generate(r.get());
            else
                oss << Assignment(r.get(), right).generate();
            oss << "\ndiv    " << r->getRef() << Assignment(&Register::rdx, rdx).generate();
            if(store != &Register::rax)
                oss << "\nmov    " << store->getRef() << ", rax\nmov    rax, " << rax->getRef();
            return oss.str();
        }
};

class Reminder : public Operator {
    public:
        Reminder(shared_ptr<Value> left, shared_ptr<Value> right) : Operator(left, right) {}

        string generate(const Value* store) override {
            ostringstream oss;
            this->store = store;

            shared_ptr<Value> rax = Register::get(), rdx = Register::get();
            oss << "\nmov    " << rax->getRef() << ", rax\nmov    " << rdx->getRef() << ", rdx";
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(left))
                oss << op->generate(&Register::rax);
            else
                oss << Assignment(&Register::rax, left).generate();
            shared_ptr<Register> r = Register::get();
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(right))
                oss << op->generate(r.get());
            else
                oss << Assignment(r.get(), right).generate();
            oss << "\ndiv    " << r->getRef() << Assignment(&Register::rax, rax).generate();
            if(store != &Register::rdx)
                oss << "\nmov    " << store->getRef() << ", rdx\nmov    rdx" << rdx->getRef();
            return oss.str();
        }
};

shared_ptr<Value> parseExpression(const vector<Lexer::Token>& tokens, int& i);

shared_ptr<Value> parseInner(const vector<Lexer::Token>& tokens, int& i) {
    shared_ptr<Value> t = nullptr;
    if(tokens[i].type == Lexer::TokenType::INT_LIT)
        t = make_shared<Constant>(ParseInt(tokens[i++]));
    else if(tokens[i].type == Lexer::TokenType::OPEN_PAREN) {
        t = parseExpression(tokens, ++i);
        i++;
    } else if(tokens[i].type == Lexer::TokenType::IDENTIFIER)
        throw runtime_error("unimplemented: " + to_string(i));
    else
        throw runtime_error("wth: " + to_string(i));
    return move(t);
}


shared_ptr<Value> parseTerm(const vector<Lexer::Token>& tokens, int& i) {
    shared_ptr<Value> node = parseInner(tokens, i);
    while(tokens[i].type == Lexer::TokenType::MULTIPLICATION || tokens[i].type == Lexer::TokenType::DIVISION) //TODO: Reminder
        if(tokens[i].type == Lexer::TokenType::MULTIPLICATION)
            node = make_shared<Multiplication>(node, parseInner(tokens, ++i));
        else if(tokens[i].type == Lexer::TokenType::DIVISION) {
            node = make_shared<Division>(node, parseInner(tokens, ++i));
        }
    return move(node);
}

shared_ptr<Value> parseExpression(const vector<Lexer::Token>& tokens, int& i) {
    shared_ptr<Value> node = parseTerm(tokens, i);
    while(tokens[i].type == Lexer::TokenType::ADDITION || tokens[i].type == Lexer::TokenType::SUBTRACTION)
        if(tokens[i].type == Lexer::TokenType::ADDITION)
            node = make_shared<Addition>(node, parseTerm(tokens, ++i));
        else if(tokens[i].type == Lexer::TokenType::SUBTRACTION)
            node = make_shared<Subtraction>(node, parseTerm(tokens, ++i));
    return move(node);
}

Parser::Parser(const vector<Lexer::Token>& tokens) : tokens(tokens) {}
vector<unique_ptr<Instruction>> Parser::Parse() {
    vector<unique_ptr<Instruction>> instructions {
        // make_unique<Instruction>(PlainASM("push   rbp")),
        // make_unique<Instruction>(Assignment(&Register::rbp, &Register::rsp))
    };
    unordered_map<string, shared_ptr<Address>> variables {};
    int stack = 0;

    for(int i = 0; i < tokens.size(); i++) {
        switch (tokens[i].type) {
            case Lexer::TokenType::RETURN:
                instructions.push_back(make_unique<Return>(parseExpression(tokens, ++i)));
                cout << i << endl;
                break;
            case Lexer::TokenType::INT_TYPE: {
                if(tokens[i+3].type != Lexer::TokenType::IDENTIFIER || !tokens[i+3].value.has_value())
                    throw runtime_error("adi brotfeld decleration: " + to_string(i));
                int size = ParseInt(tokens[i + 2]);
                stack += size;
                variables.insert(pair(tokens[i += 3].value.value(), make_shared<Address>(stack, size)));
                break;
            }
            case Lexer::TokenType::ASSIGNMENT:
                if(tokens[i-1].type != Lexer::TokenType::IDENTIFIER || !tokens[i-1].value.has_value() || variables.find(tokens[i-1].value.value()) == variables.end())
                    throw runtime_error("assigment gone wrong: " + to_string(i));
                for(pair d: variables)
                    cout << d.first << ": " << d.second << endl;
                cout << tokens[i-1].value.value() << endl;
                instructions.push_back(make_unique<Assignment>(variables.at("var").get(), parseExpression(tokens, ++i)));
                break;
        }
    }
    return move(instructions);
}
}

//variables.at(tokens[i-1].value.value()).get()