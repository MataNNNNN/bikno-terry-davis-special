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

template <typename T> class Value : public Instruction {
    public:
    
        virtual string reg() = 0;
        // Value(Instruction* next = nullptr): Instruction(next) {}
};

class Constant : public Value<int> {
    public:
        int value;
        // Constant(int value, Instruction* next = nullptr) {}
        Constant(int value);

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

class Parser {
    protected:
        const vector<Lexer::Token>& tokens;

    public:
        Parser(const vector<Lexer::Token>& tokens);
        
        vector<Instruction*> Parse();
};
}