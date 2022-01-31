#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "PKB.h"
#include "DesignExtractor.h"
#include "logging.h"

int DesignExtractor () {
	return 0;
}


void insert(std::string tableName, std::string value) {
	Logger() << "Inserting into " << tableName << " " << value;
}

void insert(std::string tableName, std::pair<int, std::string> relationship) {
	Logger() << "Inserting into " << tableName << " (" << relationship.first << "," << relationship.second << ")";
}

void insert(std::string tableName, std::pair<std::string, std::string> relationship) {
	Logger() << "Inserting into " << tableName << " (" << relationship.first << "," << relationship.second << ")";
}

void extractVariable(const AST::Var const& var) {
	insert("Variables", var.getVarName());
}

void extractModifies(std::deque<int> const & containerNums, AST::Read const& stmt) {
	std::string varName = stmt.getVar().getVarName();
	insert("Modifies", std::make_pair<>(stmt.getStmtNo(), varName));
	for (auto stmtNum : containerNums) {
		insert("Modifies", std::make_pair<>(stmtNum, varName));
	}
}

void extractModifies(std::deque<int> const & containerNums, AST::Assign const& stmt) {
	std::string varName = stmt.getLHS().getVarName();
	insert("Modifies", std::make_pair<>(stmt.getStmtNo(), varName));
	for (auto stmtNum : containerNums) {
		insert("Modifies", std::make_pair<>(stmtNum, varName));
	}
}


void TreeWalker::visit(const AST::Program& node) {
	this->nodeWalked.push_back("Program");
}

void TreeWalker::visit(const AST::Procedure& node) {
	this->nodeWalked.push_back("Procedure");
}

void TreeWalker::visit(const AST::StmtLst& node) {
	this->nodeWalked.push_back("StmtLst");
}

void TreeWalker::visit(const AST::If& node) {
	this->nodeWalked.push_back("If");
}

void TreeWalker::visit(const AST::While& node) {
	this->nodeWalked.push_back("While");
}

void TreeWalker::visit(const AST::Read& node) {
	this->nodeWalked.push_back("Read");
	extractModifies(containerStmtNumber, node);
}

void TreeWalker::visit(const AST::Print& node) {
	this->nodeWalked.push_back("Print");
}

void TreeWalker::visit(const AST::Assign& node) {
	this->nodeWalked.push_back("Assign");
	extractModifies(containerStmtNumber, node);
}

void TreeWalker::visit(const AST::Var& node) {
	this->nodeWalked.push_back("Var");
	this->vars.insert(node.getVarName());
	extractVariable(node);
}

void TreeWalker::visit(const AST::Const& node) {
	this->nodeWalked.push_back("Const");
}

void TreeWalker::visit(const AST::BinExpr& node) {
	this->nodeWalked.push_back("BinExpr");
}

void TreeWalker::visit(const AST::RelExpr& node) {
	this->nodeWalked.push_back("RelExpr");
}

void TreeWalker::visit(const AST::CondBinExpr& node) {
	this->nodeWalked.push_back("CondBinExpr");
}

void TreeWalker::visit(const AST::NotCondExpr& node) {
	this->nodeWalked.push_back("NotCondExpr");
}

void TreeWalker::enterContainer(int number) {
	this->containerStmtNumber.push_front(number);
}

void TreeWalker::exitContainer() {
	this->containerStmtNumber.pop_front();
}


