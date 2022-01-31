#pragma once

#include "Parser/Ast.h"
#include <set>
#include <stack>


class VariableExtractor : public AST::IOVisitor {
	std::vector<AST::Var> vars;
public:
	void visitRead(const AST::Read& node) override;
	void visitPrint(const AST::Print& node) override;
};

class TreeWalker : public AST::ASTNodeVisitor {
public:
	std::vector<std::string> nodeWalked;
	std::set<std::string> vars;

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
};
