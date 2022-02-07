#include "DesignExtractor.h"
#include "logging.h"

#define DEBUG Logger(Level::DEBUG) << "DesignExtractor.cpp "

void PKBAdaptor::insert(std::string tableName, std::string value) {
    DEBUG << "Inserting into " << tableName << " " << value;
    auto table = std::get_if<sTable>(&tables[tableName]);
    if (table) {
        table->insert(value);
        if (tableName == "variables") {
            pkb->insertVariable(value);
        }
    } else {
        DEBUG << "Accessing wrong table";
    }
}

void PKBAdaptor::insert(std::string tableName, std::pair<int, std::string> relationship) {
    DEBUG << "Inserting into " << tableName << " (" << relationship.first << "," << relationship.second << ")";
    auto table = std::get_if<muTable>(&tables[tableName]);
    if (table) {
        table->insert(relationship);
        if (tableName == "modifies") {
            pkb->insertRelationship(
                PKBRelationship::MODIFIES,
                PKBField{ PKBType::STATEMENT, true, Content{STMT_LO{relationship.first}} },
                PKBField{ PKBType::VARIABLE, true, Content{VAR_NAME{relationship.second}} }
            );
        }
    } else {
        DEBUG << "Accessing wrong table";
    }
}

void PKBAdaptor::insert(std::string tableName, std::pair<std::string, std::string> relationship) {
    DEBUG << "Inserting into " << tableName << " (" << relationship.first << "," << relationship.second << ")";
    auto table = std::get_if<muTable>(&tables[tableName]);
    if (table) {
        table->insert(relationship);
        if (tableName == "modifies") {
            pkb->insertRelationship(
                PKBRelationship::MODIFIES,
                PKBField{ PKBType::PROCEDURE, true, Content{PROC_NAME{relationship.first}} },
                PKBField{ PKBType::VARIABLE, true, Content{VAR_NAME{relationship.second}} }
            );
        }
    } else {
        DEBUG << "Accessing wrong table";
    }
}
