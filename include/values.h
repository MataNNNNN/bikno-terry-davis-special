#pragma once

#include <sstream>
#include <string>
#include <memory>

using std::string, std::shared_ptr, std::ostringstream;

class LValue;

class RValue {
    public:
        virtual string getRef() = 0;
        virtual int getSize() = 0;
        // virtual operator string() ;
        virtual ostringstream& generate(shared_ptr<LValue> into, ostringstream& oss);
        // virtual ostringstream& assign(shared_ptr<LValue> into, ostringstream& oss);
};

class LValue : public RValue {
    public:
        virtual string getRef() = 0;
        virtual string getRef(int size) = 0;
};

class Register : public LValue {
    public:
        static shared_ptr<Register> registers[9];
        static shared_ptr<Register> a, b, c, d, si, di, bp, sp, r8, r9, r10, r11, r12, r13, r14, r15;
        static shared_ptr<Register> get(int size);

        string n8, n16, n32, n64;
        int size;

        Register(string n8, string n16, string n32, string n64);

        string getRef()  override;
        string getRef(int size)  override;
        int getSize() override;
};

class Address : public LValue {
    public:
        int offset, size;
        shared_ptr<Register> reg;

        Address(int offset, int size, shared_ptr<Register> reg);

        string getRef() override;
        string getRef(int size) override;
        int getSize() override;
};

class Constant : public RValue {
    public:
        int val, size;

        Constant(int val, int size);

        string getRef() override;
        int getSize() override;
        ostringstream& generate(shared_ptr<LValue> into, ostringstream& oss) override;
};