#include <algorithm>
#include <unordered_set>
#include <utility>

#include "exceptions.h"
#include "pql/lexer.h"
#include "pql/query.h"
#include "pkbtypematcher.h"

namespace qps::query {
std::unordered_map<std::string, DesignEntity> designEntityMap = {
        {"stmt", DesignEntity::STMT},
        {"read", DesignEntity::READ},
        {"print", DesignEntity::PRINT},
        {"while", DesignEntity::WHILE},
        {"if", DesignEntity::IF},
        {"assign", DesignEntity::ASSIGN},
        {"variable", DesignEntity::VARIABLE},
        {"constant", DesignEntity::CONSTANT},
        {"procedure", DesignEntity::PROCEDURE},
        {"call", DesignEntity::CALL} };

std::unordered_map<AttrName, std::unordered_set<DesignEntity>>
        attrNameToDesignEntityMap = {
        {AttrName::PROCNAME, {DesignEntity::PROCEDURE, DesignEntity::CALL}},
        {AttrName::VARNAME,
                             {DesignEntity::VARIABLE, DesignEntity::READ, DesignEntity::PRINT}},
        {AttrName::VALUE, {DesignEntity::CONSTANT}},
        {AttrName::STMTNUM,
                             {DesignEntity::STMT, DesignEntity::CALL, DesignEntity::READ,
                                     DesignEntity::PRINT, DesignEntity::WHILE, DesignEntity::IF,
                                     DesignEntity::ASSIGN}} };

bool Query::isValid() const { return valid; }

void Query::setValid(bool validity) { valid = validity; }

bool Query::hasDeclaration(const std::string& name) const {
    return declarations.count(name) > 0;
}

bool Query::hasSelectElem(const Elem& e) const {
    return selectResults.hasElem(e);
}

DesignEntity Query::getDeclarationDesignEntity(const std::string& name) const {
    if (declarations.count(name) == 0)
        throw exceptions::PqlSyntaxException(
                messages::qps::parser::declarationDoesNotExistMessage);

    return declarations.find(name)->second;
}

std::unordered_map<std::string, DesignEntity> Query::getDeclarations() const {
    return declarations;
}

ResultCl Query::getResultCl() const { return selectResults; }

std::vector<std::shared_ptr<RelRef>> Query::getSuchthat() const {
    return suchthat;
}

std::vector<Pattern> Query::getPattern() const { return pattern; }

std::vector<AttrCompare> Query::getWith() const { return with; }

void Query::addDeclaration(const std::string& var, DesignEntity de) {
    if (declarations.find(var) != declarations.end())
        throw exceptions::PqlSyntaxException("Declaration already exists!");

    declarations.emplace(var, de);
}

void Query::addResultCl(const ResultCl& resultCl) { selectResults = resultCl; }

void Query::addSuchthat(const std::shared_ptr<RelRef>& rel) {
    suchthat.push_back(rel);
}

void Query::addPattern(const Pattern& p) { pattern.push_back(p); }

void Query::addWith(const AttrCompare& ac) { with.push_back(ac); }

bool AttrRef::canBeCompared(const AttrRef &o) const {
    return (isString() && o.isString()) || (isNumber() && o.isNumber());
}

Elem Elem::ofDeclaration(Declaration d) {
    Elem e;
    e.declaration = std::move(d);
    e.type = ElemType::DECLARATION;
    return e;
}

Elem Elem::ofAttrRef(AttrRef ar) {
    Elem e;
    e.ar = std::move(ar);
    e.type = ElemType::ATTR_REF;
    return e;
}

std::string Elem::getSyn() const {
    if (isDeclaration()) return getDeclaration().getSynonym();
    else
        return getAttrRef().getDeclarationSynonym();
}

ResultCl ResultCl::ofBoolean() {
    ResultCl r;
    r.boolean = true;
    return r;
}

ResultCl ResultCl::ofTuple(std::vector<Elem> tuple) {
    ResultCl r;
    r.tuple = std::move(tuple);
    return r;
}

bool ResultCl::hasElem(const Elem& e) const {
    return std::find(tuple.begin(), tuple.end(), e) != tuple.end();
}

std::vector<std::string> ResultCl::getSynAsList() const {
    std::unordered_set<std::string> distinctSyn;
    for (auto elem : getTuple()) {
        distinctSyn.insert(elem.getSyn());
    }
    std::vector<std::string> syns(distinctSyn.begin(), distinctSyn.end());
    return syns;
}

EntRef EntRef::ofVarName(std::string name) {
    EntRef e;
    e.variable = std::move(name);
    e.type = EntRefType::VARIABLE_NAME;
    return e;
}

EntRef EntRef::ofDeclaration(Declaration d) {
    EntRef e;
    e.declaration = std::move(d);
    e.type = EntRefType::DECLARATION;
    return e;
}

EntRef EntRef::ofWildcard() {
    EntRef e;
    e.type = EntRefType::WILDCARD;
    return e;
}

EntRefType EntRef::getType() const { return type; }

Declaration EntRef::getDeclaration() const { return declaration; }

std::string EntRef::getDeclarationSynonym() const {
    return declaration.getSynonym();
}

DesignEntity EntRef::getDeclarationType() const { return declaration.type; }

std::string EntRef::getVariableName() const { return variable; }

bool EntRef::isDeclaration() const { return type == EntRefType::DECLARATION; }

bool EntRef::isVarName() const { return type == EntRefType::VARIABLE_NAME; }

bool EntRef::isWildcard() const { return type == EntRefType::WILDCARD; }

StmtRef StmtRef::ofDeclaration(Declaration d) {
    StmtRef s;
    s.type = StmtRefType::DECLARATION;
    s.declaration = std::move(d);
    return s;
}

StmtRef StmtRef::ofLineNo(int lineNo) {
    StmtRef s;
    s.type = StmtRefType::LINE_NO;
    s.lineNo = lineNo;
    return s;
}

StmtRef StmtRef::ofWildcard() {
    StmtRef s;
    s.type = StmtRefType::WILDCARD;
    return s;
}

StmtRefType StmtRef::getType() const { return type; }

Declaration StmtRef::getDeclaration() const { return declaration; }

DesignEntity StmtRef::getDeclarationType() const {
    return declaration.getType();
}

std::string StmtRef::getDeclarationSynonym() const {
    return declaration.getSynonym();
}

int StmtRef::getLineNo() const { return lineNo; }

bool StmtRef::isDeclaration() const { return type == StmtRefType::DECLARATION; }

bool StmtRef::isLineNo() const { return type == StmtRefType::LINE_NO; }

bool StmtRef::isWildcard() const { return type == StmtRefType::WILDCARD; }

ExpSpec ExpSpec::ofWildcard() { return ExpSpec{ true, false, "" }; }

ExpSpec ExpSpec::ofPartialMatch(std::string str) {
    return ExpSpec{ false, true, std::move(str) };
}

ExpSpec ExpSpec::ofFullMatch(std::string str) {
    return ExpSpec{ false, false, std::move(str) };
}

bool ExpSpec::isPartialMatch() const {
    return partialMatch && (pattern.length() > 0) && !wildCard;
}

bool ExpSpec::isFullMatch() const {
    return !partialMatch && (pattern.length() > 0) && !wildCard;
}

bool ExpSpec::isWildcard() const {
    return wildCard && (pattern.length() == 0) && !partialMatch;
}

std::string ExpSpec::getPattern() const { return pattern; }

PKBField PKBFieldTransformer::transformStmtRef(const StmtRef& s) {
    PKBField stmtField;
    if (s.isLineNo()) {
        stmtField = PKBField::createConcrete(STMT_LO{ s.getLineNo() });
    } else if (s.isWildcard()) {
        stmtField = PKBField::createWildcard(PKBEntityType::STATEMENT);
    } else if (s.isDeclaration()) {
        StatementType type = evaluator::PKBTypeMatcher::getStatementType(s.getDeclarationType());
        stmtField = PKBField::createDeclaration(type);
    }
    return stmtField;
}

PKBField PKBFieldTransformer::transformEntRefVar(EntRef e) {
    PKBField entField;
    if (e.isVarName()) {
        entField = PKBField::createConcrete(VAR_NAME{ e.getVariableName() });
    } else if (e.isWildcard()) {
        entField = PKBField::createWildcard(PKBEntityType::VARIABLE);
    } else if (e.isDeclaration()) {
        entField = PKBField::createDeclaration(PKBEntityType::VARIABLE);
    }
    return entField;
}

PKBField PKBFieldTransformer::transformEntRefProc(EntRef e) {
    PKBField entField;
    if (e.isVarName()) {
        entField = PKBField::createConcrete(PROC_NAME{ e.getVariableName() });
    } else if (e.isWildcard()) {
        entField = PKBField::createWildcard(PKBEntityType::PROCEDURE);
    } else if (e.isDeclaration()) {
        entField = PKBField::createDeclaration(PKBEntityType::PROCEDURE);
    }
    return entField;
}

ExpSpec Pattern::getExpression() const {
    if (declaration.getType() != DesignEntity::ASSIGN)
        throw exceptions::PqlSyntaxException(
                messages::qps::parser::notAnAssignPatternMessage);

    return expression;
}

Pattern Pattern::ofAssignPattern(std::string synonym, EntRef er, ExpSpec exp) {
    Pattern p;
    p.declaration = Declaration{ std::move(synonym), DesignEntity::ASSIGN };
    p.lhs = std::move(er);
    p.expression = std::move(exp);
    return p;
}

Pattern Pattern::ofWhilePattern(std::string synonym, EntRef er) {
    Pattern p;
    p.declaration = Declaration{ std::move(synonym), DesignEntity::WHILE };
    p.lhs = std::move(er);
    return p;
}

Pattern Pattern::ofIfPattern(std::string synonym, EntRef er) {
    Pattern p;
    p.declaration = Declaration{ std::move(synonym), DesignEntity::IF };
    p.lhs = std::move(er);
    return p;
}

std::vector<PKBField> ModifiesS::getField() {
    return getFieldHelper(&ModifiesS::modifiesStmt, &ModifiesS::modified);
}

std::vector<Declaration> ModifiesS::getDecs() {
    return getDecsHelper(&ModifiesS::modifiesStmt, &ModifiesS::modified);
}

bool ModifiesS::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&ModifiesS::modifiesStmt, &ModifiesS::modified,
                               &r);
}

