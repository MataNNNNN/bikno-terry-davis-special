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

template <typename T> class Value : public Instruction { //TODO: retink
    public:
    
        virtual string reg() = 0;
        // Value(Instruction* next = nullptr): Instruction(next) {}
};

template <typename M> class Constant : public Value<M> {
    public:
        M value;
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

template <typename L, typename R, typename M> class Operator : public Value<M> {
    public:
        Value<L>* left;
        Value<R>* right;

        Operator(Value<L>* left, Value<R>* right);
        Operator(L left, R right);
        ~Operator();
};

class Parser {
    protected:
        const vector<Lexer::Token>& tokens;

    public:
        Parser(const vector<Lexer::Token>& tokens);
        
        vector<Instruction*> Parse();
};
}