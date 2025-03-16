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

        virtual string generate() = 0;
};

class Value : public Instruction { //TODO: retink a value is not an instruction but whatever
    public:
    
        virtual string reg() = 0;
        // Value(Instruction* next = nullptr): Instruction(next) {}
};

class Constant : public Value {
    public:
        string value;

        Constant(string value);
        Constant(int t); //TODO
        string generate() override;
        string reg() override;
};

class Return : public Instruction {
    public:
        Value* code;
        Return(Value* code);
        ~Return();

        string generate() override;
};

class Operator : public Value {
    public:
        Value* left;
        Value* right;
        
        Operator(Value* left, Value* right);
        Operator(int left, int right);
        ~Operator();
};

class Addition : public Operator {
    public:
        Addition(Value* left, Value* right);
        Addition(int left, int right);

        string generate() override;
        string reg() override;
};

class Subtraction : public Operator {
    public:
        Subtraction(Value* left, Value* right);
        Subtraction(int left, int right);

        string generate() override;
        string reg() override;
};

class Multiplication : public Operator {
    public:
        Multiplication(Value* left, Value* right);
        Multiplication(int left, int right);

        string generate() override;
        string reg() override;
};

class Division : public Operator {
    public:
        Division(Value* left, Value* right);
        Division(int left, int right);

        string generate() override;
        string reg() override;
};

class Parser {
    protected:
        const vector<Lexer::Token>& tokens;

    public:
        Parser(const vector<Lexer::Token>& tokens);
        
        vector<Instruction*> Parse();
};
}