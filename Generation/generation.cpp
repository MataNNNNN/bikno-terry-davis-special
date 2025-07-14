#include "generation.h"
#include "operators.h"

using std::dynamic_pointer_cast;

PlainASM::PlainASM(string s) : s(s) {}
ostringstream& PlainASM::generate(ostringstream& oss)  {
    oss << s;
    return oss;
}

Assignment::Assignment(shared_ptr<LValue> into, shared_ptr<RValue> value): into(into), value(value) {}
ostringstream& Assignment::generate(ostringstream& oss)  {
    return value->generate(into, oss);
}

Return::Return(shared_ptr<RValue> code): code(code) {}
ostringstream& Return::generate(ostringstream& oss)  {
    return code->generate(Register::di, oss);
}

Label::Label(string name, vector<unique_ptr<Instruction>> instructions) : name(name), instructions(std::move(instructions)) {}
ostringstream& Label::generate(ostringstream& oss)  {
    oss << '\n' << name << ':';
    
    for(auto& instruction : instructions)
        instruction->generate(oss);
    return oss;
}