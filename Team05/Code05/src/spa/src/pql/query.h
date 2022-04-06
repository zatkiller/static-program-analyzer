#pragma once

#include <unordered_map>
#include <algorithm>
#include <utility>
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

struct Declaration {
    std::string synonym;
    DesignEntity type {};

    Declaration() {}
    Declaration(std::string synonym, DesignEntity type) : synonym(std::move(synonym)), type(type) {}

    std::string getSynonym() const { return synonym; }
    DesignEntity getType() const { return type; }

    bool operator==(const Declaration &o) const { return synonym == o.synonym && type == o.type; }
};

enum class AttrName {
    INVALID,
    PROCNAME,
    VARNAME,
    VALUE,
    STMTNUM
};

struct AttrRef {
    AttrRef() {}
    AttrRef(AttrName an, Declaration d) : attrName(an), declaration(std::move(d)) {}

    bool isString() const { return attrName == AttrName::VARNAME || attrName == AttrName::PROCNAME; }
    bool isNumber() const { return attrName == AttrName::VALUE || attrName == AttrName::STMTNUM; }
    bool compatbileComparison(const AttrRef &o) const;

    bool operator==(const AttrRef &o) const { return (attrName == o.attrName) && (declaration == o.declaration); }

    AttrName getAttrName() const { return attrName; }
    std::string getDeclarationSynonym() const { return declaration.getSynonym(); }
    DesignEntity getDeclarationType() const { return declaration.getType(); }
    Declaration getDeclaration() const { return declaration; }

private:
    Declaration declaration {};
    AttrName attrName = AttrName::INVALID;
};

enum class ElemType {
    NOT_INITIALIZED,
    ATTR_REF,
    DECLARATION
};

struct Elem {
    static Elem ofDeclaration(Declaration d);
    static Elem ofAttrRef(AttrRef ar);

    bool isDeclaration() const { return type == ElemType::DECLARATION; }
    bool isAttrRef() const { return type == ElemType::ATTR_REF; }

    Declaration getDeclaration() const { return declaration; }
    AttrRef getAttrRef() const { return ar; }
    std::string getSyn() const;

    bool operator==(const Elem &o) const {
        if ((type == ElemType::ATTR_REF) && (o.type == ElemType::ATTR_REF)) {
            return ar == o.ar;
        } else if ((type == ElemType::DECLARATION) && (o.type == ElemType::DECLARATION)) {
            return declaration == o.declaration;
        }

        return (type == ElemType::NOT_INITIALIZED) && (o.type == ElemType::NOT_INITIALIZED);
    }

private:
    AttrRef ar;
    Declaration declaration {};
    ElemType type = ElemType::NOT_INITIALIZED;
};

struct ResultCl {
    static ResultCl ofBoolean();
    static ResultCl ofTuple(std::vector<Elem> tuple);

    bool isBoolean() const { return boolean; }
    std::vector<Elem> getTuple() const { return tuple; }
    std::vector<std::string> getSynAsList() const;

    bool hasElem(const Elem& e) const;


private:
    std::vector<Elem> tuple;
    bool boolean = false;
};

/**
* Struct used to store information on a StmtRef
*/
struct StmtRef {
private:
    Declaration declaration {};
    int lineNo = -1;
    StmtRefType type = StmtRefType::NOT_INITIALIZED;

public:
    /**
        * Returns a StmtRef of type Declaration
        *
        * @param declaration the declaration
        * @return StmtRef of type declaration and declaration name
        */
    static StmtRef ofDeclaration(Declaration d);

    /**
        * Returns a StmtRef of type LineNo
        *
        * @param lineNo the line number of the StmtRef
        * @return StmtRef of type lineNo and specified LineNo
        */
    static StmtRef ofLineNo(int lineNo);

    /**
        * Returns a StmtRef of type Wildcard
        *
        * @return StmtRef of type wildcard
        */
    static StmtRef ofWildcard();

    StmtRefType getType() const;
    std::string getDeclarationSynonym() const;
    DesignEntity getDeclarationType() const;
    Declaration getDeclaration() const;

    int getLineNo() const;

    bool isDeclaration() const;
    bool isLineNo() const;
    bool isWildcard() const;

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
    Declaration declaration {};
    std::string variable;
    EntRefType type = EntRefType::NOT_INITIALIZED;

public:
    /**
        * Returns a EntRef of type Declaration
        *
        * @param d the declaration
        * @return EntRef of type declaration and declaration name
        */
    static EntRef ofDeclaration(Declaration d);

    /**
    * Returns a EntRef of type variable name
    *
    * @param name the line number of the StmtRef
    * @return EntRef of type variable and variable name
    */
    static EntRef ofVarName(std::string name);

