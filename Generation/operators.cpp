#include <cassert>

#include "operators.h"
#include "generation.h"
#include "values.h"

using std::dynamic_pointer_cast;

Operator::Operator(shared_ptr<RValue> left, shared_ptr<RValue> right): left(move(left)), right(move(right)) {}
string Operator::getRef() const {
    return store->getRef();
}

Addition::Addition(shared_ptr<RValue> left, shared_ptr<RValue> right): Operator(left, right) {}
ostringstream& Addition::generate(shared_ptr<LValue> store, ostringstream& oss) {
    this->store = store;
    assert(store && "fucked up");

    if(auto op = dynamic_pointer_cast<Operator>(left))
        op->generate(store, oss);
    else if(store != left && store != right)
        Assignment(store, left).generate(oss);

    if(auto op = dynamic_pointer_cast<Operator>(right))
        op->generate(Register::get(), oss);

    oss << "\nadd    " << store->getRef() << ", " << (store == right ? left->getRef() : right->getRef());
    return oss;
}

Subtraction::Subtraction(shared_ptr<RValue> left, shared_ptr<RValue> right): Operator(left, right) {}
ostringstream& Subtraction::generate(shared_ptr<LValue> store, ostringstream& oss) {
    this->store = store;

    if(auto op = dynamic_pointer_cast<Operator>(left))
        op->generate(store, oss);
    else if(store != left)
        Assignment(store, left).generate(oss);

    if(auto op = dynamic_pointer_cast<Operator>(right))
        op->generate(Register::get(), oss);
    oss << "\nsub    " << store->getRef() << ", " << right->getRef();
    return oss;
}

Multiplication::Multiplication(shared_ptr<RValue> left, shared_ptr<RValue> right): Operator(left, right) {}
ostringstream& Multiplication::generate(shared_ptr<LValue> store, ostringstream& oss) {
    this->store = store;

    auto t = dynamic_pointer_cast<Register>(store) ? store : Register::get();

    if(auto op = dynamic_pointer_cast<Operator>(left))
        op->generate(t, oss);
    else
        Assignment(t, left).generate(oss);
    
    if(auto op = dynamic_pointer_cast<Operator>(right))
        op->generate(Register::get(), oss);

    oss << "\nimul   " << t->getRef() << ", " << right->getRef();
    if(t != store)
        Assignment(store, t).generate(oss);
    return oss;
}

Division::Division(shared_ptr<RValue> left, shared_ptr<RValue> right) : Operator(left, right) {}
ostringstream& Division::generate(shared_ptr<LValue> store, ostringstream& oss) {
    this->store = store;

    shared_ptr<Register> rax = Register::get(), rdx = Register::get();
    oss << "\nmov    " << rax->getRef() << ", rax\nmov    " << rdx->getRef() << ", rdx\nxor    rdx, rdx"; //replace with assignments
    if(auto op = dynamic_pointer_cast<Operator>(left))
        op->generate(Register::rax, oss);
    else
        Assignment(Register::rax, left).generate(oss);
    shared_ptr<Register> r = Register::get();
    if(auto op = dynamic_pointer_cast<Operator>(right))
        op->generate(r, oss);
    else
        Assignment(r, right).generate(oss);
    oss << "\ndiv    " << r->getRef();
    Assignment(Register::rdx, rdx).generate(oss);
    if(store != Register::rax)
        oss << "\nmov    " << store->getRef() << ", rax\nmov    rax, " << rax->getRef();
    return oss;
}

Remainder::Remainder(shared_ptr<RValue> left, shared_ptr<RValue> right) : Operator(left, right) {}
ostringstream& Remainder::generate(shared_ptr<LValue> store, ostringstream& oss) {
    this->store = store;

    shared_ptr<Register> rax = Register::get(), rdx = Register::get();
    oss << "\nmov    " << rax->getRef() << ", rax\nmov    " << rdx->getRef() << ", rdx";
    if(auto op = dynamic_pointer_cast<Operator>(left))
        op->generate(Register::rax, oss);
    else
        Assignment(Register::rax, left).generate(oss);
    shared_ptr<Register> r = Register::get();
    if(auto op = dynamic_pointer_cast<Operator>(right))
        op->generate(r, oss);
    else
        Assignment(r, right).generate(oss);
    oss << "\ndiv    " << r->getRef();
    Assignment(Register::rax, rax).generate(oss);
    if(store != Register::rdx)
        oss << "\nmov    " << store->getRef() << ", rdx\nmov    rdx, " << rdx->getRef();
    return oss;
}