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
            INT_TYPE,
            SIZE_OPERATOR,
            POINTER,
            ARRAY,
            ASSIGNMENT,
            INTO,
            OPEN_PAREN,
            CLOSE_PAREN
        };

        struct Token {
            TokenType type;
            optional<string> value;

            void print();
        };

        vector<Token> Lex();
    };