void ModifiesS::checkFirstArg() {
    if (modifiesStmt.isWildcard())
        throw exceptions::PqlSemanticException(
                messages::qps::parser::cannotBeWildcardMessage);
}

void ModifiesS::checkSecondArg() {
    if (modified.isDeclaration() &&
        modified.getDeclarationType() != DesignEntity::VARIABLE)
        throw exceptions::PqlSemanticException(
                messages::qps::parser::notVariableSynonymMessage);
}

size_t ModifiesS::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, modified);
    hash_combine(seed, modifiesStmt);
    return seed;
}

std::vector<PKBField> ModifiesP::getField() {
    PKBField field1 = PKBFieldTransformer::transformEntRefProc(modifiesProc);
    PKBField field2 = PKBFieldTransformer::transformEntRefVar(modified);
    return std::vector<PKBField>{field1, field2};
}

std::vector<Declaration> ModifiesP::getDecs() {
    return getDecsHelper(&ModifiesP::modifiesProc, &ModifiesP::modified);
}

bool ModifiesP::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&ModifiesP::modifiesProc, &ModifiesP::modified,
                               &r);
}

void ModifiesP::checkFirstArg() {
    if (modifiesProc.isWildcard())
        throw exceptions::PqlSemanticException(
                messages::qps::parser::cannotBeWildcardMessage);
}

