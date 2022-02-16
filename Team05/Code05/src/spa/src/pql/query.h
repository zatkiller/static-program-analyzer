#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include "PKB/PKBField.h"

namespace qps::query {

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

/**
* Struct used to store information on a StmtRef
*/
struct StmtRef {
private:
    StmtRefType type = StmtRefType::NOT_INITIALIZED;
    std::string declaration = "";
    int lineNo = -1;

public:
    /*
     * Returns a StmtRef of type Declaration
     *
     * @param name the name of the declaration
     * @return StmtRef of type declaration and declaration name
     */
    static StmtRef ofDeclaration(std::string name);

    /*
     * Returns a StmtRef of type LineNo
     *
     * @param lineNo the line number of the StmtRef
     * @return StmtRef of type lineNo and specified LineNo
     */
    static StmtRef ofLineNo(int lineNo);

    /*
     * Returns a StmtRef of type Wildcard
     *
     * @return StmtRef of type wildcard
     */
    static StmtRef ofWildcard();

    StmtRefType getType();

    std::string getDeclaration();

    int getLineNo();

    bool isDeclaration();

    bool isLineNo();

    bool isWildcard();

    /**
     * Wraps a stmtRef into a PKBField.
     *
     * @param stmtRef
     * @return a PKBField of the given stmtRef
     */
    PKBField wrapStmtRef();

    bool operator==(const StmtRef &o) const {
        if (type == StmtRefType::DECLARATION && o.type == StmtRefType::DECLARATION)
            return declaration == o.declaration;
        else if (type == StmtRefType::LINE_NO && o.type == StmtRefType::LINE_NO)
            return lineNo == o.lineNo;

        return type == StmtRefType::WILDCARD && o.type == StmtRefType::WILDCARD;
    }
};

enum class EntRefType {
    NOT_INITIALIZED,
    DECLARATION,
    VARIABLE_NAME,
    WILDCARD
};

/**
* Struct used to store information on a EntRef
*/
struct EntRef {
private:
    EntRefType type = EntRefType::NOT_INITIALIZED;
    std::string declaration = "";
    std::string variable = "";

public:
    /*
     * Returns a EntRef of type Declaration
     *
     * @param name the name of the declaration
     * @return EntRef of type declaration and declaration name
     */
    static EntRef ofDeclaration(std::string name);

    /*
     * Returns a EntRef of type variable name
     *
     * @param name the line number of the StmtRef
     * @return EntRef of type variable and variable name
     */
    static EntRef ofVarName(std::string name);

    /*
     * Returns a EntRef of type Wildcard
     *
     * @return EntRef of type wildcard
     */
    static EntRef ofWildcard();

    EntRefType getType();

    std::string getDeclaration();

    std::string getVariableName();

    bool isDeclaration();

    bool isVarName();

    bool isWildcard();

    /**
     * Wraps a entRef into a PKBField.
     *
     * @param entRef
     * @return a PKBField of the given entRef
     */
    PKBField wrapEntRef();

    bool operator==(const EntRef &o) const {
        if (type == EntRefType::DECLARATION && o.type == EntRefType::DECLARATION)
            return declaration == o.declaration;
        else if (type == EntRefType::VARIABLE_NAME && o.type == EntRefType::VARIABLE_NAME)
            return variable == o.variable;

        return type == EntRefType::WILDCARD && o.type == EntRefType::WILDCARD;
    }
};

//  Following grammar rules naming convention: https://github.com/nus-cs3203/project-wiki/wiki/Iteration-1-Scope
enum class RelRefType {
    INVALID,
    FOLLOWS,
    FOLLOWST,
    PARENT,
    PARENTT,
    MODIFIESS,
    USESS
};

/**
* Abstract class used to represent a RelRef
*/
struct RelRef {
    RelRefType type = RelRefType::INVALID;

    RelRef() {}

    explicit RelRef(RelRefType type) : type(type) {}

    virtual ~RelRef() {}

    virtual RelRefType getType() { return type; }

    virtual std::vector<PKBField> getField() { return std::vector<PKBField>(); }

    virtual std::vector<std::string> getSyns() { return std::vector<std::string>(); }
};

/**
* Struct used to represent a Modifies RelRef
*/
struct Modifies : RelRef {
    Modifies() : RelRef(RelRefType::MODIFIESS) {}

    EntRef modified;
    StmtRef modifiesStmt;

    std::vector<PKBField> getField() override {
        PKBField mStmtField = modifiesStmt.wrapStmtRef();
        PKBField modifiedField = modified.wrapEntRef();
        return std::vector<PKBField>{mStmtField, modifiedField};
    }

    std::vector<std::string> getSyns() override {
        std::vector<std::string> synonyms;
        if (modifiesStmt.isDeclaration()) {
            synonyms.push_back(modifiesStmt.getDeclaration());
        }
        if (modified.isDeclaration()) {
            synonyms.push_back(modified.getDeclaration());
        }
        return synonyms;
    }
};

/**
* Struct used to represent a Uses RelRef
*/
struct Uses : RelRef {
    Uses() : RelRef(RelRefType::USESS) {}

