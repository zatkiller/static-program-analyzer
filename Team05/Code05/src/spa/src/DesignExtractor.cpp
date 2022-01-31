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


void VariableExtractor::visitRead(const AST::Read& node) {
	Logger() << "Read variable extraction";
	
}

void VariableExtractor::visitPrint(const AST::Print& node) {
	Logger() << "Print variable extraction";
}


void TreeWalker::visit(const AST::Program& node) {
	Logger() << "Program node";
	this->nodeWalked.push_back("Program");
}

void TreeWalker::visit(const AST::Procedure& node) {
	Logger() << "Procedure node";
	this->nodeWalked.push_back("Procedure");
}

void TreeWalker::visit(const AST::StmtLst& node) {
	Logger() << "StmtLst node";
	this->nodeWalked.push_back("StmtLst");
}

void TreeWalker::visit(const AST::If& node) {
	Logger() << "If node";
	this->nodeWalked.push_back("If");
}

void TreeWalker::visit(const AST::While& node) {
	Logger() << "While node";
	this->nodeWalked.push_back("While");
}

void TreeWalker::visit(const AST::Read& node) {
	Logger() << "Read node";
	this->nodeWalked.push_back("Read");
}

void TreeWalker::visit(const AST::Print& node) {
	Logger() << "Print node";
	this->nodeWalked.push_back("Print");
}

void TreeWalker::visit(const AST::Assign& node) {
	Logger() << "Assign node";
	this->nodeWalked.push_back("Assign");
}

void TreeWalker::visit(const AST::Var& node) {
	Logger() << "Var node: " << node.getVarName();
	this->nodeWalked.push_back("Var");
	this->vars.insert(node.getVarName());
}

void TreeWalker::visit(const AST::Const& node) {
	Logger() << "Const node";
	this->nodeWalked.push_back("Const");
}

void TreeWalker::visit(const AST::BinExpr& node) {
	Logger() << "BinExpr node";
	this->nodeWalked.push_back("BinExpr");
}

void TreeWalker::visit(const AST::RelExpr& node) {
	Logger() << "RelExpr node";
	this->nodeWalked.push_back("RelExpr");
}

void TreeWalker::visit(const AST::CondBinExpr& node) {
	Logger() << "CondBinExpr node";
	this->nodeWalked.push_back("CondBinExpr");
}

void TreeWalker::visit(const AST::NotCondExpr& node) {
	Logger() << "NotCondExpr node";
	this->nodeWalked.push_back("NotCondExpr");
}
