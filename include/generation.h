#pragma once

#include <sstream>
#include <vector>
#include <memory>

#include "values.h"

using std::ostringstream, std::vector, std::unique_ptr;

class Instruction {
    public:
        virtual ostringstream& generate(ostringstream& oss) = 0;
};

class PlainASM : public Instruction {
    public:
        string s;
        PlainASM(string s);

        ostringstream& generate(ostringstream& oss) override;
};

class Assignment : public Instruction {
    public:
        shared_ptr<LValue> into;
        shared_ptr<RValue> value;

        Assignment(shared_ptr<LValue> into, shared_ptr<RValue> value);
        ostringstream& generate(ostringstream& oss) override;
};

class Return : public Instruction {
    public:
        shared_ptr<RValue> code;

        Return(shared_ptr<RValue> code);
        ostringstream& generate(ostringstream& oss) override;
};

class Label : public Instruction {
    public:
        string name;
        vector<unique_ptr<Instruction>> instructions;

        Label(string name, vector<unique_ptr<Instruction>> instructions);
        ostringstream& generate(ostringstream& oss) override;
};

class If : public Instruction {
    public:
        shared_ptr<BooleanExpr> condition;
        shared_ptr<Label> body, elseb;

        If(shared_ptr<BooleanExpr> condition, shared_ptr<Label> body, shared_ptr<Label> elseb);
        ostringstream& generate(ostringstream& oss) override;
};