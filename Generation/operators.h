#pragma once

#include <sstream>

#include "values.h"

using std::ostringstream;

class Operator : public Value {
    public:
        shared_ptr<Value> left, right, store;

        Operator(shared_ptr<Value> left, shared_ptr<Value> right);

        virtual ostringstream& generate(shared_ptr<Value> store, ostringstream& oss) = 0;
        string getRef() const override;
};

class Addition : public Operator {
    public:
        Addition(shared_ptr<Value> left, shared_ptr<Value> right);

        ostringstream& generate(shared_ptr<Value> store, ostringstream& oss) override;
};

class Subtraction : public Operator {
    public:
        Subtraction(shared_ptr<Value> left, shared_ptr<Value> right);

        ostringstream& generate(shared_ptr<Value> store, ostringstream& oss) override;
};

class Multiplication : public Operator {
    public:
        Multiplication(shared_ptr<Value> left, shared_ptr<Value> right);

        ostringstream& generate(shared_ptr<Value> store, ostringstream& oss) override;
};

class Division : public Operator {
    public:
        Division(shared_ptr<Value> left, shared_ptr<Value> right);

        ostringstream& generate(shared_ptr<Value> store, ostringstream& oss) override;
};

class Remainder : public Operator {
    public:
        Remainder(shared_ptr<Value> left, shared_ptr<Value> right);

        ostringstream& generate(shared_ptr<Value> store, ostringstream& oss) override;
};