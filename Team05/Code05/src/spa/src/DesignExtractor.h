#pragma once

#include "Parser/Ast.h"
#include <set>
#include <deque>
#include <map>
#include <variant>
#include <utility>
#include <memory>


using muTable = std::set<std::pair<std::variant<std::string, int>, std::string>>;  // modifies or uses table
using sTable = std::set<std::string>; // string only table
using Table = std::variant<muTable, sTable>;

class PKBStub {
public:
	std::map<std::string, Table> tables;

	PKBStub() {
		tables["variables"].emplace<sTable>();
		tables["modifies"].emplace<muTable>();
	}

	// Placeholder method for interfacing with PKB
	void insert(std::string tableName, std::string value);
	void insert(std::string tableName, std::pair<int, std::string> relationship);
	void insert(std::string tableName, std::pair<std::string, std::string> relationship);
};

/**
 * A foundation for all design extractor. Performs depth-first traversal on the AST and do nothing.
 * Children classes can inherit this class and override the specific methods that they want to perform
 * actions at.
 */
struct TreeWalker : public AST::ASTNodeVisitor {
	// one day I may be brave enough to use templates like https://www.foonathan.net/2017/12/visitors/
	// classic visitor design pattern will suffice for now.
	virtual void visit(const AST::Program& node) override {};
	virtual void visit(const AST::Procedure& node) override {};
	virtual void visit(const AST::StmtLst& node) override {};
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
	virtual void enterContainer(std::variant<int, std::string> containerId) override {};
	virtual void exitContainer() override {};
};


/**
 * Base class of all design extractors. Adds a PKB adaptor during construction
 */
class Extractor : public TreeWalker {
protected:
	std::shared_ptr<PKBStub> pkb;
public:
	Extractor(std::shared_ptr<PKBStub> pkb) : pkb(pkb) {};
};

class VariableExtractor : public Extractor {
public:
	using Extractor::Extractor;
	void visit(const AST::Var& node) override;
	sTable getVars() {
		return std::get<std::set<std::string>>(pkb->tables["variables"]);
	}
};

class ModifiesExtractor : public Extractor {
	std::deque<int> containerNumber;
	std::string currentProcedureName;

	void cascadeToContainer(std::string &varName);

public:
	using Extractor::Extractor;

	void visit(const AST::Read& node) override;
	void visit(const AST::Assign& node) override;
	void enterContainer(std::variant<int, std::string> containerId) override;
	void exitContainer() override;

	muTable getModifies() {
		return std::get<muTable>(pkb->tables["modifies"]);
	}
};

