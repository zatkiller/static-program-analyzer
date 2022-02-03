#pragma once
#include <deque>

#include "AST.h"
#include "Lexer.h"

class Parser {
public:
    std::unique_ptr<AST::Program> parse(const std::string& source); //main method that parses the source code
};