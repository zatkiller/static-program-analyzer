#pragma once

#include "PKB/PKBField.h"
#include "exceptions.h"
#include "logging.h"
#include "utils.h"

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace qps::query {
using utils::hash_combine;

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
* Struct used to store information on a Declaration
*/
struct Declaration {
    std::string synonym;
    DesignEntity type{};

    Declaration() {}
    Declaration(std::string synonym, DesignEntity type) : synonym(std::move(synonym)), type(type) {}

    std::string getSynonym() const { return synonym; }
    DesignEntity getType() const { return type; }

    bool operator==(const Declaration& o) const {
        return synonym == o.synonym && type == o.type;
    }
};

enum class AttrName {
    INVALID,
    PROCNAME,
    VARNAME,
    VALUE,
    STMTNUM
};

/**
* Struct used to store information on an AttrRef
*/
struct AttrRef {
    AttrRef() {}
    AttrRef(AttrName an, Declaration d) : attrName(an), declaration(std::move(d)) {}

    bool isString() const { return attrName == AttrName::VARNAME || attrName == AttrName::PROCNAME; }
    bool isNumber() const { return attrName == AttrName::VALUE || attrName == AttrName::STMTNUM; }
    bool canBeCompared(const AttrRef &o) const;

    bool operator==(const AttrRef& o) const { return (attrName == o.attrName) && (declaration == o.declaration); }

    AttrName getAttrName() const { return attrName; }
    std::string getDeclarationSynonym() const { return declaration.getSynonym(); }
    DesignEntity getDeclarationType() const { return declaration.getType(); }
    Declaration getDeclaration() const { return declaration; }

private:
    Declaration declaration{};
    AttrName attrName = AttrName::INVALID;
};

enum class ElemType {
    NOT_INITIALIZED,
    ATTR_REF,
    DECLARATION
};

/**
* Struct used to store information on an Elem
*/
struct Elem {
    static Elem ofDeclaration(Declaration d);
    static Elem ofAttrRef(AttrRef ar);

    bool isDeclaration() const { return type == ElemType::DECLARATION; }
    bool isAttrRef() const { return type == ElemType::ATTR_REF; }

    Declaration getDeclaration() const { return declaration; }
    AttrRef getAttrRef() const { return ar; }
    std::string getSyn() const;

    bool operator==(const Elem& o) const {
        if ((type == ElemType::ATTR_REF) && (o.type == ElemType::ATTR_REF)) {
            return ar == o.ar;
        } else if ((type == ElemType::DECLARATION) && (o.type == ElemType::DECLARATION)) {
            return declaration == o.declaration;
        }

        return (type == ElemType::NOT_INITIALIZED) && (o.type == ElemType::NOT_INITIALIZED);
    }

private:
    AttrRef ar;
    Declaration declaration{};
    ElemType type = ElemType::NOT_INITIALIZED;
};

/**
* Struct used to store information on the ResultCl
*/
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
    Declaration declaration{};
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

    bool operator==(const StmtRef& o) const {
        if (type == StmtRefType::DECLARATION && o.type == StmtRefType::DECLARATION) {
            return declaration == o.declaration;
        } else if (type == StmtRefType::LINE_NO && o.type == StmtRefType::LINE_NO) {
            return lineNo == o.lineNo;
        }

        return type == StmtRefType::WILDCARD && o.type == StmtRefType::WILDCARD;
    }
};

enum class EntRefType { NOT_INITIALIZED, DECLARATION, VARIABLE_NAME, WILDCARD };

/**
* Struct used to store information on a EntRef
*/
struct EntRef {
private:
    Declaration declaration{};
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

    EntRefType getType() const;

    std::string getDeclarationSynonym() const;
    DesignEntity getDeclarationType() const;
    Declaration getDeclaration() const;

    std::string getVariableName() const;

    bool isDeclaration() const;
    bool isVarName() const;
    bool isWildcard() const;

