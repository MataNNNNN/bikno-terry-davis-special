#pragma once

#include <string>
#include <memory>

using std::string, std::shared_ptr;

class Value {
    public:
        virtual string getRef() const = 0;
        // virtual operator string() const;
};

class Address : public Value {
    public:
        static const char* lengths[4];

        int i;
        const char* size;
        Address(int i, const char* size);
        Address(int i, int size);

        string getRef() const override;
};

class Constant : public Value {
    public:
        int val;
        Constant(int val);
        string getRef() const override;
};

class Register : public Value {
    public:
        static const shared_ptr<Register> registers[9];
        static shared_ptr<Register> rax, rdi, rsi, rdx, rbp, rsp, rbx, r8, r9, r10, r11, r12, r13, r14, r15;
        static const shared_ptr<Register> get();

        const string reg;
        Register(const string reg);
        string getRef() const override;
};