#pragma once

#include <vector>

#include "lexer.h"

using namespace std;

// enum class Precedence : unsigned char { maybe
//     ADDITION,
//     SUBTRACTION,
//     MULTIPLICATION,
//     DIVISION,
// };

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

template <typename T> class Value : public Instruction { //TODO: retink a value is not an instruction but whatever
    public:
    
        virtual string reg() = 0;
        // Value(Instruction* next = nullptr): Instruction(next) {}
};

template <typename M> class Constant : public Value<M> {
    public:
        M value; //TODO: be goated
        // Constant(int value, Instruction* next = nullptr) {}
        Constant(M value);

        string generate() override;
        string reg() override;
};

class Return : public Instruction {
    public:
        Value<int>* code;
        Return(Value<int>* code);
        ~Return();

        string generate() override;
};

// template <typename L, typename R, typename M> class Operator : public Value<M> {
//     public:
//         Value<L>* left;
//         Value<R>* right;

//         Operator(Value<L>* left, Value<R>* right);
//         Operator(L left, R right);
//         ~Operator();
// };

enum class OperatorType : unsigned char {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    OPEN_PAREN,
    CLOSE_PAREN
};

class Operator : public Value<int> {
    public:
        // const int precedence;
        // const OperatorType type;
        Value<int>* left;
        Value<int>* right;

        // Operator(int precedence, OperatorType type, Value<int>* left, Value<int>* right);
        // Operator(int precedence, OperatorType type, int left, int right);
        Operator(Value<int>* left, Value<int>* right);
        Operator(int left, int right);
        ~Operator();
};

class Addition : public Operator {
    public:
        Addition(Value<int>* left, Value<int>* right);
        Addition(int left, int right);

        string generate() override;
        string reg() override;
};

class Subtraction : public Operator {
    public:
        Subtraction(Value<int>* left, Value<int>* right);
        Subtraction(int left, int right);

        string generate() override;
        string reg() override;
};

class Multiplication : public Operator {
    public:
        Multiplication(Value<int>* left, Value<int>* right);
        Multiplication(int left, int right);

        string generate() override;
        string reg() override;
};

class Division : public Operator {
    public:
        Division(Value<int>* left, Value<int>* right);
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