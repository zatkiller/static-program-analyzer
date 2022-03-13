#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>
#include "PKB/PKBField.h"
#include "logging.h"
#include "exceptions.h"

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

enum class AttrName;

extern std::unordered_map<std::string, DesignEntity> designEntityMap;
extern std::unordered_map<AttrName, std::unordered_set<DesignEntity>> attrNameToDesignEntityMap;

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
    DesignEntity declarationType = DesignEntity::CONSTANT;
    int lineNo = -1;

public:
    /*
        * Returns a StmtRef of type Declaration
        *
        * @param name the name of the declaration
        * @param de the design entity of the declaration
        * @return StmtRef of type declaration and declaration name
        */
    static StmtRef ofDeclaration(std::string name, DesignEntity de);

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

    std::string getDeclaration() const;
    DesignEntity getDeclarationType() const;

    int getLineNo();

    bool isDeclaration() const;

    bool isLineNo();

    bool isWildcard();

    bool operator==(const StmtRef &o) const {
        if (type == StmtRefType::DECLARATION && o.type == StmtRefType::DECLARATION)
            return declaration == o.declaration && declarationType == o.declarationType;
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
    DesignEntity declarationType = DesignEntity::CONSTANT;
    std::string variable = "";

public:
    /*
        * Returns a EntRef of type Declaration
        *
        * @param name the name of the declaration
        * @param de the design entity of the declaration
        * @return EntRef of type declaration and declaration name
        */
    static EntRef ofDeclaration(std::string name, DesignEntity de);

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

    std::string getDeclaration() const;
    DesignEntity getDeclarationType() const;

    std::string getVariableName();

    bool isDeclaration() const;

    bool isVarName();

    bool isWildcard();

    bool operator==(const EntRef &o) const {
        if (type == EntRefType::DECLARATION && o.type == EntRefType::DECLARATION)
            return declaration == o.declaration && declarationType == o.declarationType;
        else if (type == EntRefType::VARIABLE_NAME && o.type == EntRefType::VARIABLE_NAME)
            return variable == o.variable;

        return type == EntRefType::WILDCARD && o.type == EntRefType::WILDCARD;
    }
};

struct PKBFieldTransformer {
    /**
     * Transforms a entRef into a PKBField.
     *
     * @param entRef
     * @return a PKBField of the given entRef
     */
    static PKBField transformEntRefVar(EntRef e);

    static PKBField transformEntRefProc(EntRef e);
    /**
     * Transforms a stmtRef into a PKBField.
     *
     * @param stmtRef
     * @return a PKBField of the given stmtRef
     */
    static PKBField transformStmtRef(StmtRef s);
};

//  Following grammar rules naming convention: https://github.com/nus-cs3203/project-wiki/wiki/Iteration-1-Scope
enum class RelRefType {
    INVALID,
    FOLLOWS,
    FOLLOWST,
    PARENT,
    PARENTT,
    MODIFIESS,
    MODIFIESP,
    USESS,
    USESP,
    CALLS,
    CALLST,
    NEXT,
    NEXTT,
    AFFECTS,
    AFFECTST
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

    virtual void checkFirstArg() {}
    virtual void checkSecondArg() {}

    virtual std::vector<PKBField> getField() = 0;

    virtual std::vector<std::string> getSyns() = 0;

protected:
    template<typename T, typename F1, typename F2>
    std::vector<PKBField> getFieldHelper(const F1 T::*f1, const F2 T::*f2) {
        const auto derivedPtr = static_cast<T *>(this);
        PKBField field1 = [&]() {
            if constexpr(std::is_same_v<F1, StmtRef>) {
                return PKBFieldTransformer::transformStmtRef(derivedPtr->*f1);
            } else {
                return PKBFieldTransformer::transformEntRefVar(derivedPtr->*f1);
            }
        }();
        PKBField field2 = [&]() {
            if constexpr(std::is_same_v<F2, StmtRef>) {
                return PKBFieldTransformer::transformStmtRef(derivedPtr->*f2);
            } else {
                return PKBFieldTransformer::transformEntRefVar(derivedPtr->*f2);
            }
        }();

        return std::vector<PKBField>{field1, field2};
    }

