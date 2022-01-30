#ifndef INC_21S2_CP_SPA_TEAM_05_QUERYOBJECT_H
#define INC_21S2_CP_SPA_TEAM_05_QUERYOBJECT_H

#endif //INC_21S2_CP_SPA_TEAM_05_QUERYOBJECT_H

enum class DesignEntity {
    STMT,
    READ,
    PRINT,
    CALL,
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

enum class RelRefType {
    FOLLOWS,
    FOLLOWST,
    PARENT,
    PARRENTT,
    MODIFIESS,
    USESS
};

struct RelRef {
    RelRefType type;
    std::pair<std::string, std::string> params;
};

struct Pattern {
    std::string syn_assign;
    std::pair<std::string, std::string> params;
};

class Query {
private:
    std::unordered_map<std::string, DesignEntity> declarations;
    std::vector<std::string> variable;
    std::vector<RelRef> suchthat;
    std::vector<Pattern> pattern;
public:
    std::unordered_map<std::string, DesignEntity> getDeclarations();
    std::vector<std::string> getVariable();
    std::vector<RelRef> getSuchthat();
    std::vector<Pattern> getPattern();
    void setDeclarations(std::unordered_map<std::string, DesignEntity> declarations);
    void setVariable(std::vector<std::string> select);
    void setSuchthat(std::vector<RelRef> suchthat);
    void setPattern(std::vector<Pattern> pattern);
};
