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
    std::string ayn_assign;
    std::pair<std::string, std::string> params;
};

class QueryObject {
private:
    std::unordered_map<std::string, DesignEntity> declarations;
    std::vector<std::string> select;
    std::vector<RelRef> suchthat;
    std::vector<Pattern> pattern;
public:
    std::unordered_map<std::string, DesignEntity> getDeclarations();
    std::vector<std::string> getSelect();
    std::vector<RelRef> getSuchthat();
    std::vector<Pattern> getPattern();
    void setDeclarations(std::unordered_map<std::string, DesignEntity> declarations);
    void setSelect(std::vector<std::string> select);
    void setSuchthat(std::vector<RelRef> suchthat);
    void setPattern(std::vector<Pattern> pattern);
};
