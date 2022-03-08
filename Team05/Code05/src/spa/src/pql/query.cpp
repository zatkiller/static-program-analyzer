#include <algorithm>

#include "exceptions.h"
#include "pql/query.h"

namespace qps::query {
    std::unordered_map<std::string, DesignEntity> designEntityMap = {
            {"stmt",      DesignEntity::STMT},
            {"read",      DesignEntity::READ},
            {"print",     DesignEntity::PRINT},
            {"while",     DesignEntity::WHILE},
            {"if",        DesignEntity::IF},
            {"assign",    DesignEntity::ASSIGN},
            {"variable",  DesignEntity::VARIABLE},
            {"constant",  DesignEntity::CONSTANT},
            {"procedure", DesignEntity::PROCEDURE}
    };

    bool Query::isValid() {
        return valid;
    }

    void Query::setValid(bool valid) {
        this->valid = valid;
    }

    bool Query::hasDeclaration(std::string name) {
        return declarations.count(name) > 0;
    }

    bool Query::hasVariable(std::string var) {
        return std::find(variable.begin(), variable.end(), var) != variable.end();
    }

    DesignEntity Query::getDeclarationDesignEntity(std::string name) {
        if (declarations.count(name) == 0)
            throw exceptions::PqlSyntaxException(messages::qps::parser::declarationDoesNotExistMessage);

        return declarations.find(name)->second;
    }

    std::unordered_map<std::string, DesignEntity> Query::getDeclarations() {
        return declarations;
    }

    std::vector<std::string> Query::getVariable() {
        return variable;
    }

    std::vector<std::shared_ptr<RelRef>> Query::getSuchthat() {
        return suchthat;
    }

    std::vector<Pattern> Query::getPattern() {
        return pattern;
    }

    void Query::addDeclaration(std::string var, DesignEntity de) {
        if (declarations.find(var) != declarations.end())
            throw exceptions::PqlSyntaxException("Declaration already exists!");

        declarations.insert({var, de});
    }


    void Query::addVariable(std::string var) {
        variable.push_back(var);
    }

    void Query::addSuchthat(std::shared_ptr<RelRef> rel) {
        suchthat.push_back(rel);
    }

    void Query::addPattern(Pattern p) {
        pattern.push_back(p);
    }


    EntRef EntRef::ofVarName(std::string name) {
        EntRef e;
        e.variable = name;
        e.type = EntRefType::VARIABLE_NAME;
        return e;
    }

    EntRef EntRef::ofDeclaration(std::string d, DesignEntity de) {
        EntRef e;
        e.declaration = d;
        e.type = EntRefType::DECLARATION;
        e.declarationType = de;
        return e;
    }

    EntRef EntRef::ofWildcard() {
        EntRef e;
        e.type = EntRefType::WILDCARD;
        return e;
    }

    EntRefType EntRef::getType() {
        return type;
    }

    std::string EntRef::getDeclaration() const {
        return declaration;
    }

    DesignEntity EntRef::getDeclarationType() const {
        return declarationType;
    }

    std::string EntRef::getVariableName() {
        return variable;
    }

    bool EntRef::isDeclaration() const {
        return type == EntRefType::DECLARATION;
    }

    bool EntRef::isVarName() {
        return type == EntRefType::VARIABLE_NAME;
    }

    bool EntRef::isWildcard() {
        return type == EntRefType::WILDCARD;
    }

