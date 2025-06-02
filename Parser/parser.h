#pragma once

#include <unordered_map>

#include "lexer.h"
#include "generation.h"

using std::vector, std::unordered_map, std::unique_ptr;

class Parser {
    public:
         vector<Lexer::Token>& tokens;
        unordered_map<string, shared_ptr<Address>> variables;
        int i, stack;

        Parser( vector<Lexer::Token>& tokens);
        vector<unique_ptr<Instruction>> Parse();

        vector<unique_ptr<Instruction>> ParseScope();

        shared_ptr<RValue> parseExpression();
        shared_ptr<RValue> parseInner();
        shared_ptr<RValue> parseTerm();
};