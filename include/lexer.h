#pragma once

#include <vector>
#include <optional>
#include <unordered_map>
#include <string>
#include <fstream>

using std::string, std::ifstream, std::optional, std::vector, std::unordered_map;

class Lexer {
        
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
            REMAINDER,
            OPEN_PAREN,
            CLOSE_PAREN,
            INT_TYPE,
            SIZE_OPERATOR,
            POINTER,
            ARRAY,
            ASSIGNMENT,
            INTO,
            LOOP
        };

        struct Token {
            size_t line;
            TokenType type;
            optional<string> value;

            void print();
            inline operator int() {
                return (int)type;
            }
        };

        vector<Token> Lex();

    protected:
        static unordered_map<int, TokenType> symbols;
        ifstream& file;
    };