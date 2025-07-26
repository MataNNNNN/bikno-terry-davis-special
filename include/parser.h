#pragma once

#include <unordered_map>

#include "lexer.h"
#include "generation.h"

using std::vector, std::unordered_map, std::unique_ptr;

class Parser {
    public:
        const vector<Lexer::Token>& tokens;
        unordered_map<string, shared_ptr<Address>> variables;
        size_t i;
        int stack;

        Parser(const vector<Lexer::Token>& tokens);
        vector<unique_ptr<Instruction>> Parse();

        vector<unique_ptr<Instruction>> ParseScope();

        shared_ptr<RValue> parseExpression();
        shared_ptr<RValue> parseInner();
        shared_ptr<RValue> parseTerm();
};