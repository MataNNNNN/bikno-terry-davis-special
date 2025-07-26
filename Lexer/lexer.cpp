#include <iostream>

#include "lexer.h"

using std::runtime_error;

Lexer::Lexer(ifstream& file) : file(file) {}

void Lexer::Token::print() {
    std::cout << (int)type << " " << value.value_or("no val") << std::endl;
}

void PushToken(const string& line, vector<Lexer::Token>& tokens, size_t i, size_t last, size_t lineNum) {
    static const unordered_map<string, Lexer::TokenType> keywords {
        {"↩️", Lexer::TokenType::RETURN},
        {"🔢", Lexer::TokenType::INT_TYPE},
        {"🔄️", Lexer::TokenType::LOOP},
        {"🤔", Lexer::TokenType::IF}
    };

    if(i > last) {
        string str = line.substr(last, i - last);
        if(str.size() > 1 && isdigit(str[0]) && str[1] < 0)
            tokens.emplace_back(lineNum, Lexer::TokenType::INT_LIT, str);
        // else if(str.size() > 1 && str[0] > 0 && str[1] > 0) //TODO: make it goated
        //     tokens.push_back({Lexer::TokenType::STRING_LIT, str});
        else if(keywords.find(str) != keywords.end())
            tokens.emplace_back(lineNum, keywords.at(str));
        else
            tokens.emplace_back(lineNum, Lexer::TokenType::IDENTIFIER, str);
    } else
        std::cout << "empty string " << last << " " << i << std::endl;
}

static const unordered_map<int, Lexer::TokenType> symbols {
    {'✊', Lexer::TokenType::SEMICOLON},
    {'➕', Lexer::TokenType::ADDITION},
    {'➖', Lexer::TokenType::SUBTRACTION},
    {'✖', Lexer::TokenType::MULTIPLICATION},
    {'➗', Lexer::TokenType::DIVISION},
    {'💯', Lexer::TokenType::SIZE_OPERATOR},
    {'☝', Lexer::TokenType::POINTER},
    {'🛄', Lexer::TokenType::ARRAY},
    {'👈', Lexer::TokenType::ASSIGNMENT},
    {'🤝', Lexer::TokenType::INTO},
    {'🫸', Lexer::TokenType::OPEN_PAREN},
    {'🫷', Lexer::TokenType::CLOSE_PAREN},
    {'🧮', Lexer::TokenType::REMAINDER},
    {'📈', Lexer::TokenType::L},
    {'📉', Lexer::TokenType::G},
    {'🟰', Lexer::TokenType::EQUAL}
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
                if(ch == '🟰' && (tokens.back().type == TokenType::L || tokens.back().type == TokenType::G))
                    tokens.back().type = tokens.back().type == TokenType::L ? TokenType::LE : TokenType::GE;
                else
                    tokens.emplace_back(lineNum, symbols.at(ch));
            }
            i += count;
        }
        PushToken(line, tokens, i, last, lineNum);
    }
    return tokens;
}