    StmtRef StmtRef::ofDeclaration(std::string d, DesignEntity de) {
        StmtRef s;
        s.type = StmtRefType::DECLARATION;
        s.declaration = d;
        s.declarationType = de;
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

    StmtRefType StmtRef::getType() {
        return type;
    }

    DesignEntity StmtRef::getDeclarationType() const {
        return declarationType;
    }

    std::string StmtRef::getDeclaration() const {
        return declaration;
    }

    int StmtRef::getLineNo() {
        return lineNo;
    }

    bool StmtRef::isDeclaration() const {
        return type == StmtRefType::DECLARATION;
    }

    bool StmtRef::isLineNo() {
        return type == StmtRefType::LINE_NO;
    }

    bool StmtRef::isWildcard() {
        return type == StmtRefType::WILDCARD;
    }

    ExpSpec ExpSpec::ofWildcard() {
        return ExpSpec {true, false, ""};
    }

    ExpSpec ExpSpec::ofPartialMatch(std::string str) {
        return ExpSpec { false, true, str };
    }

    ExpSpec ExpSpec::ofFullMatch(std::string str) {
        return ExpSpec { false, true, str };
    }

    bool ExpSpec::isPartialMatch() {
        return partialMatch && (pattern.length() > 0) && !wildCard;
    }

    bool ExpSpec::isFullMatch() {
        return !partialMatch && (pattern.length() > 0) && !wildCard;
    }

    bool ExpSpec::isWildcard() {
        return wildCard && (pattern.length() < 0) && !partialMatch;
    }

    std::string ExpSpec::getPattern() {
        return pattern;
    }

    PKBField PKBFieldTransformer::transformStmtRef(StmtRef s) {
        PKBField stmtField;
        if (s.isLineNo()) {
            stmtField = PKBField::createConcrete(STMT_LO{s.getLineNo()});
        } else if (s.isWildcard()) {
            stmtField = PKBField::createWildcard(PKBEntityType::STATEMENT);
        } else if (s.isDeclaration()) {
            stmtField = PKBField::createDeclaration(StatementType::All);
        }
        return stmtField;
    }

    PKBField PKBFieldTransformer::transformEntRef(EntRef e) {
        PKBField entField;
        if (e.isVarName()) {
            entField = PKBField::createConcrete(VAR_NAME{e.getVariableName()});
        } else if (e.isWildcard()) {
            entField = PKBField::createWildcard(PKBEntityType::VARIABLE);
        } else if (e.isDeclaration()) {
            entField = PKBField::createDeclaration(PKBEntityType::VARIABLE);
        }
        return entField;
    }

    std::string Pattern::getSynonym() {
        return synonym;
    }

    EntRef Pattern::getEntRef() {
        return lhs;
    }

    ExpSpec Pattern::getExpression() {
        return expression;
    }

    std::vector<PKBField> ModifiesS::getField() {
        return getFieldHelper(&ModifiesS::modifiesStmt, &ModifiesS::modified);
    }

    std::vector<std::string> ModifiesS::getSyns() {
        return getSynsHelper(&ModifiesS::modifiesStmt, &ModifiesS::modified);
    }

    void ModifiesS::checkFirstArg() {
        if (modifiesStmt.isWildcard())
            throw exceptions::PqlSemanticException(messages::qps::parser::cannotBeWildcardMessage);
    }

    void ModifiesS::checkSecondArg() {
        if(modified.isDeclaration() && modified.getDeclarationType() != DesignEntity::VARIABLE)
            throw exceptions::PqlSemanticException(messages::qps::parser::notVariableSynonymMessage);
    }

    std::vector<PKBField> ModifiesP::getField() {
        return getFieldHelper(&ModifiesP::modifiesProc, &ModifiesP::modified);
    }

    std::vector<std::string> ModifiesP::getSyns() {
        return getSynsHelper(&ModifiesP::modifiesProc, &ModifiesP::modified);
    }

    void ModifiesP::checkFirstArg() {
        if (modifiesProc.isWildcard())
            throw exceptions::PqlSemanticException(messages::qps::parser::cannotBeWildcardMessage);
    }

    void ModifiesP::checkSecondArg() {
        if(modified.isDeclaration() && modified.getDeclarationType() != DesignEntity::VARIABLE)
            throw exceptions::PqlSemanticException(messages::qps::parser::notVariableSynonymMessage);
    }


    std::vector<PKBField> UsesP::getField() {
        return getFieldHelper(&UsesP::useProc, &UsesP::used);
    }

    std::vector<std::string> UsesP::getSyns()  {
        return getSynsHelper(&UsesP::useProc, &UsesP::used);
    }

    void UsesP::checkFirstArg() {
        if (useProc.isWildcard())
            throw exceptions::PqlSemanticException(messages::qps::parser::cannotBeWildcardMessage);
    }

    void UsesP::checkSecondArg() {
        if (used.isDeclaration() && used.getDeclarationType() != DesignEntity::VARIABLE)
            throw exceptions::PqlSemanticException(messages::qps::parser::notVariableSynonymMessage);
    }

    std::vector<PKBField> UsesS::getField() {
        return getFieldHelper(&UsesS::useStmt, &UsesS::used);
    }

    std::vector<std::string> UsesS::getSyns()  {
        return getSynsHelper(&UsesS::useStmt, &UsesS::used);
    }

    void UsesS::checkFirstArg() {
        if (useStmt.isWildcard())
            throw exceptions::PqlSemanticException(messages::qps::parser::cannotBeWildcardMessage);
    }

    void UsesS::checkSecondArg() {
        if (used.isDeclaration() && used.getDeclarationType() != DesignEntity::VARIABLE)
            throw exceptions::PqlSemanticException(messages::qps::parser::notVariableSynonymMessage);
    }

    std::vector<PKBField> Follows::getField() {
        return getFieldHelper(&Follows::follower, &Follows::followed);
    }

    std::vector<std::string> Follows::getSyns() {
        return getSynsHelper(&Follows::follower, &Follows::followed);
    }

    std::vector<PKBField> FollowsT::getField() {
        return getFieldHelper(&FollowsT::follower, &FollowsT::transitiveFollowed);
    }

    std::vector<std::string> FollowsT::getSyns() {
        return getSynsHelper(&FollowsT::follower, &FollowsT::transitiveFollowed);
    }

    std::vector<PKBField> Parent::getField() {
        return getFieldHelper(&Parent::parent, &Parent::child);
    }

    std::vector<std::string> Parent::getSyns() {
        return getSynsHelper(&Parent::parent, &Parent::child);
    }

    std::vector<PKBField> ParentT::getField() {
        return getFieldHelper(&ParentT::parent, &ParentT::transitiveChild);
    }

    std::vector<std::string> ParentT::getSyns() {
        return getSynsHelper(&ParentT::parent, &ParentT::transitiveChild);
    }

    std::vector<PKBField> Calls::getField() {
        return getFieldHelper(&Calls::caller, &Calls::callee);
    }

    std::vector<std::string> Calls::getSyns() {
        return getSynsHelper(&Calls::caller, &Calls::callee);
    }

    void Calls::checkFirstArg() {
        if (caller.isDeclaration() && caller.getDeclarationType() != DesignEntity::PROCEDURE) {
            throw exceptions::PqlSemanticException(messages::qps::parser::notProcedureSynonymMessage);
        }
    }

    void Calls::checkSecondArg() {
        if (callee.isDeclaration() && callee.getDeclarationType() != DesignEntity::PROCEDURE) {
            throw exceptions::PqlSemanticException(messages::qps::parser::notProcedureSynonymMessage);
        }
    }

    std::vector<PKBField> CallsT::getField() {
        return getFieldHelper(&CallsT::caller, &CallsT::transitiveCallee);
    }

    std::vector<std::string> CallsT::getSyns() {
        return getSynsHelper(&CallsT::caller, &CallsT::transitiveCallee);
    }

    void CallsT::checkFirstArg() {
        if (caller.isDeclaration() && caller.getDeclarationType() != DesignEntity::PROCEDURE) {
            throw exceptions::PqlSemanticException(messages::qps::parser::notProcedureSynonymMessage);
        }
    }

    void CallsT::checkSecondArg() {
        if (transitiveCallee.isDeclaration() && transitiveCallee.getDeclarationType() != DesignEntity::PROCEDURE) {
            throw exceptions::PqlSemanticException(messages::qps::parser::notProcedureSynonymMessage);
        }
    }

    std::vector<std::string> Next::getSyns() {
        return getSynsHelper(&Next::before, &Next::after);
    }

    std::vector<PKBField> Next::getField() {
        return getFieldHelper(&Next::before, &Next::after);
    }

    std::vector<std::string> NextT::getSyns() {
        return getSynsHelper(&NextT::before, &NextT::transitiveAfter);
    }

    std::vector<PKBField> NextT::getField() {
        return getFieldHelper(&NextT::before, &NextT::transitiveAfter);
    }

    AttrCompareRef AttrCompareRef::ofString(std::string str) {
        AttrCompareRef acr;
        acr.type = AttrCompareRefType::STRING;
        acr.str_value = str;
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
        acr.ar = ar;
        return acr;
    }

}  // namespace qps::query
