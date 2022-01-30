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

static std::unordered_map<std::string, DESIGN_ENTITY> designEntityMap = {
        { "stmt", DESIGN_ENTITY::STMT },
        { "read", DESIGN_ENTITY::READ } ,
        { "print", DESIGN_ENTITY::PRINT },
        { "while", DESIGN_ENTITY::WHILE },
        { "if", DESIGN_ENTITY::IF },
        { "assign", DESIGN_ENTITY::ASSIGN },
        { "variable", DESIGN_ENTITY::VARIABLE },
        { "constant", DESIGN_ENTITY::CONSTANT },
        { "procedure", DESIGN_ENTITY::PROCEDURE }
};

struct Query {
    std::unordered_map<std::string, DESIGN_ENTITY> declarations;
    std::vector<std::string> variables;
    // Need to add clauses

    void addDeclaration(std::string, DESIGN_ENTITY);
};

