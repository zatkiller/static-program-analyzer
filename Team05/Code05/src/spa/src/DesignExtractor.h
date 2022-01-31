#pragma once

#include "Parser/Ast.h"


class VariableExtractor : public AST::Visitor {
	std::vector<AST::Var> vars;
public:
	void visit(AST::Read victim);
	void visit(AST::Print victim);
	void visit(AST::Statement victim);
};

