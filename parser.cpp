#include <iostream>
#include <stack>
#include <cassert>
#include <cmath>

#include "parser.h"
#include "lexer.h"

using namespace std;

namespace Parser {
class PlainASM : public Instruction {
    public:
        string s;
        PlainASM(string s) : s(s) {}

        string generate() const override {
            return s;
        }
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
        static shared_ptr<Register> rax, rdi, rsi, rdx, rbp, rsp, rbx, r8, r9, r10, r11, r12, r13, r14, r15;
        static const shared_ptr<Register> get() {
            for(int i = 0; i < 10; i++)
                if(registers[i].use_count() <= 2)
                    return registers[i];
            throw runtime_error("no registers left");
        }

        const string reg;
        Register(const string reg): reg(reg) {}
        string getRef() const override {
            return reg;
        }
};
shared_ptr<Register> Register::rax(make_shared<Register>("rax")),
    Register::rdi(make_shared<Register>("rdi")),
    Register::rsi(make_shared<Register>("rsi")),
    Register::rdx(make_shared<Register>("rdx")),
    Register::rbp(make_shared<Register>("rbp")),
    Register::rsp(make_shared<Register>("rsp")),
    Register::rbx(make_shared<Register>("rbx")),
    Register::r8(make_shared<Register>("r8")),
    Register::r9(make_shared<Register>("r9")),
    Register::r10(make_shared<Register>("r10")),
    Register::r11(make_shared<Register>("r11")),
    Register::r12(make_shared<Register>("r12")),
    Register::r13(make_shared<Register>("r13")),
    Register::r14(make_shared<Register>("r14")),
    Register::r15(make_shared<Register>("r15"));

const shared_ptr<Register> Register::registers[9] = {
    Register::r8,
    Register::rbx,
    Register::r9,
    Register::r10,
    Register::r11,
    Register::r12,
    Register::r13,
    Register::r14,
    Register::r15
};

const char* Address::lengths[4] {
    "BYTE", //1
    "WORD", //2
    "DWORD", //4
    "QWORD" //8
};
Address::Address(int i, const char* size): i(i), size(size) {}
Address::Address(int i, int size): i(i), size(lengths[size-1]) {}
string Address::getRef() const {
    return size + (" [rbp-" + to_string(i)) + "]";
}

Address::~Address() {
    std::cout << i << " freed " << size << endl;
}

class Operator : public Value {
    public:
        shared_ptr<Value> left, right, store;

        Operator(shared_ptr<Value> left, shared_ptr<Value> right): left(move(left)), right(move(right)) {}

        virtual string generate(shared_ptr<Value> store) = 0;
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
        shared_ptr<Value> into, value; //TODO: retink

        Assignment(shared_ptr<Value> into, shared_ptr<Value> value): into(into), value(value) {}
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
            return Assignment(Register::rax, code).generate();// + "\npop rbp\nret\n";
        }
};

class Addition : public Operator {
    public:
        Addition(shared_ptr<Value> left, shared_ptr<Value> right): Operator(left, right) {}

        string generate(shared_ptr<Value> store) override {
            ostringstream oss;
            this->store = store;
            assert(store && "fucked up");
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(left))
                oss << op->generate(store);
            else if(store != left)
                oss << Assignment(store, left).generate();

            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(right))
                oss << op->generate(Register::get());
            oss << "\nadd    " << store->getRef() << ", " << right->getRef();
            return oss.str();
        }
};

class Subtraction : public Operator {
    public:
        Subtraction(shared_ptr<Value> left, shared_ptr<Value> right): Operator(left, right) {}

        string generate(shared_ptr<Value> store) override {
            ostringstream oss;
            this->store = store;

            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(left))
                oss << op->generate(store);
            else if(store != left)
                oss << Assignment(store, left).generate();

            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(right))
                oss << op->generate(Register::get());
            oss << "\nsub    " << store->getRef() << ", " << right->getRef();
            return oss.str();
        }
};

class Multiplication : public Operator {
    public:
        Multiplication(shared_ptr<Value> left, shared_ptr<Value> right): Operator(left, right) {}

        string generate(shared_ptr<Value> store) override {
            ostringstream oss;
            this->store = store;
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(left))
                oss << op->generate(store);
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(right))
                oss << op->generate(Register::get());
            oss << Assignment(store, left).generate() << "\nimul   " << store->getRef() << ", " << right->getRef();
            return oss.str();
        }
};

class Division : public Operator {
    public:
        Division(shared_ptr<Value> left, shared_ptr<Value> right) : Operator(left, right) {}

        string generate(shared_ptr<Value> store) override {
            ostringstream oss;
            this->store = store;

            shared_ptr<Value> rax = Register::get(), rdx = Register::get();
            oss << "\nmov    " << rax->getRef() << ", rax\nmov    " << rdx->getRef() << ", rdx\nxor    rdx, rdx"; //replace with assignments
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(left))
                oss << op->generate(Register::rax);
            else
                oss << Assignment(Register::rax, left).generate();
            shared_ptr<Register> r = Register::get();
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(right))
                oss << op->generate(r);
            else
                oss << Assignment(r, right).generate();
            oss << "\ndiv    " << r->getRef() << Assignment(Register::rdx, rdx).generate();
            if(store != Register::rax)
                oss << "\nmov    " << store->getRef() << ", rax\nmov    rax, " << rax->getRef();
            return oss.str();
        }
};

