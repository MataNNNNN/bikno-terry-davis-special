#pragma once

#include <sstream>

#include "values.h"

using std::ostringstream;

class Operator : public RValue {
    public:
        shared_ptr<RValue> left, right;
        shared_ptr<LValue> store;

        Operator(shared_ptr<RValue> left, shared_ptr<RValue> right);

        // virtual ostringstream& generate(shared_ptr<LValue> store, ostringstream& oss) = 0;
        string getRef()  override;
        int getSize()  override;
};

class Addition : public Operator {
    public:
        Addition(shared_ptr<RValue> left, shared_ptr<RValue> right);

        ostringstream& generate(shared_ptr<LValue> store, ostringstream& oss) override;
};

class Subtraction : public Operator {
    public:
        Subtraction(shared_ptr<RValue> left, shared_ptr<RValue> right);

        ostringstream& generate(shared_ptr<LValue> store, ostringstream& oss) override;
};

class Multiplication : public Operator {
    public:
        Multiplication(shared_ptr<RValue> left, shared_ptr<RValue> right);

        ostringstream& generate(shared_ptr<LValue> store, ostringstream& oss) override;
};

class Division : public Operator {
    public:
        Division(shared_ptr<RValue> left, shared_ptr<RValue> right);

        ostringstream& generate(shared_ptr<LValue> store, ostringstream& oss) override;
};

class Remainder : public Operator {
    public:
        Remainder(shared_ptr<RValue> left, shared_ptr<RValue> right);

        ostringstream& generate(shared_ptr<LValue> store, ostringstream& oss) override;
};