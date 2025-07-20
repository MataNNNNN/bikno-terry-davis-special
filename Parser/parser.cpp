#include <iostream>
#include <stack>
#include <cmath>
#include <cassert>

#include "parser.h"
#include "operators.h"

using std::runtime_error, std::make_shared, std::to_string, std::make_unique;

int ParseInt(Lexer::Token& token) {
    if(token.type != Lexer::TokenType::INT_LIT || !token.value.has_value())
        throw runtime_error("expected integer");
    
    int r = 0;
    for(size_t i = 0; i < token.value.value().size(); i += 7)
        r = r * 10 + token.value.value()[i] - '0';
    return r;
}

shared_ptr<RValue> Parser::parseInner() {
    shared_ptr<RValue> t = nullptr;
    bool negative = false;
    if(tokens[i].type == Lexer::TokenType::SUBTRACTION) {
        negative = true; //later
        i++;
    }

    switch (tokens[i].type) {
        case Lexer::TokenType::INT_LIT:
            t = make_shared<Constant>(ParseInt(tokens[i]) * (negative ? -1 : 1), 1);
            break;
        case Lexer::TokenType::OPEN_PAREN:
            i++;
            t = parseExpression();
            break;
        case Lexer::TokenType::IDENTIFIER:
            assert(tokens[i].value.has_value() && "yeaheya");
            
            t = variables.at(tokens[i].value.value());
            break;
        default:
            throw runtime_error("wth: " + to_string(i));
            break;
    }
    i++;
    return t;
}


shared_ptr<RValue> Parser::parseTerm() {
    shared_ptr<RValue> node = parseInner();
    while(tokens[i].type == Lexer::TokenType::MULTIPLICATION || tokens[i].type == Lexer::TokenType::DIVISION || tokens[i].type == Lexer::TokenType::REMAINDER) {
        if(tokens[i].type == Lexer::TokenType::MULTIPLICATION) {
            i++;
            node = make_shared<Multiplication>(node, parseInner());
        } else if(tokens[i].type == Lexer::TokenType::DIVISION) {
            i++;
            node = make_shared<Division>(node, parseInner());
        } else if(tokens[i].type == Lexer::TokenType::REMAINDER) {
            i++;
            node = make_shared<Remainder>(node, parseInner());
        }
    }
    return node;
}

shared_ptr<RValue> Parser::parseExpression() {
    shared_ptr<RValue> node = parseTerm();
    while(tokens[i].type == Lexer::TokenType::ADDITION || tokens[i].type == Lexer::TokenType::SUBTRACTION) {
        if(tokens[i].type == Lexer::TokenType::ADDITION) {
            i++;
            node = make_shared<Addition>(node, parseTerm());
        }
        else if(tokens[i].type == Lexer::TokenType::SUBTRACTION) {
            i++;
            node = make_shared<Subtraction>(node, parseTerm());
        }
    }
    return node;
}

vector<unique_ptr<Instruction>> Parser::ParseScope() {
    vector<unique_ptr<Instruction>> instructions {};

    for(; i < tokens.size(); i++) {
        switch (tokens[i].type) {
            case Lexer::TokenType::RETURN:
                i++;
                instructions.push_back(make_unique<Return>(parseExpression()));
                break;
            case Lexer::TokenType::INT_TYPE: {
                if(tokens[i+3].type != Lexer::TokenType::IDENTIFIER || !tokens[i+3].value.has_value())
                    throw runtime_error("adi brotfeld declaration: " + to_string(i));
                
                int size = ParseInt(tokens[i + 2]);
                stack += stack % size + size;
                variables.emplace(tokens[i += 3].value.value(), make_shared<Address>(-stack, size, Register::bp));
                break;
            }
            case Lexer::TokenType::ASSIGNMENT: {
                if(tokens[i-1].type != Lexer::TokenType::IDENTIFIER || !tokens[i-1].value.has_value() || variables.find(tokens[i-1].value.value()) == variables.end())
                    throw runtime_error("assignment gone wrong: " + to_string(i));

                auto s = variables.at(tokens[i++-1].value.value());
                instructions.push_back(make_unique<Assignment>(s, parseExpression()));
                break;
            }
            default:
                throw runtime_error("unimplemented/unexpected at " + to_string(tokens[i].line));
        }
    }
    return instructions;
}

Parser::Parser(vector<Lexer::Token>& tokens) : tokens(tokens) {}
vector<unique_ptr<Instruction>> Parser::Parse() {
    vector<unique_ptr<Instruction>> instructions {};
    i = 0;
    stack = 0;
    vector<unique_ptr<Instruction>> start = ParseScope();
    start.insert(start.begin(), make_unique<PlainASM>("\nsub    rsp, 16"));
    start.insert(start.begin(), make_unique<Assignment>(Register::bp, Register::sp));

    instructions.push_back(make_unique<Label>("_start", std::move(start)));

    return instructions;
}

// struct Scope {
//     int a;
//     unordered_map<string, shared_ptr<Address>> vars;
// };

// class Scopes : public std::stack<Scope> {
//     public:
//         Scopes() : std::stack<Scope>() {}
//         // gay
//         shared_ptr<Address> Get(string& name) {
//             if(empty())
//                 throw new runtime_error("unknown");
//             if(top().vars.find(name) != top().vars.end())
//                 return top().vars.at(name);
//             Scope s = top();
//             pop();
//             shared_ptr<Address> r = Get(name);
//             push(s);
//             return r;
//         }
// };