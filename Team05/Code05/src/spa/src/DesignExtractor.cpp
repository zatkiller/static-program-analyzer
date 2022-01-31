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
}

void TreeWalker::visit(const AST::Print& node) {
	this->nodeWalked.push_back("Print");
}

void TreeWalker::visit(const AST::Assign& node) {
	this->nodeWalked.push_back("Assign");
}

void TreeWalker::visit(const AST::Var& node) {
	this->nodeWalked.push_back("Var");
	this->vars.insert(node.getVarName());
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
