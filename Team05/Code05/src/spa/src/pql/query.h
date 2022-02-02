#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

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

extern std::unordered_map<std::string, DesignEntity> designEntityMap;

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

    StmtRefType getType();
    std::string getDeclaration();
    int getLineNo();

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

    EntRefType getType();
    std::string getDeclaration();
    std::string getVariableName();

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
    RelRef() {}
    RelRef(RelRefType type) : type(type) {}
    virtual ~RelRef() {};
    virtual RelRefType getType() {return type;}
};

struct Modifies : RelRef {

    Modifies() : RelRef(RelRefType::MODIFIESS) {}
    EntRef modified;
    StmtRef modifiesStmt;
};

struct Uses : RelRef {
    Uses() : RelRef(RelRefType::USESS) {}

    EntRef used;
    StmtRef useStmt;
    RelRefType getType() {return type;}
};

struct Pattern {
    std::string synonym;
    EntRef lhs;
    std::string expression;

    void setSynonym(std::string);
    void setLhs(EntRef);
    void setExpression(std::string);

    std::string getSynonym();
    EntRef getEntRef();
    std::string getExpression();

};

class Query {
private:
    std::unordered_map<std::string, DesignEntity> declarations;
    std::vector<std::string> variable;
    std::vector<std::shared_ptr<RelRef>> suchthat;
    std::vector<Pattern> pattern;
public:
    std::unordered_map<std::string, DesignEntity> getDeclarations();
    std::vector<std::string> getVariable();
    std::vector<std::shared_ptr<RelRef>> getSuchthat();
    std::vector<Pattern> getPattern();

    bool hasDeclaration(std::string name);
    bool hasVariable(std::string var);

    void addDeclaration(std::string, DesignEntity);
    void addVariable(std::string var);
    void addSuchthat(std::shared_ptr<RelRef>);
    void addPattern(Pattern pattern);

    DesignEntity getDeclarationDesignEntity(std::string name);
};
