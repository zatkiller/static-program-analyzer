#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <set>
#include <algorithm>
#include <iterator>
#include <memory>
#include <variant>
#include <map>

#include "query.h"

enum PKBType {
    STMT_LO,
    VAR_NAME,
    PROC_NAME
};

struct PKBField {
    PKBType tag;
    bool isConcrete; // true if IDENT is used (e.g. Modifies(s, ‘_’))
    int stmtLo;
    std::string vName;
    std::string pName;
    int cName;

    std::string to_string();
};

using sTable = std::set<std::pair<std::string, std::string>>;  // stmt tables
using vTable = std::set<std::string>;  // variable tables
using Table = std::variant<sTable, vTable>;

struct PKBStub {
    std::map<std::string, Table> tables;
    PKBStub() {
        tables["variables"].emplace<vTable>();
        tables["statements"].emplace<sTable>();
    }

    // Placeholder method for interfacing with PKB
    std::vector<PKBField> getAll(DesignEntity);
};


std::set<int> processSuchthat(std::vector<std::shared_ptr<RelRef>>, DesignEntity);

std::set<int> processPattern(std::vector<Pattern>, DesignEntity);

std::string processResult(std::set<int>);

std::string evaluate(Query);