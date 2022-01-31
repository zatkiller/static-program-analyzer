#pragma once

#include "Parser/Ast.h"
#include <set>
#include <deque>
#include <utility>

// Placeholder method for interfacing with PKB
void insert(std::string tableName, std::string value);
void insert(std::string tableName, std::pair<int, std::string> relationship);
void insert(std::string tableName, std::pair<std::string, std::string> relationship);

class TreeWalker : public AST::ASTNodeVisitor {
public:
	std::vector<std::string> nodeWalked;
	std::deque<int> containerStmtNumber;
	std::set<std::string> vars;

	// one day I may be brave enough to use templates like https://www.foonathan.net/2017/12/visitors/
	// classic visitor design pattern will suffice for now.
	void visit(const AST::Program& node) override;
	void visit(const AST::Procedure& node) override;
	void visit(const AST::StmtLst& node) override;
	void visit(const AST::If& node) override;
	void visit(const AST::While& node) override;
	void visit(const AST::Read& node) override;
	void visit(const AST::Print& node) override;
	void visit(const AST::Assign& node) override;
	void visit(const AST::Var& node) override;
	void visit(const AST::Const& node) override;
	void visit(const AST::BinExpr& node) override;
	void visit(const AST::RelExpr& node) override;
	void visit(const AST::CondBinExpr& node) override;
	void visit(const AST::NotCondExpr& node) override;
	void enterContainer(int number) override;
	void exitContainer() override;
};