void ModifiesP::checkSecondArg() {
    if (modified.isDeclaration() &&
        modified.getDeclarationType() != DesignEntity::VARIABLE)
        throw exceptions::PqlSemanticException(
                messages::qps::parser::notVariableSynonymMessage);
}

size_t ModifiesP::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, modified);
    hash_combine(seed, modifiesProc);
    return seed;
}

std::vector<PKBField> UsesP::getField() {
    PKBField field1 = PKBFieldTransformer::transformEntRefProc(useProc);
    PKBField field2 = PKBFieldTransformer::transformEntRefVar(used);
    return std::vector<PKBField>{field1, field2};
}

std::vector<Declaration> UsesP::getDecs() {
    return getDecsHelper(&UsesP::useProc, &UsesP::used);
}

bool UsesP::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&UsesP::useProc, &UsesP::used, &r);
}

size_t UsesP::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, used);
    hash_combine(seed, useProc);
    return seed;
}

void UsesP::checkFirstArg() {
    if (useProc.isWildcard())
        throw exceptions::PqlSemanticException(
                messages::qps::parser::cannotBeWildcardMessage);
}

void UsesP::checkSecondArg() {
    if (used.isDeclaration() &&
        used.getDeclarationType() != DesignEntity::VARIABLE)
        throw exceptions::PqlSemanticException(
                messages::qps::parser::notVariableSynonymMessage);
}

