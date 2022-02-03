#pragma once
#include <deque>
#include <memory>
#include <string>

#include "AST.h"
#include "Lexer.h"

class Parser {
public:
    std::unique_ptr<AST::Program> parse(const std::string& source);  // main method that parses the source code
    bool processSimple(const std::string& sourceCode);  // TODO(@NayLin_H99, @rootkie): Overall method for the user
};


#ifdef UNIT_TEST
// TODO(@NayLin_H99, @rootkie): Add testing
#endif  // UNIT_TEST
