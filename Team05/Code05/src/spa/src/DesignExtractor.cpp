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
}

void TreeWalker::visit(const AST::Procedure& node)
{
	Logger() << "Procedure variable extraction";
}

void TreeWalker::visit(const AST::StmtLst& node)
{
	Logger() << "StmtLst variable extraction";
}

void TreeWalker::visit(const AST::If& node)
{
	Logger() << "If variable extraction";
}

void TreeWalker::visit(const AST::While& node)
{
	Logger() << "While variable extraction";
}

void TreeWalker::visit(const AST::Read& node)
{
	Logger() << "Read variable extraction";
}

void TreeWalker::visit(const AST::Print& node)
{
	Logger() << "Print variable extraction";
}

void TreeWalker::visit(const AST::Assign& node)
{
	Logger() << "Assign variable extraction";
}

void TreeWalker::visit(const AST::Var& node)
{
	Logger() << "Var variable extraction";
}

void TreeWalker::visit(const AST::Const& node)
{
	Logger() << "Const variable extraction";
}

void TreeWalker::visit(const AST::BinExpr& node)
{
	Logger() << "BinExpr variable extraction";
}

void TreeWalker::visit(const AST::RelExpr& node)
{
	Logger() << "RelExpr variable extraction";
}

void TreeWalker::visit(const AST::CondBinExpr& node)
{
	Logger() << "CondBinExpr variable extraction";
}

void TreeWalker::visit(const AST::NotCondExpr& node)
{
	Logger() << "NotCondExpr variable extraction";
}
