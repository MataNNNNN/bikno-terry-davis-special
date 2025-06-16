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
    if(auto op = dynamic_pointer_cast<Operator>(value))
        op->generate(into, oss);
    else if(auto cons = dynamic_pointer_cast<Constant>(value))
        oss << "\nmov    " << into->getRef() + ", " + cons->getRef();
    else
        oss << "\n" << (into->getSize() == value->getSize() ? "mov    " : "movsx  ") << into->getRef() << ", " << value->getRef();
    return oss;
}

Return::Return(shared_ptr<RValue> code): code(code) {}
ostringstream& Return::generate(ostringstream& oss)  {
    return Assignment(Register::di, code).generate(oss); // + "\npop rbp\nret\n"; whartever
}

Label::Label(string name, vector<unique_ptr<Instruction>> instructions) : name(name), instructions(std::move(instructions)) {}
ostringstream& Label::generate(ostringstream& oss)  {
    oss << '\n' << name << ':';
    
    for(auto& instruction : instructions)
        instruction->generate(oss);
    return oss;
}