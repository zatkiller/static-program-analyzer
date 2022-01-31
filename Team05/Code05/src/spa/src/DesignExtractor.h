#pragma once

#include "Parser/Ast.h"


class VariableExtractor : public AST::IOVisitor {
	std::vector<AST::Var> vars;
public:
	void visitRead(const AST::Read& node) override;
	void visitPrint(const AST::Print& node) override;
};

