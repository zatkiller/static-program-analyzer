#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "DesignExtractor.h"
#include "logging.h"

#define DEBUG Logger(Level::DEBUG) << "DesignExtractor.cpp "

void PKBStub::insert(std::string tableName, std::string value) {
    DEBUG << "Inserting into " << tableName << " " << value;
    auto table = std::get_if<sTable>(&tables[tableName]);
    if (table) {
        table->insert(value);
    } else {
        DEBUG << "Accessing wrong table";
    }
}

void PKBStub::insert(std::string tableName, std::pair<int, std::string> relationship) {
    DEBUG << "Inserting into " << tableName << " (" << relationship.first << "," << relationship.second << ")";
    auto table = std::get_if<muTable>(&tables[tableName]);
    if (table) {
        table->insert(relationship);
    } else {
        DEBUG << "Accessing wrong table";
    }
}

void PKBStub::insert(std::string tableName, std::pair<std::string, std::string> relationship) {
    DEBUG << "Inserting into " << tableName << " (" << relationship.first << "," << relationship.second << ")";
    auto table = std::get_if<muTable>(&tables[tableName]);
    if (table) {
        table->insert(relationship);
    } else {
        DEBUG << "Accessing wrong table";
    }
}

void VariableExtractor::visit(const AST::Var& node) {
    pkb->insert("variables", node.getVarName());
}

void ModifiesExtractor::cascadeToContainer(const std::string& varName) {
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
