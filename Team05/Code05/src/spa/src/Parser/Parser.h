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

private:
    int lineCount = 1;
    std::unique_ptr<AST::Expr> parseConstExpr(std::deque<Token>& tokens);
    std::unique_ptr<AST::Expr> parseVariableExpr(std::deque<Token>& tokens);
    AST::RelOp parseRelOp(std::deque<Token>& tokens);
    AST::CondOp parseCondOp(std::deque<Token>& tokens);
    AST::BinOp parseBinOp(std::deque<Token>& tokens);
    std::unique_ptr<AST::Expr> shuntingYardParser(std::deque<Token>& tokens);
    std::unique_ptr<AST::CondExpr> parseRelExpr(std::deque<Token>& tokens);
    std::unique_ptr<AST::CondExpr> parseCondExpr(std::deque<Token>& tokens);
    std::unique_ptr<AST::Expr> parseExpr(std::deque<Token>& tokens);
    AST::StmtLst parseStmtLst(std::deque<Token>& tokens);
    std::unique_ptr<AST::Statement> parseReadStmt(std::deque<Token>& tokens);
    std::unique_ptr<AST::Statement> parsePrintStmt(std::deque<Token>& tokens);
    std::unique_ptr<AST::Statement> parseAssignStmt(std::deque<Token>& tokens);
    std::unique_ptr<AST::Statement> parseWhileStmt(std::deque<Token>& tokens);
    std::unique_ptr<AST::Statement> parseIfStmt(std::deque<Token>& tokens);
    std::unique_ptr<AST::Procedure> parseProcedure(std::deque<Token>& tokens);
    std::unique_ptr<AST::Program> parseProgram(std::deque<Token>& tokens);

    friend class ParserUnitTest;
};
