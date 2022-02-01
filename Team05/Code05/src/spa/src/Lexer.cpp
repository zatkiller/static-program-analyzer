#include <Lexer.h>
#include <iterator>
#include "logging.h"

#ifdef _DEBUG
void logQueue(std::queue<Token> q) {
    std::ostringstream oss;
    oss << "[";
    while (!q.empty()) {
        if (q.size() == 1) {
            std::visit([&](auto&& v) { oss << v; }, q.front().value);
        } else {
            std::visit([&](auto&& v) { oss << v << ", "; }, q.front().value);
        }
        q.pop();
    }
    oss << "]";

    Logger(Level::DEBUG) << "Lexer.cpp " << "Tokens: " << oss.str();
}
#endif  // _DEBUG


/**
 * Lexes the input string which represents the source code into a queue of tokens.
 * This lex method currently has no failure conditions as all unrecognized symbols
 * are considered as special characters.
 */
void Lexer::lex(const std::string& source) {
    Logger() << "Lexer.cpp " << "Lexing the source code:\n" << source;

    char lastChar = ' ';
    for (auto it = source.begin(); it != source.end(); it++) {
        lastChar = *it;
        if (isspace(lastChar)) continue;

        // handle names
        if (isalpha(lastChar)) {
            std::string s;
            s.push_back(lastChar);
            while (isalnum(*std::next(it))) {
                it++; lastChar = *it;
                s.push_back(lastChar);
            }
            this->tokens.push(Token{TokenType::name, s});
            continue;
        }

        // handle number
        if (isdigit(lastChar)) {
            std::string s;
            s.push_back(lastChar);
            while (isdigit(*std::next(it))) {
                it++; lastChar = *it;
                s.push_back(lastChar);
            }
            int number = strtod(s.c_str(), nullptr);
            this->tokens.push(Token{TokenType::number, number});
            continue;
        }

        // handle special
        this->tokens.push(Token{TokenType::special, lastChar});
    }

    // Add EOF to token queue to mark the end
    this->tokens.push(Token{TokenType::eof, EOF});

#ifdef _DEBUG
    logQueue(this->tokens);
#endif  // _DEBUG
}

std::queue<Token>& Lexer::getTokens() {
    return tokens;
}
