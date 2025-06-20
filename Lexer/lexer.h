#pragma once

#include <vector>
#include <optional>
#include <string>
#include <fstream>

using std::string, std::ifstream, std::optional, std::vector;

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
            TokenType type;
            optional<string> value;

            void print() ;
            inline operator int()  {
                return (int)type;
            }
        };

        vector<Token> Lex();
    };