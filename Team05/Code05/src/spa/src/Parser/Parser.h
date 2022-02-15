#pragma once
#include <deque>
#include <memory>
#include <string>

#include "AST.h"
#include "Lexer.h"

namespace SimpleParser {

namespace ExprParser {
    std::unique_ptr<AST::Expr> parse(std::deque<Token>& tokens);
}

namespace StmtLstParser {
    AST::StmtLst parse(std::deque<Token>& tokens);
}

namespace CondExprParser {
    std::unique_ptr<AST::CondExpr> parse(std::deque<Token>& tokens);
}

class Parser {
public:
// can expose this under the namespace
    std::unique_ptr<AST::Program> parse(const std::string& source);  // main method that parses the source code
private:
    std::unique_ptr<AST::Procedure> parseProcedure(std::deque<Token>& tokens);
    std::unique_ptr<AST::Program> parseProgram(std::deque<Token>& tokens);

    friend class ParserUnitTest;
};

}  // namespace SimpleParser