std::vector<PKBField> UsesS::getField() {
    return getFieldHelper(&UsesS::useStmt, &UsesS::used);
}

std::vector<Declaration> UsesS::getDecs() {
    return getDecsHelper(&UsesS::useStmt, &UsesS::used);
}

bool UsesS::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&UsesS::useStmt, &UsesS::used, &r);
}

size_t UsesS::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, used);
    hash_combine(seed, useStmt);
    return seed;
}

void UsesS::checkFirstArg() {
    if (useStmt.isWildcard())
        throw exceptions::PqlSemanticException(
                messages::qps::parser::cannotBeWildcardMessage);
}

void UsesS::checkSecondArg() {
    if (used.isDeclaration() &&
        used.getDeclarationType() != DesignEntity::VARIABLE)
        throw exceptions::PqlSemanticException(
                messages::qps::parser::notVariableSynonymMessage);
}

std::vector<PKBField> Follows::getField() {
    return getFieldHelper(&Follows::follower, &Follows::followed);
}

std::vector<Declaration> Follows::getDecs() {
    return getDecsHelper(&Follows::follower, &Follows::followed);
}

bool Follows::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&Follows::follower, &Follows::followed, &r);
}

size_t Follows::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, follower);
    hash_combine(seed, followed);

    return seed;
}

std::vector<PKBField> FollowsT::getField() {
    return getFieldHelper(&FollowsT::follower, &FollowsT::transitiveFollowed);
}

std::vector<Declaration> FollowsT::getDecs() {
    return getDecsHelper(&FollowsT::follower, &FollowsT::transitiveFollowed);
}

bool FollowsT::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&FollowsT::follower, &FollowsT::transitiveFollowed,
                               &r);
}

size_t FollowsT::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, follower);
    hash_combine(seed, transitiveFollowed);
    return seed;
}

std::vector<PKBField> Parent::getField() {
    return getFieldHelper(&Parent::parent, &Parent::child);
}

std::vector<Declaration> Parent::getDecs() {
    return getDecsHelper(&Parent::parent, &Parent::child);
}

bool Parent::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&Parent::parent, &Parent::child, &r);
}

size_t Parent::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, parent);
    hash_combine(seed, child);
    return seed;
}

std::vector<PKBField> ParentT::getField() {
    return getFieldHelper(&ParentT::parent, &ParentT::transitiveChild);
}

std::vector<Declaration> ParentT::getDecs() {
    return getDecsHelper(&ParentT::parent, &ParentT::transitiveChild);
}

bool ParentT::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&ParentT::parent, &ParentT::transitiveChild, &r);
}

size_t ParentT::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, parent);
    hash_combine(seed, transitiveChild);
    return seed;
}

std::vector<PKBField> Calls::getField() {
    PKBField field1 = PKBFieldTransformer::transformEntRefProc(caller);
    PKBField field2 = PKBFieldTransformer::transformEntRefProc(callee);
    return std::vector<PKBField>{field1, field2};
}

std::vector<Declaration> Calls::getDecs() {
    return getDecsHelper(&Calls::caller, &Calls::callee);
}

bool Calls::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&Calls::caller, &Calls::callee, &r);
}

size_t Calls::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, caller);
    hash_combine(seed, callee);
    return seed;
}

void Calls::checkFirstArg() {
    if (caller.isDeclaration() &&
        caller.getDeclarationType() != DesignEntity::PROCEDURE) {
        throw exceptions::PqlSemanticException(
                messages::qps::parser::notProcedureSynonymMessage);
    }
}

void Calls::checkSecondArg() {
    if (callee.isDeclaration() &&
        callee.getDeclarationType() != DesignEntity::PROCEDURE) {
        throw exceptions::PqlSemanticException(
                messages::qps::parser::notProcedureSynonymMessage);
    }
}

std::vector<PKBField> CallsT::getField() {
    PKBField field1 = PKBFieldTransformer::transformEntRefProc(caller);
    PKBField field2 = PKBFieldTransformer::transformEntRefProc(transitiveCallee);
    return std::vector<PKBField>{field1, field2};
}

std::vector<Declaration> CallsT::getDecs() {
    return getDecsHelper(&CallsT::caller, &CallsT::transitiveCallee);
}

bool CallsT::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&CallsT::caller, &CallsT::transitiveCallee, &r);
}

size_t CallsT::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, caller);
    hash_combine(seed, transitiveCallee);
    return seed;
}

