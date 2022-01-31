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
    auto table = std::get_if<muTable>(&tables[tableName]);
    if (table) {
        table->insert(relationship);
    }
    else {
        Logger() << "Accessing wrong table";
    }
}

void PKBStub::insert(std::string tableName, std::pair<std::string, std::string> relationship) {
    Logger() << "Inserting into " << tableName << " (" << relationship.first << "," << relationship.second << ")";
    auto table = std::get_if<muTable>(&tables[tableName]);
    if (table) {
        table->insert(relationship);
    }
    else {
        Logger() << "Accessing wrong table";
    }
}

void VariableExtractor::visit(const AST::Var& node) {
    //insert("Variables", node.getVarName());
    pkb->insert("variables", node.getVarName());
}

void ModifiesExtractor::cascadeToContainer(std::string &varName) {
    for (auto stmtNo : containerNumber) {
        pkb->insert("modifies", std::make_pair<>(stmtNo, varName));
    }
    if (!currentProcedureName.empty()) {
        pkb->insert("modifies", std::make_pair<>(currentProcedureName, varName));
    }
    
}

void ModifiesExtractor::visit(const AST::Read& node) {
    std::string varName = node.getVar().getVarName();
    pkb->insert("modifies", std::make_pair<>(node.getStmtNo(), varName));
    cascadeToContainer(varName);
}

void ModifiesExtractor::visit(const AST::Assign& node) {
    std::string varName = node.getLHS().getVarName();
    pkb->insert("modifies", std::make_pair<>(node.getStmtNo(), varName));
    cascadeToContainer(varName);
}

void ModifiesExtractor::enterContainer(std::variant<int, std::string> containerId) {
    int *pNum = std::get_if<int>(&containerId);
    if (pNum) {
        containerNumber.push_front(*pNum);
    } else {
        currentProcedureName = std::get<std::string>(containerId);
    }
}

void ModifiesExtractor::exitContainer() {
    containerNumber.pop_front();
}
