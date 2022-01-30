
#include <unordered_map>
#include <vector>
#include <string>

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

static std::unordered_map<std::string, DesignEntity> designEntityMap = {
        { "stmt", DesignEntity::STMT },
        { "read", DesignEntity::READ } ,
        { "print", DesignEntity::PRINT },
        { "while", DesignEntity::WHILE },
        { "if", DesignEntity::IF },
        { "assign", DesignEntity::ASSIGN },
        { "variable", DesignEntity::VARIABLE },
        { "constant", DesignEntity::CONSTANT },
        { "procedure", DesignEntity::PROCEDURE }
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

    void addDeclaration(std::string, DesignEntity);
    bool hasDeclaration(std::string name);
    DesignEntity getDeclarationDesignEntity(std::string name);

    void setDeclarations(std::unordered_map<std::string, DesignEntity> declarations);
    void setVariable(std::vector<std::string> select);
    void setSuchthat(std::vector<RelRef> suchthat);
    void setPattern(std::vector<Pattern> pattern);
};
