#include <unordered_map>
#include <vector>
#include <string>

enum class DESIGN_ENTITY {
    STMT,
    READ,
    PRINT,
    WHILE,
    IF,
    ASSIGN,
    VARIABLE,
    CONSTANT,
    PROCEDURE
};

struct Query {
    std::unordered_map<std::string, DESIGN_ENTITY> declarations;
    std::vector<std::string> variables;
    // Need to add clauses
};