class Remainder : public Operator {
    public:
        Remainder(shared_ptr<Value> left, shared_ptr<Value> right) : Operator(left, right) {}

        string generate(shared_ptr<Value> store) override {
            ostringstream oss;
            this->store = store;

            shared_ptr<Value> rax = Register::get(), rdx = Register::get();
            oss << "\nmov    " << rax->getRef() << ", rax\nmov    " << rdx->getRef() << ", rdx";
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(left))
                oss << op->generate(Register::rax);
            else
                oss << Assignment(Register::rax, left).generate();
            shared_ptr<Register> r = Register::get();
            if(shared_ptr<Operator> op = dynamic_pointer_cast<Operator>(right))
                oss << op->generate(r);
            else
                oss << Assignment(r, right).generate();
            oss << "\ndiv    " << r->getRef() << Assignment(Register::rax, rax).generate();
            if(store != Register::rdx)
                oss << "\nmov    " << store->getRef() << ", rdx\nmov    rdx, " << rdx->getRef();
            return oss.str();
        }
};

shared_ptr<Value> Parser::parseInner() {
    shared_ptr<Value> t = nullptr;
    if(tokens[i].type == Lexer::TokenType::INT_LIT)
        t = make_shared<Constant>(ParseInt(tokens[i]));
    else if(tokens[i].type == Lexer::TokenType::OPEN_PAREN) {
        i++;
        t = parseExpression();
    } else if(tokens[i].type == Lexer::TokenType::IDENTIFIER) {
        assert(tokens[i].value.has_value() && "yeaheya");

        cout << "toktopk " << tokens[i].value.value() << " " << variables.at(tokens[i].value.value()).use_count() << endl;
        t = variables.at(tokens[i].value.value());
    }
    else
        throw runtime_error("wth: " + to_string(i));
    i++;
    return move(t);
}


shared_ptr<Value> Parser::parseTerm() {
    shared_ptr<Value> node = parseInner();
    while(tokens[i].type == Lexer::TokenType::MULTIPLICATION || tokens[i].type == Lexer::TokenType::DIVISION || tokens[i].type == Lexer::TokenType::REMAINDER) {
        if(tokens[i].type == Lexer::TokenType::MULTIPLICATION) {
            i++;
            node = make_shared<Multiplication>(node, parseInner());
        } else if(tokens[i].type == Lexer::TokenType::DIVISION) {
            i++;
            node = make_shared<Division>(node, parseInner());
        } else if(tokens[i].type == Lexer::TokenType::REMAINDER) {
            i++;
            node = make_shared<Remainder>(node, parseInner());
        }
    }
    return move(node);
}

shared_ptr<Value> Parser::parseExpression() {
    shared_ptr<Value> node = parseTerm();
    while(tokens[i].type == Lexer::TokenType::ADDITION || tokens[i].type == Lexer::TokenType::SUBTRACTION) {
        if(tokens[i].type == Lexer::TokenType::ADDITION) {
            i++;
            node = make_shared<Addition>(node, parseTerm());
        }
        else if(tokens[i].type == Lexer::TokenType::SUBTRACTION) {
            i++;
            node = make_shared<Subtraction>(node, parseTerm());
        }
    }
    return move(node);
}

Parser::Parser(const vector<Lexer::Token>& tokens) : tokens(tokens) {}
vector<unique_ptr<Instruction>> Parser::Parse() {
    vector<unique_ptr<Instruction>> instructions {};
    instructions.push_back(make_unique<Assignment>(Register::rbp, Register::rsp));
    instructions.push_back(make_unique<PlainASM>("sub    rsp, 16"));
    int stack = 0;

    for(i = 0; i < tokens.size(); i++) {
        switch (tokens[i].type) {
            case Lexer::TokenType::RETURN:
                i++;
                instructions.push_back(make_unique<Return>(parseExpression()));
                cout << i << endl;
                break;
            case Lexer::TokenType::INT_TYPE: {
                if(tokens[i+3].type != Lexer::TokenType::IDENTIFIER || !tokens[i+3].value.has_value())
                    throw runtime_error("adi brotfeld decleration: " + to_string(i));
                int size = ParseInt(tokens[i + 2]);
                stack += pow(2, size - 1);
                variables.insert(pair(tokens[i += 3].value.value(), make_shared<Address>(stack, size)));
                break;
            }
            case Lexer::TokenType::ASSIGNMENT:
                if(tokens[i-1].type != Lexer::TokenType::IDENTIFIER || !tokens[i-1].value.has_value() || variables.find(tokens[i-1].value.value()) == variables.end())
                    throw runtime_error("assigment gone wrong: " + to_string(i));
                for(pair d: variables)
                    cout << d.first << ": " << d.second << endl;
                cout << tokens[i-1].value.value() << endl;
                i++;
                instructions.push_back(make_unique<Assignment>(variables.at("var"), parseExpression()));
                break;
        }
    }
    return move(instructions);
}
}

//variables.at().get()