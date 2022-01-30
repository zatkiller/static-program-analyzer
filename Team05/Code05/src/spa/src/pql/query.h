
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

enum class StmtRefType {
    NOT_INITIALIZED,
    DECLARATION,
    LINE_NO,
    WILDCARD
};

struct StmtRef {
    StmtRefType type = StmtRefType::NOT_INITIALIZED;
    std::string declaration;
    int lineNo;

    void setType(StmtRefType);
    void setDeclaration(std::string);
    void setLineNo(int);

    void getType();
    void getDeclaration();
    void getLineNo();

    bool isDeclaration();
    bool isLineNo();
    bool isWildcard();
};

enum class EntRefType {
    NOT_INITIALIZED,
    DECLARATION,
    VARIABLE_NAME,
    WILDCARD
};

struct EntRef{
    EntRefType type = EntRefType::NOT_INITIALIZED;
    std::string declaration;
    std::string variable;

    void setType(EntRefType);
    void setDeclaration(std::string);
    void setVariableName(std::string);

    void getType();
    void getDeclaration();
    void getVariableName();

    bool isDeclaration();
    bool isVarName();
    bool isWildcard();
};

enum class RelRefType {
    INVALID,
    FOLLOWS,
    FOLLOWST,
    PARENT,
    PARRENTT,
    MODIFIESS,
    USESS
};

// Abstract class
struct RelRef {
    RelRefType type = RelRefType::INVALID;
    virtual ~RelRef() {};
};

struct Modifies : RelRef {
    RelRefType type = RelRefType::MODIFIESS;

    EntRef modified;
    StmtRef modifiesStmt;
};

struct Uses : RelRef {
    RelRefType type = RelRefType::USESS;

    EntRef used;
    StmtRef useStmt;
};

struct Pattern {
    std::string synonym;
    std::pair<EntRef, std::string> params;
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