    template<typename T, typename F1, typename F2>
    std::vector<std::string> getSynsHelper(const F1 T::*f1, const F2 T::*f2) {
        std::vector<std::string> synonyms;
        const auto derivedPtr = static_cast<T *>(this);
        if ((derivedPtr->*f1).isDeclaration()) {
            synonyms.push_back((derivedPtr->*f1).getDeclaration());
        }

        if ((derivedPtr->*f2).isDeclaration()) {
            synonyms.push_back((derivedPtr->*f2).getDeclaration());
        }

        return synonyms;
    }
};

/**
* Struct used to represent a ModifiesS RelRef
*/
struct ModifiesS : RelRef {
    ModifiesS() : RelRef(RelRefType::MODIFIESS) {}

    EntRef modified;
    StmtRef modifiesStmt;

    std::vector<PKBField> getField() override;
    std::vector<std::string> getSyns() override;

    void checkFirstArg() override;
    void checkSecondArg() override;
};

/**
* Struct used to represent a ModifiesP RelRef
*/
struct ModifiesP : RelRef {
    ModifiesP() : RelRef(RelRefType::MODIFIESP) {}

    EntRef modified;
    EntRef modifiesProc;

    std::vector<PKBField> getField() override;
    std::vector<std::string> getSyns() override;

    void checkFirstArg() override;
    void checkSecondArg() override;
};

/**
* Struct used to represent a UsesS RelRef
*/
struct UsesS : RelRef {
    UsesS() : RelRef(RelRefType::USESS) {}

    EntRef used;
    StmtRef useStmt;

    std::vector<PKBField> getField() override;

    std::vector<std::string> getSyns() override;

    void checkFirstArg() override;
    void checkSecondArg() override;
};
/**override
* Struct used to represent a UsesP RelRef
*/
struct UsesP : RelRef {
    UsesP() : RelRef(RelRefType::USESP) {}

    EntRef used;
    EntRef useProc;

    std::vector<PKBField> getField() override;
    std::vector<std::string> getSyns() override;

    void checkFirstArg() override;
    void checkSecondArg() override;
};

struct Follows : RelRef {
    Follows() : RelRef(RelRefType::FOLLOWS) {}

    StmtRef follower;
    StmtRef followed;

    std::vector<PKBField> getField() override;

    std::vector<std::string> getSyns() override;
};

struct FollowsT : RelRef {
    FollowsT() : RelRef(RelRefType::FOLLOWST) {}

    StmtRef follower;
    StmtRef transitiveFollowed;

    std::vector<PKBField> getField() override;

    std::vector<std::string> getSyns() override;
};

struct Parent : RelRef {
    Parent() : RelRef(RelRefType::PARENT) {}

    StmtRef parent;
    StmtRef child;

    std::vector<PKBField> getField() override;

    std::vector<std::string> getSyns() override;
};

struct ParentT : RelRef {
    ParentT() : RelRef(RelRefType::PARENTT) {}

    StmtRef parent;
    StmtRef transitiveChild;

    std::vector<PKBField> getField() override;

    std::vector<std::string> getSyns() override;
};

struct Calls : RelRef {
    Calls() : RelRef(RelRefType::CALLS) {}

    EntRef caller;
    EntRef callee;

    std::vector<PKBField> getField() override;
    std::vector<std::string> getSyns() override;

    void checkFirstArg() override;
    void checkSecondArg() override;
};

struct CallsT: RelRef {
    CallsT() : RelRef(RelRefType::CALLST) {}

    EntRef caller;
    EntRef transitiveCallee;

    std::vector<PKBField> getField() override;
    std::vector<std::string> getSyns() override;

    void checkFirstArg() override;
    void checkSecondArg() override;
};

struct Next : RelRef {
    Next() : RelRef(RelRefType::NEXT) {}

    StmtRef before;
    StmtRef after;

    std::vector<PKBField> getField() override;
    std::vector<std::string> getSyns() override;
};

struct NextT : RelRef {
    NextT() : RelRef(RelRefType::NEXTT) {}

    StmtRef before;
    StmtRef transitiveAfter;

