#pragma once

#include <set>
#include <deque>
#include <map>
#include <variant>
#include <utility>
#include <memory>
#include <string>

#include "Parser/AST.h"
#include "Treewalker.h"

using muTable = std::set<std::pair<std::variant<std::string, int>, std::string>>;  // modifies or uses table
using sTable = std::set<std::string>;  // string only table
using Table = std::variant<muTable, sTable>;

/**
 * A PKB adaptor for testing purposes. Will be developed when integrating with PKB implementation
 */
struct PKBStub {
    std::map<std::string, Table> tables;
    PKBStub() {
        tables["variables"].emplace<sTable>();
        tables["modifies"].emplace<muTable>();
    }

    // Placeholder method for interfacing with PKB
    void insert(std::string tableName, std::string value);
    void insert(std::string tableName, std::pair<int, std::string> relationship);
    void insert(std::string tableName, std::pair<std::string, std::string> relationship);
};

/**
 * Base class of all design extractors. Adds a PKB adaptor during construction
 */
class Extractor : public TreeWalker {
protected:
    std::shared_ptr<PKBStub> pkb;
public:
    explicit Extractor(std::shared_ptr<PKBStub> pkb) : pkb(pkb) {}
};
