#include <string>

#include "lexer.h"

struct Parser {
    Lexer lexer;

    void parsePql(std::string query);
};