    EntRef used;
    StmtRef useStmt;

    std::vector<PKBField> getField() override {
        PKBField uStmtField = useStmt.wrapStmtRef();
        PKBField usedField = used.wrapEntRef();
        return std::vector<PKBField>{uStmtField, usedField};
    }

    std::vector<std::string> getSyns() override {
        std::vector<std::string> synonyms;
        if (useStmt.isDeclaration()) {
            synonyms.push_back(useStmt.getDeclaration());
        }
        if (used.isDeclaration()) {
            synonyms.push_back(used.getDeclaration());
        }
        return synonyms;
    }
};

struct Follows : RelRef {
    Follows() : RelRef(RelRefType::FOLLOWS) {}

    StmtRef follower;
    StmtRef followed;

    std::vector<PKBField> getField() override {
        PKBField followerField = follower.wrapStmtRef();
        PKBField followedField = followed.wrapStmtRef();
        return std::vector<PKBField>{followerField, followedField};
    }

    std::vector<std::string> getSyns() override {
        std::vector<std::string> synonyms;
        if (follower.isDeclaration()) {
            synonyms.push_back(follower.getDeclaration());
        }
        if (followed.isDeclaration()) {
            synonyms.push_back(followed.getDeclaration());
        }
        return synonyms;
    }
};

struct FollowsT : RelRef {
    FollowsT() : RelRef(RelRefType::FOLLOWST) {}

    StmtRef follower;
    StmtRef transitiveFollowed;

    std::vector<PKBField> getField() override {
        PKBField followerField = follower.wrapStmtRef();
        PKBField tFollowedField = transitiveFollowed.wrapStmtRef();
        return std::vector<PKBField>{followerField, tFollowedField};
    }

    std::vector<std::string> getSyns() override {
        std::vector<std::string> synonyms;
        if (follower.isDeclaration()) {
            synonyms.push_back(follower.getDeclaration());
        }
        if (transitiveFollowed.isDeclaration()) {
            synonyms.push_back(transitiveFollowed.getDeclaration());
        }
        return synonyms;
    }
};

struct Parent : RelRef {
    Parent() : RelRef(RelRefType::PARENT) {}

    StmtRef parent;
    StmtRef child;

    std::vector<PKBField> getField() override {
        PKBField parentField = parent.wrapStmtRef();
        PKBField childField = child.wrapStmtRef();
        return std::vector<PKBField>{parentField, childField};
    }

    std::vector<std::string> getSyns() override {
        std::vector<std::string> synonyms;
        if (parent.isDeclaration()) {
            synonyms.push_back(parent.getDeclaration());
        }
        if (child.isDeclaration()) {
            synonyms.push_back(child.getDeclaration());
        }
        return synonyms;
    }
};

struct ParentT : RelRef {
    ParentT() : RelRef(RelRefType::PARENTT) {}

    StmtRef parent;
    StmtRef transitiveChild;

    std::vector<PKBField> getField() override {
        PKBField parentField = parent.wrapStmtRef();
        PKBField tChildField = transitiveChild.wrapStmtRef();
        return std::vector<PKBField>{parentField, tChildField};
    }

    std::vector<std::string> getSyns() override {
        std::vector<std::string> synonyms;
        if (parent.isDeclaration()) {
            synonyms.push_back(parent.getDeclaration());
        }
        if (transitiveChild.isDeclaration()) {
            synonyms.push_back(transitiveChild.getDeclaration());
        }
        return synonyms;
    }
};

/**
* Struct used to represent a Pattern
*/
struct Pattern {
    std::string synonym;
    EntRef lhs;
    std::string expression;

    std::string getSynonym();

    EntRef getEntRef();

    std::string getExpression();

    bool operator==(const Pattern &o) const {
        return (synonym == o.synonym) && (lhs == o.lhs) && (expression == o.expression);
    }
};

/**
* Struct used to represent a query that has been parsed
*/
class Query {
private:
    std::unordered_map<std::string, DesignEntity> declarations;
    std::vector<std::string> variable;
    std::vector<std::shared_ptr<RelRef>> suchthat;
    std::vector<Pattern> pattern;
    bool valid;

public:
    std::unordered_map<std::string, DesignEntity> getDeclarations();

    std::vector<std::string> getVariable();

    std::vector<std::shared_ptr<RelRef>> getSuchthat();

    std::vector<Pattern> getPattern();

    bool isValid();

    void setValid(bool);

    bool hasDeclaration(std::string);

    bool hasVariable(std::string var);

    void addDeclaration(std::string, DesignEntity);

    void addVariable(std::string var);

    void addSuchthat(std::shared_ptr<RelRef>);

    void addPattern(Pattern);

    /*
     * Returns the DesignEntity of the specified declaration
     *
     * @param declaration the name of the declaration
     * @return DesignEntity of the specified declaration
     */
     DesignEntity getDeclarationDesignEntity(std::string declaration);

};

}  // namespace qps::query