    /**
        * Returns a EntRef of type Wildcard
        *
        * @return EntRef of type wildcard
        */
    static EntRef ofWildcard();

    EntRefType getType();

    std::string getDeclarationSynonym() const;
    DesignEntity getDeclarationType() const;
    Declaration getDeclaration() const;

    std::string getVariableName();

    bool isDeclaration() const;

    bool isVarName();

    bool isWildcard();

    bool operator==(const EntRef &o) const {
        if (type == EntRefType::DECLARATION && o.type == EntRefType::DECLARATION)
            return declaration == o.declaration;
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
    static PKBField transformStmtRef(const StmtRef& s);
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

    virtual RelRefType getType() const { return type; }

    virtual void checkFirstArg() {}
    virtual void checkSecondArg() {}

    virtual std::vector<PKBField> getField() = 0;
    virtual std::vector<Declaration> getDecs() = 0;

    virtual bool equalTo(const RelRef& r) const = 0;

    bool operator==(const RelRef& r) const {
        return equalTo(r);
    }
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
    std::vector<Declaration> getDecsHelper(const F1 T::*f1, const F2 T::*f2) {
        std::vector<Declaration> synonyms;
        const auto derivedPtr = static_cast<T *>(this);
        if ((derivedPtr->*f1).isDeclaration()) {
            synonyms.push_back((derivedPtr->*f1).getDeclaration());
        }

        if ((derivedPtr->*f2).isDeclaration()) {
            synonyms.push_back((derivedPtr->*f2).getDeclaration());
        }

        return synonyms;
    }

    template<typename T, typename F1, typename F2>
    bool equalityCheckHelper(const F1 T::*f1, const F2 T::*f2, const RelRef* r ) const {
        const auto derivedPtr1 = static_cast<const T *>(this);
        const auto derivedPtr2 = static_cast<const T *>(r);
        return (derivedPtr1->*f1 == derivedPtr2->*f1) && (derivedPtr1->*f2 == derivedPtr2->*f2);
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
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;
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
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;
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

    std::vector<Declaration> getDecs() override;
    bool equalTo(const RelRef& r) const override;
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
    std::vector<Declaration> getDecs() override;
    bool equalTo(const RelRef& r) const override;
    void checkFirstArg() override;
    void checkSecondArg() override;
};

struct Follows : RelRef {
    Follows() : RelRef(RelRefType::FOLLOWS) {}

    StmtRef follower;
    StmtRef followed;

    std::vector<PKBField> getField() override;

    std::vector<Declaration> getDecs() override;
    bool equalTo(const RelRef& r) const override;
};

struct FollowsT : RelRef {
    FollowsT() : RelRef(RelRefType::FOLLOWST) {}

    StmtRef follower;
    StmtRef transitiveFollowed;

    std::vector<PKBField> getField() override;

    std::vector<Declaration> getDecs() override;
    bool equalTo(const RelRef& r) const override;
};

struct Parent : RelRef {
    Parent() : RelRef(RelRefType::PARENT) {}

    StmtRef parent;
    StmtRef child;

    std::vector<PKBField> getField() override;

    std::vector<Declaration> getDecs() override;
    bool equalTo(const RelRef& r) const override;
};

struct ParentT : RelRef {
    ParentT() : RelRef(RelRefType::PARENTT) {}

    StmtRef parent;
    StmtRef transitiveChild;

    std::vector<PKBField> getField() override;

    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;
};

struct Calls : RelRef {
    Calls() : RelRef(RelRefType::CALLS) {}

    EntRef caller;
    EntRef callee;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    void checkFirstArg() override;
    void checkSecondArg() override;
};

struct CallsT: RelRef {
    CallsT() : RelRef(RelRefType::CALLST) {}

    EntRef caller;
    EntRef transitiveCallee;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    void checkFirstArg() override;
    void checkSecondArg() override;
};

struct Next : RelRef {
    Next() : RelRef(RelRefType::NEXT) {}

    StmtRef before;
    StmtRef after;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;
    bool equalTo(const RelRef& r) const override;
};

struct NextT : RelRef {
    NextT() : RelRef(RelRefType::NEXTT) {}

    StmtRef before;
    StmtRef transitiveAfter;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;
    bool equalTo(const RelRef& r) const override;
};

struct Affects : RelRef {
    Affects() : RelRef(RelRefType::AFFECTS) {}

    StmtRef affectingStmt;
    StmtRef affected;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    void checkFirstArg() override;
    void checkSecondArg() override;
};

struct AffectsT : RelRef {
    AffectsT() : RelRef(RelRefType::AFFECTST) {}

    StmtRef affectingStmt;
    StmtRef transitiveAffected;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    void checkFirstArg() override;
    void checkSecondArg() override;
};

/**
* Struct used to represent a Expression-Spec
*/
struct ExpSpec {
    ExpSpec() {}

    static ExpSpec ofWildcard();
    static ExpSpec ofPartialMatch(std::string str);
    static ExpSpec ofFullMatch(std::string str);

    bool isPartialMatch() const;
    bool isFullMatch() const;
    bool isWildcard() const;

    std::string getPattern() const;

    bool operator==(const ExpSpec &o) const {
        return (wildCard == o.wildCard) && (partialMatch == o.partialMatch) && (pattern == o.pattern);
    }

private:
    ExpSpec(bool wildCard, bool partialMatch, std::string pattern) : wildCard(wildCard), partialMatch(partialMatch),
                                                                    pattern(std::move(pattern)) {}

    std::string pattern;
    bool wildCard = false;
    bool partialMatch = false;
};

/**
* Struct used to represent a Pattern
*/
struct Pattern {
    static Pattern ofAssignPattern(std::string synonym, EntRef er, ExpSpec exp);
    static Pattern ofIfPattern(std::string synonym, EntRef er);
    static Pattern ofWhilePattern(std::string synonym, EntRef er);

    std::string getSynonym() const { return declaration.getSynonym(); }
    DesignEntity getSynonymType() const { return declaration.getType(); }

    EntRef getEntRef() const { return lhs; }
    ExpSpec getExpression() const;

    bool operator==(const Pattern &o) const {
        return (declaration == o.declaration) && (lhs == o.lhs) &&
            (expression == o.expression);
    }

private:
    EntRef lhs {};
    Declaration declaration {};
    ExpSpec expression {};
};

enum class AttrCompareRefType {
    NOT_INITIALIZED,
    NUMBER,
    STRING,
    ATTRREF
};

struct AttrCompareRef {
private:
    AttrRef ar {};
    std::string str_value;
    AttrCompareRefType type = AttrCompareRefType::NOT_INITIALIZED;
    int number = -1;

public:
    static AttrCompareRef ofString(std::string str);
    static AttrCompareRef ofNumber(int num);
    static AttrCompareRef ofAttrRef(AttrRef ar);

    std::string getString() const { return str_value; }
    int getNumber() const { return number; }
    AttrRef getAttrRef() const { return ar; }

    bool isString() const { return type == AttrCompareRefType::STRING; }
    bool isNumber() const { return type == AttrCompareRefType::NUMBER; }
    bool isAttrRef() const { return type == AttrCompareRefType::ATTRREF; }
    bool validComparison(const AttrCompareRef&  o) const;

    bool operator==(const AttrCompareRef &o) const {
        return (type == o.type) && (str_value == o.str_value) && (ar == o.ar) && (number == o.number);
    }
};


struct AttrCompare {
    AttrCompareRef lhs;
    AttrCompareRef rhs;

    AttrCompare() {}
    AttrCompare(AttrCompareRef lhs, AttrCompareRef rhs) : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    AttrCompareRef getLhs() const { return lhs; }
    AttrCompareRef getRhs() const { return rhs; }
    void validateComparingTypes() const;

    bool operator==(const AttrCompare &o) const {
        return (lhs == o.lhs) && (rhs == o.rhs);
    }
};

/**
* Struct used to represent a query that has been parsed
*/
class Query {
private:
    std::unordered_map<std::string, DesignEntity> declarations;
    ResultCl selectResults;
    std::vector<std::shared_ptr<RelRef>> suchthat;
    std::vector<Pattern> pattern;
    std::vector<AttrCompare> with;
    bool valid;

public:
    std::unordered_map<std::string, DesignEntity> getDeclarations() const;
    ResultCl getResultCl() const;

    std::vector<std::shared_ptr<RelRef>> getSuchthat() const;
    std::vector<Pattern> getPattern() const;
    std::vector<AttrCompare> getWith() const;


    bool isValid() const;
    void setValid(bool);

    bool hasDeclaration(const std::string&) const;
    bool hasSelectElem(const Elem &e) const;

    void addDeclaration(const std::string &, DesignEntity);
    void addResultCl(const ResultCl& resultCl);

    void addSuchthat(const std::shared_ptr<RelRef>&);
    void addPattern(const Pattern&);
    void addWith(const AttrCompare&);

    /**
        * Returns the DesignEntity of the specified declaration
        *
        * @param declaration the name of the declaration
        * @return DesignEntity of the specified declaration
        */
    DesignEntity getDeclarationDesignEntity(const std::string &declaration) const;
};

}  // namespace qps::query