    std::vector<PKBField> getField() override;
    std::vector<std::string> getSyns() override;
};

/*
* Struct used to represent a Expression-Spec
*/
struct ExpSpec {
    ExpSpec() {}

    static ExpSpec ofWildcard();

    static ExpSpec ofPartialMatch(std::string str);
    static ExpSpec ofFullMatch(std::string str);

    bool isPartialMatch();
    bool isFullMatch();
    bool isWildcard();

    std::string getPattern();

    bool operator==(const ExpSpec &o) const {
        return (wildCard == o.wildCard) && (partialMatch == o.partialMatch) && (pattern == o.pattern);
    }

private:
    ExpSpec(bool wildCard, bool partialMatch, std::string pattern) : wildCard(wildCard), partialMatch(partialMatch),
                                                                    pattern(pattern) {}

    bool wildCard = false;
    bool partialMatch = false;
    std::string pattern = "";
};

/**
* Struct used to represent a Pattern
*/
struct Pattern {
    std::string synonym;
    EntRef lhs;
    ExpSpec expression;

    std::string getSynonym();

    EntRef getEntRef();

    ExpSpec getExpression();

    bool operator==(const Pattern &o) const {
        return (synonym == o.synonym) && (lhs == o.lhs) && (expression == o.expression);
    }
};

enum class AttrName {
    INVALID,
    PROCNAME,
    VARNAME,
    VALUE,
    STMTNUM
};

struct AttrRef {
    AttrName attrName = AttrName::INVALID;
    DesignEntity declarationType {};
    std::string declaration = "";

    bool isString() const { return attrName == AttrName::VARNAME || attrName == AttrName::PROCNAME; }
    bool isNumber() const { return attrName == AttrName::VALUE || attrName == AttrName::STMTNUM; }
    bool compatbileComparison(AttrRef o) const { return (isString() && o.isString()) || (isNumber() && o.isNumber()); }
};

enum class AttrCompareRefType {
    NOT_INITIALIZED,
    NUMBER,
    STRING,
    ATTRREF
};

struct AttrCompareRef {
private:
    AttrCompareRefType type = AttrCompareRefType::NOT_INITIALIZED;
    int number = -1;
    std::string str_value = "";
    AttrRef ar;

public:
    static AttrCompareRef ofString(std::string str);
    static AttrCompareRef ofNumber(int num);
    static AttrCompareRef ofAttrRef(AttrRef ar);

    std::string getString() { return str_value; }
    int getNumber() { return number; }
    AttrRef getAttrRef() { return ar; }

    bool isString() const { return type == AttrCompareRefType::STRING; }
    bool isNumber() const { return type == AttrCompareRefType::NUMBER; }
    bool isAttrRef() const { return type == AttrCompareRefType::ATTRREF; }
    bool validComparison(AttrCompareRef  o) const;
};


struct AttrCompare {
    AttrCompareRef lhs;
    AttrCompareRef rhs;

    AttrCompare(AttrCompareRef lhs, AttrCompareRef rhs) : lhs(lhs), rhs(rhs) {}

    AttrCompareRef getLhs() { return lhs; }
    AttrCompareRef getRhs() { return rhs; }
    void validateComparingTypes();
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
    std::vector<AttrCompare> with;
    bool valid;

public:
    std::unordered_map<std::string, DesignEntity> getDeclarations();

    std::vector<std::string> getVariable();
    std::vector<std::shared_ptr<RelRef>> getSuchthat();
    std::vector<Pattern> getPattern();
    std::vector<AttrCompare> getWith();


    bool isValid();
    void setValid(bool);

    bool hasDeclaration(std::string);
    bool hasVariable(std::string var);

    void addDeclaration(std::string, DesignEntity);
    void addVariable(std::string var);
    void addSuchthat(std::shared_ptr<RelRef>);
    void addPattern(Pattern);
    void addWith(AttrCompare);

    /*
        * Returns the DesignEntity of the specified declaration
        *
        * @param declaration the name of the declaration
        * @return DesignEntity of the specified declaration
        */
    DesignEntity getDeclarationDesignEntity(std::string declaration);
};

}  // namespace qps::query
