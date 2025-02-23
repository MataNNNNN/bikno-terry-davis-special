#include <vector>
#include <optional>
#include <string>
#include <fstream>
#include <iostream>

#include "lexer.h"

using namespace std;

Lexer::Lexer(ifstream& file) : file(file) {}

void Lexer::Token::print() {
    cout << (int)type << " " << value.value_or("no val") << endl;
}

void PushToken(const string& line, vector<Lexer::Token>& tokens, int i, int count, int& last) {
    if(i > last) {
        string str = line.substr(last, i - last);
        if(str.size() > 1 && isdigit(str[0]) && str[1] < 0)
            tokens.push_back({Lexer::TokenType::INT_LIT, str});
        else if(str.size() > 1 && str[0] > 0 && str[1] > 0) //TODO: make it goated
            tokens.push_back({Lexer::TokenType::STRING_LIT, str});
        else if(str == "↩️")
            tokens.push_back({Lexer::TokenType::RETURN});
        else if(str == "🔢")
            tokens.push_back({Lexer::TokenType::INT_TYPE});
        else
            tokens.push_back({Lexer::TokenType::IDENTIFIER, str});
    } else
        cout << "empty string " << last << " " << i << endl;
    last = i + count;
}

void PushToken(Lexer::TokenType type, const string& line, vector<Lexer::Token>& tokens, int i, int count, int& last) {
    PushToken(line, tokens, i, count, last);
    tokens.push_back({type});
}

vector<Lexer::Token> Lexer::Lex() {
    vector<Token> tokens {};
    string line;

    while(getline(file, line)) {
        int last = 0, i = 0;
        while (i < line.size()) {
            if(line[i] == 0) //remove ts💔💔
                cerr << "bad bad time should not be here ever!!!!!" << endl;
            if(line[i] == ' ') {
                PushToken(line, tokens, i++, 1, last);
                continue;
            }

            int count = 0;
            if(line[i] >= 0) {
                // if(line.size() > i + 1 && line[i+1] > 0) { remove ts💔💔
                //     PushToken(line, tokens, i, 0, last);
                //     while(line[i] > 0)
                //         i++;
                //     tokens.push_back({TokenType::STRING_LIT, line.substr(last, i - last)});
                //     continue;
                // }
                i++;
                continue;
            } else for(int t = (unsigned char)line[i] >> 4; t > 0; t >>= 1) // else for shel ha goats😎😎😎
                count = (t & 1) * (count + 1);

            string ch = line.substr(i, count);
            if(ch == "💬") { //good for now
                last = line.size();
                break;
            } else if(ch == "✊")//TODO: ☹️☹️☹️
                PushToken(TokenType::SEMICOLON, line, tokens, i, count, last);
            else if(ch == "➕")
                PushToken(TokenType::ADDITION, line, tokens, i, count, last);
            else if(ch == "➖")
                PushToken(TokenType::SUBTRACTION, line, tokens, i, count, last);
            else if(ch == "✖️")
                PushToken(TokenType::MULTIPLICATION, line, tokens, i, count, last);
            else if(ch == "➗")
                PushToken(TokenType::DIVISION, line, tokens, i, count, last);
            else if(ch == "💯")
                PushToken(TokenType::SIZE_OPERATOR, line, tokens, i, count, last);
            else if(ch == "☝")
                PushToken(TokenType::POINTER, line, tokens, i, count, last);
            else if(ch == "🛄") //TODO: לחשוב על זה יותר
                PushToken(TokenType::ARRAY, line, tokens, i, count, last);
            else if(ch == "👈")
                PushToken(TokenType::ASSIGNMENT, line, tokens, i, count, last);
            else if(ch == "🤝")
                PushToken(TokenType::INTO, line, tokens, i, count, last);
            else if(ch == "🫸")
                PushToken(TokenType::OPEN_PAREN, line, tokens, i, count, last);
            else if(ch == "🫷")
                PushToken(TokenType::CLOSE_PAREN, line, tokens, i, count, last);
            i += count;
        }
        PushToken(line, tokens, i, 0, last);
    }
    return tokens;
}