#pragma once
#include <queue>

#include "Parser/AST.h"
#include "Lexer.h"

class Parser {
public:
    std::unique_ptr<AST::Program> Parser::parse(const std::string& source); //main method that parses the source code
};

