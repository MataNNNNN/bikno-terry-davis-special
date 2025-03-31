#pragma once

#include <sstream>
#include <vector>
#include <memory>
#include <string>

#include "lexer.h"

using namespace std;

namespace Parser {
class Instruction {
    public:
        virtual string generate() const = 0; 
};

class Parser {
    public:
        const vector<Lexer::Token>& tokens;
        Parser(const vector<Lexer::Token>& tokens);
        vector<unique_ptr<Instruction>> Parse();
};
}