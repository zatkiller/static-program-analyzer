#pragma once

#include <stdio.h>
#include "StatementTable.h"
#include "PKBField.h"

// count of an item in a set can only be 0 or 1
bool StatementTable::contains(StatementType type, int statementNumber) {
	/*if (field.tag != PKBType::STATEMENT) {
		throw "Only statements are accepted!";
	}
	STMT_LO stmt = std::get<STMT_LO>(field.content);*/
	return rows.count(StatementRow(type, statementNumber)) == 1;
}

void StatementTable::insert(StatementType type, int statementNumber) {
	/*if (field.tag != PKBType::STATEMENT) {
		throw "Only statements are accepted!";
	}
	STMT_LO stmt = std::get<STMT_LO>(field.content);*/
	rows.insert(StatementRow(type, statementNumber));
}

int StatementTable::getSize() {
	return rows.size();
}

std::vector<STMT_LO> StatementTable::getAllStmt() {
	std::vector<STMT_LO> res;

	for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
		res.push_back(iter->getStmt());
	}
	
	return res;
}

std::vector<STMT_LO> StatementTable::getStmtOfType(StatementType type) {
	std::vector<STMT_LO> res;

	for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
		if (iter->getStmt().type == type) {
			res.push_back(iter->getStmt());
		}
	}

	return res;
}