void CallsT::checkFirstArg() {
    if (caller.isDeclaration() &&
        caller.getDeclarationType() != DesignEntity::PROCEDURE) {
        throw exceptions::PqlSemanticException(
                messages::qps::parser::notProcedureSynonymMessage);
    }
}

void CallsT::checkSecondArg() {
    if (transitiveCallee.isDeclaration() &&
        transitiveCallee.getDeclarationType() != DesignEntity::PROCEDURE) {
        throw exceptions::PqlSemanticException(
                messages::qps::parser::notProcedureSynonymMessage);
    }
}

std::vector<Declaration> Next::getDecs() {
    return getDecsHelper(&Next::before, &Next::after);
}

std::vector<PKBField> Next::getField() {
    return getFieldHelper(&Next::before, &Next::after);
}

bool Next::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&Next::before, &Next::after, &r);
}

size_t Next::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, before);
    hash_combine(seed, after);
    return seed;
}

std::vector<Declaration> NextT::getDecs() {
    return getDecsHelper(&NextT::before, &NextT::transitiveAfter);
}

std::vector<PKBField> NextT::getField() {
    return getFieldHelper(&NextT::before, &NextT::transitiveAfter);
}

bool NextT::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&NextT::before, &NextT::transitiveAfter, &r);
}

size_t NextT::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, before);
    hash_combine(seed, transitiveAfter);
    return seed;
}

std::vector<Declaration> Affects::getDecs() {
    return getDecsHelper(&Affects::affectingStmt, &Affects::affected);
}

std::vector<PKBField> Affects::getField() {
    return getFieldHelper(&Affects::affectingStmt, &Affects::affected);
}

bool Affects::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&Affects::affectingStmt, &Affects::affected, &r);
}

size_t Affects::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, affectingStmt);
    hash_combine(seed, affected);
    return seed;
}

void Affects::checkFirstArg() {
    if (affectingStmt.isDeclaration() &&
        affectingStmt.getDeclarationType() != DesignEntity::ASSIGN) {
        throw exceptions::PqlSemanticException(
                messages::qps::parser::notAssignSynonymMessage);
    }
}

void Affects::checkSecondArg() {
    if (affected.isDeclaration() &&
        affected.getDeclarationType() != DesignEntity::ASSIGN) {
        throw exceptions::PqlSemanticException(
                messages::qps::parser::notAssignSynonymMessage);
    }
}

std::vector<Declaration> AffectsT::getDecs() {
    return getDecsHelper(&AffectsT::affectingStmt, &AffectsT::transitiveAffected);
}

std::vector<PKBField> AffectsT::getField() {
    return getFieldHelper(&AffectsT::affectingStmt,
                          &AffectsT::transitiveAffected);
}

bool AffectsT::equalTo(const RelRef& r) const {
    if (r.type != this->type)
        return false;
    return equalityCheckHelper(&AffectsT::affectingStmt,
                               &AffectsT::transitiveAffected, &r);
}

size_t AffectsT::getHash() const {
    size_t seed = 0;
    hash_combine(seed, type);
    hash_combine(seed, affectingStmt);
    hash_combine(seed, transitiveAffected);
    return seed;
}

void AffectsT::checkFirstArg() {
    if (affectingStmt.isDeclaration() &&
        affectingStmt.getDeclarationType() != DesignEntity::ASSIGN) {
        throw exceptions::PqlSemanticException(
                messages::qps::parser::notAssignSynonymMessage);
    }
}

void AffectsT::checkSecondArg() {
    if (transitiveAffected.isDeclaration() &&
        transitiveAffected.getDeclarationType() != DesignEntity::ASSIGN) {
        throw exceptions::PqlSemanticException(
                messages::qps::parser::notAssignSynonymMessage);
    }
}

AttrCompareRef AttrCompareRef::ofString(std::string str) {
    AttrCompareRef acr;
    acr.type = AttrCompareRefType::STRING;
    acr.str_value = std::move(str);
    return acr;
}

AttrCompareRef AttrCompareRef::ofNumber(int num) {
    AttrCompareRef acr;
    acr.type = AttrCompareRefType::NUMBER;
    acr.number = num;
    return acr;
}

AttrCompareRef AttrCompareRef::ofAttrRef(AttrRef ar) {
    AttrCompareRef acr;
    acr.type = AttrCompareRefType::ATTRREF;
    acr.ar = std::move(ar);
    return acr;
}
}  // namespace qps::query