    bool operator==(const EntRef& o) const {
        if (type == EntRefType::DECLARATION && o.type == EntRefType::DECLARATION)
            return declaration == o.declaration;
        else if (type == EntRefType::VARIABLE_NAME &&
            o.type == EntRefType::VARIABLE_NAME)
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

//  Following grammar rules naming convention:
//  https://github.com/nus-cs3203/project-wiki/wiki/Iteration-1-Scope
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
    virtual size_t getHash() const = 0;

    virtual RelRefType getType() const { return type; }

    /**
    * Applies Semantic checks on first argument of RelRef
    */
    virtual void validateFirstArg() {}

    /**
     * Applies Semantic checks on second argument of RelRef
     */
    virtual void validateSecondArg() {}

    virtual bool equalTo(const RelRef& r) const = 0;

    bool operator==(const RelRef& r) const { return equalTo(r); }

    /**
      * Pure virtual function that returns the RelRef member attributes
      * as PKBFields
      * 
      * @returns a vector of PKBFields
      */
    virtual std::vector<PKBField> getField() = 0;

    /**
      * Pure virtual function that returns the RelRef member attributes
      * as Declarations
      *
      * @returns a vector of Declarations
      */
    virtual std::vector<Declaration> getDecs() = 0;

protected:
    /**
    * Returns a vector of PKBFields after transforming the RelRef subclass 
    * members to the respective PKBFields
    * @param *f1 memory address of class member
    * @param *f2 memory address of class member
    *
    * @return a vector of PKBField
    */
    template <typename T, typename F1, typename F2>
    std::vector<PKBField> getFieldHelper(const F1 T::*f1, const F2 T::*f2) {
        const auto derivedPtr = static_cast<T*>(this);
        PKBField field1 = [&]() {
            if constexpr (std::is_same_v<F1, StmtRef>) {
                return PKBFieldTransformer::transformStmtRef(derivedPtr->*f1);
            } else {
                return PKBFieldTransformer::transformEntRefVar(derivedPtr->*f1);
            }
        }();
        PKBField field2 = [&]() {
            if constexpr (std::is_same_v<F2, StmtRef>) {
                return PKBFieldTransformer::transformStmtRef(derivedPtr->*f2);
            } else {
                return PKBFieldTransformer::transformEntRefVar(derivedPtr->*f2);
            }
        }();

        return std::vector<PKBField>{field1, field2};
    }

    /**
    * Returns a vector of Declaration from the RelRef subclass 
    * member attributes
    * 
    * @param *f1 memory address of class member
    * @param *f2 memory address of class member
    *
    * @return a vector of Declaration
    */
    template <typename T, typename F1, typename F2>
    std::vector<Declaration> getDecsHelper(const F1 T::*f1, const F2 T::*f2) {
        std::vector<Declaration> synonyms;
        const auto derivedPtr = static_cast<T*>(this);
        if ((derivedPtr->*f1).isDeclaration()) {
            synonyms.push_back((derivedPtr->*f1).getDeclaration());
        }

        if ((derivedPtr->*f2).isDeclaration()) {
            synonyms.push_back((derivedPtr->*f2).getDeclaration());
        }

        return synonyms;
    }

    /**
    * Returns True if a RelRef subclass is = to one aonother
    
    * @param *f1 memory address of class member
    * @param *f2 memory address of class member
    *
    * @return boolean result whethere a subclass is equal to one another
    */
    template <typename T, typename F1, typename F2>
    bool equalityCheckHelper(const F1 T::*f1, const F2 T::*f2, const RelRef* r) const {
        const auto derivedPtr1 = static_cast<const T*>(this);
        const auto derivedPtr2 = static_cast<const T*>(r);
        return (derivedPtr1->*f1 == derivedPtr2->*f1) &&
            (derivedPtr1->*f2 == derivedPtr2->*f2);
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
    size_t getHash() const override;

    void validateFirstArg() override;
    void validateSecondArg() override;
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
    size_t getHash() const override;

    void validateFirstArg() override;
    void validateSecondArg() override;
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
    size_t getHash() const override;

    void validateFirstArg() override;
    void validateSecondArg() override;
};

/**
* Struct used to represent a UsesP RelRef
*/
struct UsesP : RelRef {
    UsesP() : RelRef(RelRefType::USESP) {}

    EntRef used;
    EntRef useProc;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    size_t getHash() const override;

    void validateFirstArg() override;
    void validateSecondArg() override;
};

/**
* Struct used to represent a Follows RelRef
*/
struct Follows : RelRef {
    Follows() : RelRef(RelRefType::FOLLOWS) {}

    StmtRef follower;
    StmtRef followed;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;
    bool equalTo(const RelRef& r) const override;

    size_t getHash() const override;
};

/**
* Struct used to represent a Follows* RelRef
*/
struct FollowsT : RelRef {
    FollowsT() : RelRef(RelRefType::FOLLOWST) {}

    StmtRef follower;
    StmtRef transitiveFollowed;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    size_t getHash() const override;
};

/**
* Struct used to represent a Parent RelRef
*/
struct Parent : RelRef {
    Parent() : RelRef(RelRefType::PARENT) {}

    StmtRef parent;
    StmtRef child;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    size_t getHash() const override;
};

/**
* Struct used to represent a Parent* RelRef
*/
struct ParentT : RelRef {
    ParentT() : RelRef(RelRefType::PARENTT) {}

    StmtRef parent;
    StmtRef transitiveChild;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    size_t getHash() const override;
};

/**
* Struct used to represent a Calls RelRef
*/
struct Calls : RelRef {
    Calls() : RelRef(RelRefType::CALLS) {}

    EntRef caller;
    EntRef callee;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    void validateFirstArg() override;
    void validateSecondArg() override;

    size_t getHash() const override;
};

/**
* Struct used to represent a Calls* RelRef
*/
struct CallsT : RelRef {
    CallsT() : RelRef(RelRefType::CALLST) {}

    EntRef caller;
    EntRef transitiveCallee;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    void validateFirstArg() override;
    void validateSecondArg() override;

    size_t getHash() const override;
};

/**
* Struct used to represent a Next RelRef
*/
struct Next : RelRef {
    Next() : RelRef(RelRefType::NEXT) {}

    StmtRef before;
    StmtRef after;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    size_t getHash() const override;
};

/**
* Struct used to represent a Next* RelRef
*/
struct NextT : RelRef {
    NextT() : RelRef(RelRefType::NEXTT) {}

    StmtRef before;
    StmtRef transitiveAfter;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    size_t getHash() const override;
};

/**
* Struct used to represent a Affects RelRef
*/
struct Affects : RelRef {
    Affects() : RelRef(RelRefType::AFFECTS) {}

    StmtRef affectingStmt;
    StmtRef affected;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    void validateFirstArg() override;
    void validateSecondArg() override;

    size_t getHash() const override;
};

/**
* Struct used to represent a Affects* RelRef
*/
struct AffectsT : RelRef {
    AffectsT() : RelRef(RelRefType::AFFECTST) {}

    StmtRef affectingStmt;
    StmtRef transitiveAffected;

    std::vector<PKBField> getField() override;
    std::vector<Declaration> getDecs() override;

    bool equalTo(const RelRef& r) const override;

    void validateFirstArg() override;
    void validateSecondArg() override;

    size_t getHash() const override;
};

/**
* Struct used to represent a Expression-Spec
*/
struct ExpSpec {
    ExpSpec() {}

    /**
    * Returns a ExpSpec of type wildcard
    *
    * @return ExpSpec of type wildcard
    */
    static ExpSpec ofWildcard();
    /**
    * Returns a ExpSpec of type partial match
    * @param str the pattern used in the partial match
    * @return ExpSpec of type partial match
    */
    static ExpSpec ofPartialMatch(std::string str);
    /**
    * Returns a ExpSpec of type full match
    * @param str the pattern used in the full match
    * @return ExpSpec of type full match
    */
    static ExpSpec ofFullMatch(std::string str);

    bool isPartialMatch() const;
    bool isFullMatch() const;
    bool isWildcard() const;

    std::string getPattern() const;

    bool operator==(const ExpSpec& o) const {
        return (wildCard == o.wildCard) && (partialMatch == o.partialMatch) && (pattern == o.pattern);
    }

private:
    ExpSpec(bool wildCard, bool partialMatch, std::string pattern)
        : wildCard(wildCard), partialMatch(partialMatch), pattern(std::move(pattern)) {}

    std::string pattern;
    bool wildCard = false;
    bool partialMatch = false;
};

/**
* Struct used to represent a Pattern
*/
struct Pattern {
    /**
    * Returns a Pattern which is of type Assign
    *
    * @param synonym the synonym of the declaration for the assign pattern
    * @param er the lhs of the assign pattern
    * @param exp the ExpSpec of the assign pattern
    *
    * @return Pattern which has declaartion type of assign
    */
    static Pattern ofAssignPattern(std::string synonym, EntRef er, ExpSpec exp);
    /**
    * Returns a Pattern which is of type If
    *
    * @param synonym the synonym of the declaration for the If pattern
    * @param er the lhs of the If pattern
    *
    * @return Pattern which has declaration type of If
    */
    static Pattern ofIfPattern(std::string synonym, EntRef er);
    /**
    * Returns a Pattern which is of type While
    *
    * @param synonym the synonym of the declaration for the while pattern
    * @param er the lhs of the while pattern
    * @param exp the ExpSpec of the while pattern
    *
    * @return Pattern which has declaration type of while
    */
    static Pattern ofWhilePattern(std::string synonym, EntRef er);

    std::string getSynonym() const { return declaration.getSynonym(); }
    DesignEntity getSynonymType() const { return declaration.getType(); }
    Declaration getDeclaration() const { return declaration; }

    EntRef getEntRef() const { return lhs; }
    ExpSpec getExpression() const;

    bool operator==(const Pattern& o) const {
        return (declaration == o.declaration) 
            && (lhs == o.lhs)
            && (expression == o.expression);
    }

private:
    EntRef lhs{};
    Declaration declaration{};
    ExpSpec expression{};
};

enum class AttrCompareRefType { NOT_INITIALIZED, NUMBER, STRING, ATTRREF };

/**
* Struct used to represent an AttrCompareRef (ref of a with clause)
*/
struct AttrCompareRef {
private:
    AttrRef ar {};
    std::string str_value;
    AttrCompareRefType type = AttrCompareRefType::NOT_INITIALIZED;
    int number = -1;

public:
    /**
    * Returns a AttrCompareRef of type string
    *
    * @param str the string represented by AttrCompareRef
    * @return AttrCompareRef of type string
    */
    static AttrCompareRef ofString(std::string str);
    /**
    * Returns a AttrCompareRef of type number
    *
    * @param num the number represented by AttrCompareRef
    * @return AttrCompareRef of type number
    */
    static AttrCompareRef ofNumber(int num);
    /**
    * Returns a AttrCompareRef of type AttrRef
    *
    * @param ar the attrRrf represented by AttrComareRef
    * @return AttrCompareRef of type attrref
    */
    static AttrCompareRef ofAttrRef(AttrRef ar);

    std::string getString() const { return str_value; }
    int getNumber() const { return number; }
    AttrRef getAttrRef() const { return ar; }

    bool isString() const { return type == AttrCompareRefType::STRING; }
    bool isNumber() const { return type == AttrCompareRefType::NUMBER; }
    bool isAttrRef() const { return type == AttrCompareRefType::ATTRREF; }

    bool operator==(const AttrCompareRef& o) const {
        if (isString() && o.isString()) {
            return str_value == o.getString();
        } else if (isNumber() && o.isNumber()) {
            return number == o.getNumber();
        } else if (isAttrRef() && o.isAttrRef()) {
            return ar == o.getAttrRef();
        }

        return false;
    }
};

/**
* Struct used to represent an AttrCompare (with clause)
*/
struct AttrCompare {
    AttrCompareRef lhs;
    AttrCompareRef rhs;

    AttrCompare() {}
    AttrCompare(AttrCompareRef lhs, AttrCompareRef rhs) : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    AttrCompareRef getLhs() const { return lhs; }
    AttrCompareRef getRhs() const { return rhs; }

    bool operator==(const AttrCompare &o) const {
        return (lhs == o.lhs) && (rhs == o.rhs);
    }
};

/**
* Struct used to represent a PQL query that has been parsed
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
    bool hasSelectElem(const Elem& e) const;

    void addDeclaration(const std::string&, DesignEntity);
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
    DesignEntity getDeclarationDesignEntity(const std::string& declaration) const;
};
}  // namespace qps::query

// Hash declarations
namespace std {
using qps::query::AttrCompare;
using qps::query::AttrCompareRef;
using qps::query::AttrRef;
using qps::query::Declaration;
using qps::query::DesignEntity;
using qps::query::EntRef;
using qps::query::ExpSpec;
using qps::query::ModifiesS;
using qps::query::Pattern;
using qps::query::RelRef;
using qps::query::StmtRef;

using utils::hash_combine;

template <> struct hash<Declaration> {
    size_t operator()(const Declaration& d) const {
        size_t seed = 0;
        hash_combine(seed, d.getSynonym());
        hash_combine(seed, d.getType());
        return seed;
    }
};

template <> struct hash<AttrRef> {
    size_t operator()(const AttrRef& a) const {
        size_t seed = 0;
        hash_combine(seed, a.getAttrName());
        hash_combine(seed, a.getDeclaration());
        return seed;
    }
};

template <> struct hash<StmtRef> {
    size_t operator()(const StmtRef& s) const {
        size_t seed = 0;
        hash_combine(seed, s.getType());

        if (s.isDeclaration()) {
            hash_combine(seed, s.getDeclaration());
        } else if (s.isLineNo()) {
            hash_combine(seed, s.getLineNo());
        }

        return seed;
    }
};

template <> struct hash<EntRef> {
    size_t operator()(const EntRef& e) const {
        size_t seed = 0;
        hash_combine(seed, e.getType());

        if (e.isDeclaration()) {
            hash_combine(seed, e.getDeclaration());
        } else if (e.isVarName()) {
            hash_combine(seed, e.getVariableName());
        }

        return seed;
    }
};

template <> struct hash<RelRef*> {
    size_t operator()(RelRef* r) const { return r->getHash(); }
};

template <> struct equal_to<std::shared_ptr<RelRef>> {
    bool operator()(std::shared_ptr<RelRef> lhs, std::shared_ptr<RelRef> rhs) const { return *lhs == *rhs; }
};

template <> struct hash<ExpSpec> {
    size_t operator()(const ExpSpec& e) const {
        size_t seed = 0;
        hash_combine(seed, e.isWildcard());
        hash_combine(seed, e.isPartialMatch());
        hash_combine(seed, e.getPattern());
        return seed;
    }
};

template <> struct hash<Pattern> {
    size_t operator()(const Pattern& p) const {
        size_t seed = 0;
        hash_combine(seed, p.getDeclaration());
        hash_combine(seed, p.getEntRef());

        if (p.getSynonymType() == DesignEntity::ASSIGN) {
            hash_combine(seed, p.getExpression());
        }

        return seed;
    }
};

template <> struct hash<AttrCompareRef> {
    size_t operator()(const AttrCompareRef& a) const {
        size_t seed = 0;
        if (a.isString()) {
            hash_combine(seed, a.getString());
        } else if (a.isNumber()) {
            hash_combine(seed, a.getNumber());
        } else if (a.isAttrRef()) {
            hash_combine(seed, a.getAttrRef());
        }
        return seed;
    }
};

template <> struct hash<AttrCompare> {
    size_t operator()(const AttrCompare& a) const {
        size_t seed = 0;
        hash_combine(seed, a.getLhs());
        hash_combine(seed, a.getRhs());
        return seed;
    }
};

}  // namespace std
