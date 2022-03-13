#include <iterator>

#include "logging.h"
#include "Lexer.h"

namespace sp {
namespace parser {

#ifdef _DEBUG
void logQueue(std::deque<Token> q) {
    std::ostringstream oss;
    oss << "[";
    while (!q.empty()) {
        if (q.size() == 1) {
            std::visit([&](auto&& v) { oss << v; }, q.front().value);
        } else {
            std::visit([&](auto&& v) { oss << v << ", "; }, q.front().value);
        }
        q.pop_front();
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
            while (std::next(it) != source.end() && isalnum(*std::next(it))) {
                it++; lastChar = *it;
                s.push_back(lastChar);
            }
            this->tokens.push_back(Token{TokenType::name, s});
            continue;
        }

        // handle number
        if (isdigit(lastChar)) {
            std::string s;
            s.push_back(lastChar);
            while (std::next(it) != source.end() && isdigit(*std::next(it))) {
                it++; lastChar = *it;
                s.push_back(lastChar);
            }
            if (s.length() > 1 && s[0] == '0') {
                Logger(Level::ERROR) << "Number cannot start with 0!";
                throw std::invalid_argument("Number cannot start with 0!");
            }
            int number = strtod(s.c_str(), nullptr);
            this->tokens.push_back(Token{TokenType::number, number});
            continue;
        }

        // handle special
        this->tokens.push_back(Token{TokenType::special, lastChar});
    }

    // Add EOF to token queue to mark the end
    this->tokens.push_back(Token{TokenType::eof, EOF});

#ifdef _DEBUG
    logQueue(this->tokens);
#endif  // _DEBUG
}

std::deque<Token>& Lexer::getTokens() {
    return tokens;
}

}  // namespace parser
}  // namespace sp
