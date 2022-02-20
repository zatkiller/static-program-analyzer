#pragma once
#include <deque>
#include <memory>
#include <string>

#include "AST.h"
#include "Lexer.h"

namespace sp {
namespace parser {
extern int lineCount;

namespace expr_parser {
    std::unique_ptr<ast::Expr> parse(std::deque<Token>& tokens);
}  // namespace expr_parser

namespace statement_list_parser {
    ast::StmtLst parse(std::deque<Token>& tokens);
}  // namespace statement_list_parser

namespace cond_expr_parser {
    std::unique_ptr<ast::CondExpr> parse(std::deque<Token>& tokens);
}  // namespace cond_expr_parser

// can expose this under the namespace
std::unique_ptr<ast::Program> parse(const std::string& source);  // main method that parses the source code
std::unique_ptr<ast::Procedure> parseProcedure(std::deque<Token>& tokens);
std::unique_ptr<ast::Program> parseProgram(std::deque<Token>& tokens);

}  // namespace parser
}  // namespace sp
