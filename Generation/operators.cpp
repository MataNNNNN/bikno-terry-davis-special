#include <cassert>

#include "operators.h"
#include "generation.h"
#include "values.h"

using std::dynamic_pointer_cast;

Operator::Operator(shared_ptr<RValue> left, shared_ptr<RValue> right): left(move(left)), right(move(right)) {}

string Operator::getRef() {
    return store->getRef();
}
int Operator::getSize() {
    return std::max(left->getSize(), right->getSize());
}

Addition::Addition(shared_ptr<RValue> left, shared_ptr<RValue> right): Operator(left, right) {}
ostringstream& Addition::generate(shared_ptr<LValue> store, ostringstream& oss) {
    this->store = store;

    left->generate(store, oss);

    if(auto op = dynamic_pointer_cast<Operator>(right))
        op->generate(Register::get(getSize()), oss);
        
    auto chosen = (store == right ? left : right);
    oss << "\nadd    " << store->getRef(chosen->getSize()) << ", " << chosen->getRef();
    return oss;
}

Subtraction::Subtraction(shared_ptr<RValue> left, shared_ptr<RValue> right): Operator(left, right) {}
ostringstream& Subtraction::generate(shared_ptr<LValue> store, ostringstream& oss) {
    this->store = store;

    left->generate(store, oss);

    if(auto op = dynamic_pointer_cast<Operator>(right))
        op->generate(Register::get(getSize()), oss);
    oss << "\nsub    " << store->getRef(right->getSize()) << ", " << right->getRef();
    return oss;
}

Multiplication::Multiplication(shared_ptr<RValue> left, shared_ptr<RValue> right): Operator(left, right) {}
ostringstream& Multiplication::generate(shared_ptr<LValue> store, ostringstream& oss) {
    this->store = store;

    shared_ptr<LValue> t = dynamic_pointer_cast<Register>(store) ? store : Register::get(getSize());

    left->generate(t, oss);
    
    if(auto op = dynamic_pointer_cast<Operator>(right))
        op->generate(Register::get(getSize()), oss);

    if(right->getSize() == t->getSize())
        oss << "\nimul   " << t->getRef() << ", " << right->getRef();
    else {
        auto ninig = Register::get(t->getSize());
        right->generate(ninig, oss);
        oss << "\nimul   " << t->getRef() << ", " << ninig->getRef();
    }

    if(t != store)
        t->generate(store, oss);
    return oss;
}

Division::Division(shared_ptr<RValue> left, shared_ptr<RValue> right) : Operator(left, right) {}
ostringstream& Division::generate(shared_ptr<LValue> store, ostringstream& oss) {
    this->store = store;

    // shared_ptr<Register> rax = Register::get(8), rdx = Register::get(8);
    // oss << "\nmov    " << rax->getRef() << ", rax\nmov    " << rdx->getRef() << ", rdx\nxor    rdx, rdx\nxor    rax, rax"; //replace with assignments

    left->generate(Register::a, oss);
        
    shared_ptr<Register> r = Register::get(getSize());
    right->generate(r, oss);

    oss << "\ndiv    " << r->getRef();
    // rdx->generate(Register::d, oss);
    store == Register::a || Register::a->generate(store, oss);
    // if(store != Register::a)
    //     Register::a->generate(store, oss);
        // oss << "\nmov    " << store->getRef() << ", rax\nmov    rax, " << rax->getRef();
    return oss;
}

Remainder::Remainder(shared_ptr<RValue> left, shared_ptr<RValue> right) : Operator(left, right) {}
ostringstream& Remainder::generate(shared_ptr<LValue> store, ostringstream& oss) {
    this->store = store;

    // shared_ptr<Register> rax = Register::get(8), rdx = Register::get(8);
    // oss << "\nmov    " << rax->getRef() << ", rax\nmov    " << rdx->getRef() << ", rdx";

    left->generate(Register::a, oss);

    shared_ptr<Register> r = Register::get(getSize());
    right->generate(r, oss);

    oss << "\ndiv    " << r->getRef();
    // rax->generate(Register::a, oss);
    store == Register::d || Register::d->generate(store, oss);
    // if(store != Register::d)
    //     oss << "\nmov    " << store->getRef() << ", rdx\nmov    rdx, " << rdx->getRef();
    return oss;
}