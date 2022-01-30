#include "lexer.h"

#include <sstream>

Lexer::Lexer(std::string input) {
    std::stringstream ss(input);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
}