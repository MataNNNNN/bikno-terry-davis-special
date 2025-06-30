#include <iostream>

#include "lexer.h"

using std::runtime_error;

Lexer::Lexer(ifstream& file) : file(file) {}

void Lexer::Token::print()  {
    static string names[] {"int_lit", "return", "semicolon", "identifier", "string_lit", "addition", "subtraction", "multiplication", "division", "remainder", "open_paren", "close_paren", "int_type", "size_operator", "pointer", "array", "assignment", "into"};
    std::cout << names[(int)type] << " " << value.value_or("no val") << std::endl;
}

void PushToken(const string& line, vector<Lexer::Token>& tokens, size_t i, size_t last, size_t lineNum) {
    static  unordered_map<string, Lexer::TokenType> keywords {
        {"↩️", Lexer::TokenType::RETURN},
        {"🔢", Lexer::TokenType::INT_TYPE},
        {"🔄️", Lexer::TokenType::LOOP}
    };

    if(i > last) {
        string str = line.substr(last, i - last);
        if(str.size() > 1 && isdigit(str[0]) && str[1] < 0)
            tokens.emplace_back(lineNum, last, Lexer::TokenType::INT_LIT, str);
        // else if(str.size() > 1 && str[0] > 0 && str[1] > 0) //TODO: make it goated
        //     tokens.push_back({Lexer::TokenType::STRING_LIT, str});
        else if(keywords.find(str) != keywords.end())
            tokens.emplace_back(lineNum, last, keywords.at(str));
        else
            tokens.emplace_back(lineNum, last, Lexer::TokenType::IDENTIFIER, str);
    } else
        std::cout << "empty string " << last << " " << i << std::endl;
}

unordered_map<int, Lexer::TokenType> Lexer::symbols {
    {'✊', TokenType::SEMICOLON},
    {'➕', TokenType::ADDITION},
    {'➖', TokenType::SUBTRACTION},
    {'✖', TokenType::MULTIPLICATION},
    {'➗', TokenType::DIVISION},
    {'💯', TokenType::SIZE_OPERATOR},
    {'☝', TokenType::POINTER},
    {'🛄', TokenType::ARRAY},
    {'👈', TokenType::ASSIGNMENT},
    {'🤝', TokenType::INTO},
    {'🫸', TokenType::OPEN_PAREN},
    {'🫷', TokenType::CLOSE_PAREN},
    {'🧮', TokenType::REMAINDER}
};

vector<Lexer::Token> Lexer::Lex() {
    vector<Token> tokens {};
    string line;
    size_t lineNum;

    while(getline(file, line)) {
        size_t last = 0, i = 0;
        while (i < line.size()) {
            if(line[i] == ' ') {
                PushToken(line, tokens, i++, last, lineNum);
                last = i;
                continue;
            } else if(line[i] > 0) {
                i++;
                continue;
            }

            int count = 0;
            int ch = 0;
            for(int t = (unsigned char)line[i] >> 4; t > 0; t >>= 1)
                if(t&1)
                    ch = (ch << 8) | (unsigned char)line[i + count++];
                else {
                    count = 0;
                    ch = 0;
                }
                
            if(ch == '💬') { //good for now
                last = line.size();
                break;
            } else if(symbols.find(ch) != symbols.end()) {
                PushToken(line, tokens, i, last, lineNum);
                last = i + count;
                tokens.push_back({lineNum, i, symbols.at(ch)});
            }
            i += count;
        }
        PushToken(line, tokens, i, last, lineNum);
    }
    return tokens;
}