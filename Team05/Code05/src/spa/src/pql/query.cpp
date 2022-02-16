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

    EntRef EntRef::ofDeclaration(std::string d) {
        EntRef e;
        e.declaration = d;
        e.type = EntRefType::DECLARATION;
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

    std::string EntRef::getDeclaration() {
        return declaration;
    }

    std::string EntRef::getVariableName() {
        return variable;
    }

    bool EntRef::isDeclaration() {
        return type == EntRefType::DECLARATION;
    }

    bool EntRef::isVarName() {
        return type == EntRefType::VARIABLE_NAME;
    }

    bool EntRef::isWildcard() {
        return type == EntRefType::WILDCARD;
    }

    StmtRef StmtRef::ofDeclaration(std::string d) {
        StmtRef s;
        s.type = StmtRefType::DECLARATION;
        s.declaration = d;
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

    std::string StmtRef::getDeclaration() {
        return declaration;
    }

    int StmtRef::getLineNo() {
        return lineNo;
    }

    bool StmtRef::isDeclaration() {
        return type == StmtRefType::DECLARATION;
    }

    bool StmtRef::isLineNo() {
        return type == StmtRefType::LINE_NO;
    }

    bool StmtRef::isWildcard() {
        return type == StmtRefType::WILDCARD;
    }

    PKBField PKBFieldTransformer::transformStmtRef(StmtRef s) {
        PKBField stmtField;
        if (s.isLineNo()) {
            stmtField = PKBField::createConcrete(STMT_LO{s.getLineNo()});
        } else if (s.isWildcard()) {
            stmtField = PKBField::createWildcard(PKBEntityType::STATEMENT);
        } else if (s.isDeclaration()) {
            stmtField = PKBField::createDeclaration(PKBEntityType::STATEMENT);
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

    std::string Pattern::getExpression() {
        return expression;
    }

    std::vector<PKBField> Modifies::getField() {
        PKBField mStmtField = PKBFieldTransformer::transformStmtRef(modifiesStmt);
        PKBField modifiedField = PKBFieldTransformer::transformEntRef(modified);
        return std::vector<PKBField>{mStmtField, modifiedField};
    }

    std::vector<std::string> Modifies::getSyns(){
        std::vector<std::string> synonyms;
        if (modifiesStmt.isDeclaration()) {
            synonyms.push_back(modifiesStmt.getDeclaration());
        }
        if (modified.isDeclaration()) {
            synonyms.push_back(modified.getDeclaration());
        }
        return synonyms;
    }

    std::vector<PKBField> Uses::getField() {
        PKBField uStmtField = PKBFieldTransformer::transformStmtRef(useStmt);
        PKBField usedField = PKBFieldTransformer::transformEntRef(used);
        return std::vector<PKBField>{uStmtField, usedField};
    }

    std::vector<std::string> Uses::getSyns()  {
        std::vector<std::string> synonyms;
        if (useStmt.isDeclaration()) {
            synonyms.push_back(useStmt.getDeclaration());
        }
        if (used.isDeclaration()) {
            synonyms.push_back(used.getDeclaration());
        }
        return synonyms;
    }

    std::vector<PKBField> Follows::getField() {
        PKBField followerField = PKBFieldTransformer::transformStmtRef(follower);
        PKBField followedField = PKBFieldTransformer::transformStmtRef(followed);
        return std::vector<PKBField>{followerField, followedField};
    }

    std::vector<std::string> Follows::getSyns() {
        std::vector<std::string> synonyms;
        if (follower.isDeclaration()) {
            synonyms.push_back(follower.getDeclaration());
        }
        if (followed.isDeclaration()) {
            synonyms.push_back(followed.getDeclaration());
        }
        return synonyms;
    }

    std::vector<PKBField> FollowsT::getField() {
        PKBField followerField = PKBFieldTransformer::transformStmtRef(follower);
        PKBField followedField = PKBFieldTransformer::transformStmtRef(transitiveFollowed);
        return std::vector<PKBField>{followerField, followedField};
    }

    std::vector<std::string> FollowsT::getSyns() {
        std::vector<std::string> synonyms;
        if (follower.isDeclaration()) {
            synonyms.push_back(follower.getDeclaration());
        }
        if (transitiveFollowed.isDeclaration()) {
            synonyms.push_back(transitiveFollowed.getDeclaration());
        }
        return synonyms;
    }

    std::vector<PKBField> Parent::getField() {
        PKBField followerField = PKBFieldTransformer::transformStmtRef(parent);
        PKBField followedField = PKBFieldTransformer::transformStmtRef(child);
        return std::vector<PKBField>{followerField, followedField};
    }

    std::vector<std::string> Parent::getSyns() {
        std::vector<std::string> synonyms;
        if (parent.isDeclaration()) {
            synonyms.push_back(parent.getDeclaration());
        }
        if (child.isDeclaration()) {
            synonyms.push_back(child.getDeclaration());
        }
        return synonyms;
    }

    std::vector<PKBField> ParentT::getField() {
        PKBField followerField = PKBFieldTransformer::transformStmtRef(parent);
        PKBField followedField = PKBFieldTransformer::transformStmtRef(transitiveChild);
        return std::vector<PKBField>{followerField, followedField};
    }

    std::vector<std::string> ParentT::getSyns() {
        std::vector<std::string> synonyms;
        if (parent.isDeclaration()) {
            synonyms.push_back(parent.getDeclaration());
        }
        if (transitiveChild.isDeclaration()) {
            synonyms.push_back(transitiveChild.getDeclaration());
        }
        return synonyms;
    }
}  // namespace qps::query
