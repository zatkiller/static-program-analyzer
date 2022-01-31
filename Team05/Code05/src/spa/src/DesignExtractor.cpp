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


void PKBStub::insert(std::string tableName, std::string value) {
	Logger() << "Inserting into " << tableName << " " << value;
	auto table = std::get_if<sTable>(&tables[tableName]);
	if (table) {
		table->insert(value);
	} else {
		Logger() << "Accessing wrong table";
	}
}

void PKBStub::insert(std::string tableName, std::pair<int, std::string> relationship) {
	Logger() << "Inserting into " << tableName << " (" << relationship.first << "," << relationship.second << ")";
}

void PKBStub::insert(std::string tableName, std::pair<std::string, std::string> relationship) {
	Logger() << "Inserting into " << tableName << " (" << relationship.first << "," << relationship.second << ")";
}

void VariableExtractor::visit(const AST::Var& node) {
	//insert("Variables", node.getVarName());
	pkb->insert("variables", node.getVarName());
}
