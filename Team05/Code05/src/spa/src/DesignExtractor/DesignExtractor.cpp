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
