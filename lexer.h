#pragma once

#include <vector>
#include <optional>
#include <string>
#include <fstream>

using namespace std;

class Lexer {
    protected:
        ifstream& file;
        
    public:
        Lexer(ifstream& file);

        enum class TokenType : unsigned char {
            INT_LIT,
            RETURN,
            SEMICOLON,
            IDENTIFIER,
            STRING_LIT,
            ADDITION,
            SUBTRACTION,
            MULTIPLICATION,
            DIVISION,
            OPEN_PAREN,
            CLOSE_PAREN,
            INT_TYPE,
            SIZE_OPERATOR,
            POINTER,
            ARRAY,
            ASSIGNMENT,
            INTO,
        };

        struct Token {
            TokenType type;
            optional<string> value;

            void print() const;
            inline operator int() const {
                return (int)type;
            }
        };

        vector<Token> Lex();
    };