#pragma once

#include <sstream>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "lexer.h"

using namespace std;

namespace Parser {
class Instruction {
    public:
        virtual ostringstream& generate(ostringstream& oss) const = 0; 
};

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

class Parser {
    public:
        const vector<Lexer::Token>& tokens;
        unordered_map<string, shared_ptr<Address>> variables;
        int i;

        Parser(const vector<Lexer::Token>& tokens);
        vector<unique_ptr<Instruction>> Parse();

        shared_ptr<Value> parseExpression();
        shared_ptr<Value> parseInner();
        shared_ptr<Value> parseTerm();
};
}