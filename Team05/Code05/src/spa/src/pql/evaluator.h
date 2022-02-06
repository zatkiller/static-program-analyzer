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

#include "pql/query.h"

enum class StatementType {
    Statement, Assignment, While, If, Read, Print, Call
};

enum class PKBType {
    STATEMENT, VARIABLE, PROCEDURE, CONST
};

typedef int CONST;

typedef struct VAR_NAME {
    VAR_NAME(std::string str) : name(str) {}
    std::string name;

public:
    bool operator == (const VAR_NAME&) const;
    bool operator < (const VAR_NAME&) const;
} VAR_NAME;

typedef struct PROC_NAME {
    PROC_NAME(std::string str) : name(str) {}
    std::string name;

public:
    bool operator == (const PROC_NAME&) const;
    bool operator < (const PROC_NAME&) const;
} PROC_NAME;

typedef struct STMT_LO {
    int statementNum;
    StatementType type;

public:
    bool operator == (const STMT_LO&) const;
    bool operator < (const STMT_LO&) const;
} STMT_LO;

using Content = std::variant<STMT_LO, VAR_NAME, PROC_NAME, CONST>;

struct PKBField {
    PKBType tag; // const field members?
    bool isConcrete;
    Content content;


public:
    PKBField(PKBType type, bool concrete, Content c) : tag(type), isConcrete(concrete), content(c) {}
    PKBField() {}

    bool operator == (const PKBField&) const;
    bool operator < (const PKBField&) const;
};


struct PKBStub {
    // Placeholder method for interfacing with PKB
    static std::set<PKBField> getStatements(StatementType);
    static std::set<PKBField> getVariables();
    static std::set<PKBField> getConst();
    static std::set<PKBField> getProcedures();
};

enum class PKBRelationship {
    MODIFIES, USES
};

std::set<PKBField> getall(DesignEntity);

std::string PKBFieldToString(PKBField);

std::string processResult(std::set<PKBField>);

std::string evaluate(Query);