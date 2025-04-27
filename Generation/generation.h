#pragma once

#include <sstream>
#include <vector>
#include <memory>

#include "values.h"

using std::ostringstream, std::vector, std::unique_ptr;

class Instruction {
    public:
        virtual ostringstream& generate(ostringstream& oss) const = 0;
};

class PlainASM : public Instruction {
    public:
        string s;
        PlainASM(string s);

        ostringstream& generate(ostringstream& oss) const override;
};

class Assignment : public Instruction {
    public:
        shared_ptr<Value> into, value;

        Assignment(shared_ptr<Value> into, shared_ptr<Value> value);
        ostringstream& generate(ostringstream& oss) const override;
};

class Return : public Instruction {
    public:
        shared_ptr<Value> code;

        Return(shared_ptr<Value> code);
        ostringstream& generate(ostringstream& oss) const override;
};

class If : public Instruction {
    public:
        vector<unique_ptr<Instruction>> scope;
        shared_ptr<Value> condition;
};

class Label : public Instruction {
    public:
        vector<unique_ptr<Instruction>> instructions;
        string name;

        Label(string name, vector<unique_ptr<Instruction>> instructions);
        ostringstream& generate(ostringstream& oss) const override;
};