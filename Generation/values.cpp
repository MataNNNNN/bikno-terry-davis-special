#include <iostream>

#include "values.h"

using std::make_shared, std::to_string;

const char* Address::lengths[4] {
    "BYTE", //1
    "WORD", //2
    "DWORD", //4
    "QWORD" //8
};

Address::Address(int i, const char* size): i(i), size(size) {}
Address::Address(int i, int size): i(i), size(lengths[size-1]) {}

string Address::getRef() const {
    return size + (" [rbp-" + std::to_string(i)) + "]";
}

Constant::Constant(int val) : val(val) {}
string Constant::getRef() const {
    return std::to_string(val);
}

shared_ptr<Register> Register::rax(make_shared<Register>("rax")),
    Register::rdi(make_shared<Register>("rdi")),
    Register::rsi(make_shared<Register>("rsi")),
    Register::rdx(make_shared<Register>("rdx")),
    Register::rbp(make_shared<Register>("rbp")),
    Register::rsp(make_shared<Register>("rsp")),
    Register::rbx(make_shared<Register>("rbx")),
    Register::r8(make_shared<Register>("r8")),
    Register::r9(make_shared<Register>("r9")),
    Register::r10(make_shared<Register>("r10")),
    Register::r11(make_shared<Register>("r11")),
    Register::r12(make_shared<Register>("r12")),
    Register::r13(make_shared<Register>("r13")),
    Register::r14(make_shared<Register>("r14")),
    Register::r15(make_shared<Register>("r15"));

const shared_ptr<Register> Register::registers[9] = {
    Register::rbx,
    Register::r8,
    Register::r9,
    Register::r10,
    Register::r11,
    Register::r12,
    Register::r13,
    Register::r14,
    Register::r15
};
const shared_ptr<Register> Register::get() {
    for(auto& reg: registers)
        if(reg.use_count() <= 2)
            return reg;
    throw std::runtime_error("no registers available deal with it"); //opopopop
}

Register::Register(const string reg) : reg(reg) {}
string Register::getRef() const {
    return reg;
}