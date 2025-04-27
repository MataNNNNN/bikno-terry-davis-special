#include "generation.h"
#include "operators.h"

using std::dynamic_pointer_cast;

PlainASM::PlainASM(string s) : s(s) {}
ostringstream& PlainASM::generate(ostringstream& oss) const {
    oss << s;
    return oss;
}

Assignment::Assignment(shared_ptr<Value> into, shared_ptr<Value> value): into(into), value(value) {}
ostringstream& Assignment::generate(ostringstream& oss) const {
    if(auto op = dynamic_pointer_cast<Operator>(value))
        op->generate(into, oss);
    else
        oss << "\nmov    " << into->getRef() + ", " + value->getRef();
    return oss;
}

Return::Return(const shared_ptr<Value> code): code(code) {}
ostringstream& Return::generate(ostringstream& oss) const {
    return Assignment(Register::rdi, code).generate(oss); // + "\npop rbp\nret\n";
}

Label::Label(string name, vector<unique_ptr<Instruction>> instructions) : name(name), instructions(std::move(instructions)) {}
ostringstream& Label::generate(ostringstream& oss) const {
    oss << '\n' << name << ':';
    
    for(auto& instruction : instructions)
        instruction->generate(oss);
    return oss;
}