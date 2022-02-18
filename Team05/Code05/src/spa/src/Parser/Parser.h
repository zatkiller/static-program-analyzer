#pragma once
#include <deque>
#include <memory>
#include <string>

#include "AST.h"
#include "Lexer.h"

namespace sp {
namespace parser {
extern int lineCount;

namespace ExprParser {
    std::unique_ptr<ast::Expr> parse(std::deque<Token>& tokens);
}

namespace StmtLstParser {
    ast::StmtLst parse(std::deque<Token>& tokens);
}

namespace CondExprParser {
    std::unique_ptr<ast::CondExpr> parse(std::deque<Token>& tokens);
}

// can expose this under the namespace
std::unique_ptr<ast::Program> parse(const std::string& source);  // main method that parses the source code
std::unique_ptr<ast::Procedure> parseProcedure(std::deque<Token>& tokens);
std::unique_ptr<ast::Program> parseProgram(std::deque<Token>& tokens);

}  // namespace parser
}  // namespace sp
