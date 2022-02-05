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



enum class StatementType {
    Statement, Assignment, While, If, Read, Print, Call
};

enum class PKBType {
    STATEMENT, VARIABLE, PROCEDURE, CONST
};


struct PKBField {
    PKBType tag; // const field members?
    bool isConcrete;
    std::variant<, VAR_NAME, PROC_NAME, CONST> content;


public:
    PKBField(PKBType type, bool concrete, Content c) : tag(type), isConcrete(concrete), content(c) {}
    PKBField() {}

    bool operator == (const PKBField&) const;
    bool operator < (const PKBField&) const;
};

struct PKBResponse {
    bool isContent;
    union Response {
        std::set<PKBField> content;
        std::set<std::vector<PKBField>> contentList;
    };
}

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
    static PKBResponse getStatements(StatementType);
    static PKBResponse getVariables();
    static PKBResponse getConst();
    static PKBResponse getProcedures();
};

PKBResponse getall(DesignEntity);

std::string PKBFieldToString(PKBField);

std::set<int> processSuchthat(std::vector<std::shared_ptr<RelRef>>, DesignEntity);

std::set<int> processPattern(std::vector<Pattern>, DesignEntity);

std::string processResult(std::set<int>);

std::string evaluate(Query);