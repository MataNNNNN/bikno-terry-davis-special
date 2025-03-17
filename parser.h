#pragma once

#include <vector>

#include "lexer.h"

using namespace std;

// template <typename R, typename ...Args> class Function {
//     public:
//         string name;

//         Function(string name);
// };
namespace Parser{
class Instruction {
    public:
        // Instruction* next;
        
        // Instruction(Instruction* next = nullptr): next(next) {}
        // ~Instruction() {
        //     delete next;
        // }

        virtual string generate() const = 0;
};

class Value {
    public:
        string reg;

        virtual string reg() const { //TODO dont be stupid
            return reg;
        }
        // Value(Instruction* next = nullptr): Instruction(next) {}
};

class Constant : public Value {
    public:
        string value;

        Constant(string value);
        Constant(int t); //TODO
        string reg() const override;
};

class Return : public Instruction {
    public:
        Value* code;
        Return(Value* code);
        ~Return();

        string generate() const override;
};

class Operator : public Value, public Instruction {
    public:
        Value* left;
        Value* right;
        Value* store;
        
        Operator(Value* left, Value* right, Value* store = nullptr);
        Operator(int left, int right, Value* store = nullptr);
        ~Operator();
};

class Addition : public Operator {
    public:
        Addition(Value* left, Value* right, Value* store = nullptr);
        Addition(int left, int right, Value* store = nullptr);

        string generate() const override;
        string reg() const override;
};

class Subtraction : public Operator {
    public:
        Subtraction(Value* left, Value* right, Value* store = nullptr);
        Subtraction(int left, int right, Value* store = nullptr);

        string generate() const override;
        string reg() const override;
};

class Multiplication : public Operator {
    public:
        Multiplication(Value* left, Value* right, Value* store = nullptr);
        Multiplication(int left, int right, Value* store = nullptr);

        string generate() const override;
        string reg() const override;
};

class Division : public Operator {
    public:
        Division(Value* left, Value* right, Value* store = nullptr);
        Division(int left, int right, Value* store = nullptr);

        string generate() const override;
        string reg() const override;
};

class Parser {
    protected:
        const vector<Lexer::Token>& tokens;

    public:
        Parser(const vector<Lexer::Token>& tokens);
        
        vector<Instruction*> Parse();
};
}