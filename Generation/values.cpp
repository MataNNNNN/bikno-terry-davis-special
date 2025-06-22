#include <iostream>

#include "values.h"

using std::make_shared, std::to_string;

Address::Address(int offset, int size, shared_ptr<Register> reg): offset(offset), size(size), reg(reg) {}

string Address::getRef()  {
    return getRef(size);
}
string Address::getRef(int size)  {
    string n;
    switch (size) {
        case 0:
            return getRef(this->size);
        case 1:
            n = "BYTE";
            break;
        case 2:
            n = "WORD";
            break;
        case 4:
            n = "DWORD";
            break;
        case 8:
            n = "QWORD";
            break;
        default:
            throw std::runtime_error("nsdgfdfsg" + std::to_string(size));
    }
    return n + " [" + reg->getRef(8) + std::to_string(offset) + "]";
}
int Address::getSize()  {
    return size;
}

Constant::Constant(int val, int size) : val(val), size(size) {}

string Constant::getRef()  {
    return std::to_string(val);
}
int Constant::getSize()  {
    return size;
}

shared_ptr<Register>
    Register::a(make_shared<Register>("al", "ax", "eax", "rax")),
    Register::b(make_shared<Register>("bl", "bx", "ebx", "rbx")),
    Register::c(make_shared<Register>("cl", "cx", "ecx", "rcx")),
    Register::d(make_shared<Register>("dl", "dx", "edx", "rdx")),
    Register::si(make_shared<Register>("sil", "si", "esi", "rsi")),
    Register::di(make_shared<Register>("dil", "di", "edi", "rdi")),
    Register::bp(make_shared<Register>("bpl", "bp", "ebp", "rbp")),
    Register::sp(make_shared<Register>("spl", "sp", "esp", "rsp")),
    Register::r8(make_shared<Register>("r8b", "r8w", "r8d", "r8")),
    Register::r9(make_shared<Register>("r9b", "r9w", "r9d", "r9")),
    Register::r10(make_shared<Register>("r10b", "r10w", "r10d", "r10")),
    Register::r11(make_shared<Register>("r11b", "r11w", "r11d", "r11")),
    Register::r12(make_shared<Register>("r12b", "r12w", "r12d", "r12")),
    Register::r13(make_shared<Register>("r13b", "r13w", "r13d", "r13")),
    Register::r14(make_shared<Register>("r14b", "r14w", "r14d", "r14")),
    Register::r15(make_shared<Register>("r15b", "r15w", "r15d", "r15"));

 shared_ptr<Register> Register::registers[9] = {
    Register::b,
    Register::r8,
    Register::r9,
    Register::r10,
    Register::r11,
    Register::r12,
    Register::r13,
    Register::r14,
    Register::r15
};

shared_ptr<Register> Register::get(int size) {
for(auto& reg: registers)
    if(reg.use_count() <= 2) {
        reg->size = size;
        return reg;
    }
throw std::runtime_error("no registers available deal with it"); //opopopop
}

Register::Register(string n8, string n16, string n32, string n64) : n8(n8), n16(n16), n32(n32), n64(n64), size(8) {}
string Register::getRef()  {
    return getRef(size);
}

string Register::getRef(int size) {
    // this->size = size;
    switch (size) {
        case 0:
            std::cout << "Register get size 0 i dont remember when i added this or why" << std::endl;
            return getRef(size);
        case 1:
            return n8;
        case 2:
            return n16;
        case 4:
            return n32;
        case 8:
            return n64;
    }
    throw std::runtime_error("brotrobot");
}

int Register::getSize()  {
    return size;
}