#include <unordered_map>
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

class QueryObject {
    std::unordered_map<std::string, DESIGN_ENTITY> declarations;
};