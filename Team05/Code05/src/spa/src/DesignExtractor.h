#pragma once

#include "Parser/Ast.h"
#include <set>
#include <deque>
#include <map>
#include <variant>
#include <utility>
#include <memory>


using ssTable = std::set<std::pair<std::string, std::string>>;
using isTable = std::set<std::pair<int, std::string>>;
using sTable = std::set<std::string>;
using Table = std::variant<ssTable, isTable, sTable>;

class PKBStub {
public:
	std::map<std::string, Table> tables;

	PKBStub() {
		tables["variables"].emplace<sTable>();
		tables["modifies"].emplace<ssTable>();
		tables["modifies"].emplace<isTable>();
	}

	// Placeholder method for interfacing with PKB
	void insert(std::string tableName, std::string value);
	void insert(std::string tableName, std::pair<int, std::string> relationship);
	void insert(std::string tableName, std::pair<std::string, std::string> relationship);
};

// one day I may be brave enough to use templates like https://www.foonathan.net/2017/12/visitors/
// classic visitor design pattern will suffice for now.
class TreeWalker : public AST::ASTNodeVisitor {
public:
	virtual void visit(const AST::Program& node) override {};
	void visit(const AST::Procedure& node) override {};
	void visit(const AST::StmtLst& node) override {};
	virtual void visit(const AST::If& node) override {};
	virtual void visit(const AST::While& node) override {};
	virtual void visit(const AST::Read& node) override {};
	virtual void visit(const AST::Print& node) override {};
	virtual void visit(const AST::Assign& node) override {};
	virtual void visit(const AST::Var& node) override {};
	virtual void visit(const AST::Const& node) override {};
	virtual void visit(const AST::BinExpr& node) override {};
	virtual void visit(const AST::RelExpr& node) override {};
	virtual void visit(const AST::CondBinExpr& node) override {};
	virtual void visit(const AST::NotCondExpr& node) override {};
	virtual void enterContainer(int number) override {};
	virtual void exitContainer() override {};
};

class VariableExtractor : public TreeWalker {
	std::shared_ptr<PKBStub> pkb;
public:
	VariableExtractor(std::shared_ptr<PKBStub> pkb) : pkb(pkb) {};
	void visit(const AST::Var& node) override;
	std::set<std::string> getVars() {
		return std::get<std::set<std::string>>(pkb->tables["variables"]);
	}
};

