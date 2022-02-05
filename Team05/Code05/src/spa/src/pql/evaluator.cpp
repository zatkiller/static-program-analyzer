#include "evaluator.h"
#include "../logging.h"

#define DEBUG Logger(Level::DEBUG) << "evaluator.cpp "

std::string PKBFieldToString(PKBField pkbField) {
    if(pkbField.tag == PKBType::STATEMENT || pkbField.tag == PKBType::CONST) {
        return std::to_string(pkbField.content);
    } else if (pkbField.tag == PKBType::VARIABLE || pkbField.tag == PKBType::PROCEDURE) {
        return pkbField.content;
    }
}

std::unordered_map<DesignEntity, StatementType> StatementTypeMap = {
        {DesignEntity::STMT, StatementType::Statment},
        {DesignEntity::ASSIGN, StatementType::Assignment},
        {DesignEntity::WHILE, StatementType::While},
        {DesignEntity::IF, StatementType::If},
        {DesignEntity::READ, StatementType::Read},
        {DesignEntity::PRINT, StatementType::Print},
        {DesignEntity::CALL, StatementType::Call}
};

PKBResponse getAll(DesignEntity type) {
    PKBResponse result;
    if (type == DesignEntity::PROCEDURE) {
        result = PKBStub::getProcedures();
    } else if (type == DesignEntity::CONSTANT) {
        result = PKBStub::getConst();
    } else if (type == DesignEntity::VARIABLE) {
        result = PKBStub::getVariables();
    } else {
        StatementType sType = StatementTypeMap.find(type)->second;
        result = PKBStub::getStatements(sType);
    }
    return result;
}
//Replace int by PKBField
std::set<int> processSuchthat(std::vector<std::shared_ptr<RelRef>> clauses, DesignEntity returnType) {
    //TODO: Modifies when PKBResponse is definded Replace int with PKBFields
    PKBResponse result;
    //only one clause for now
    for (auto r : clauses) {
        RelRefType type = r.get()->getType();

        if (type == RelRefType::MODIFIESS) {
            std::shared_ptr<Modifies> mPtr = std::dynamic_pointer_cast<Modifies>(r);
            Modifies* modifiesPtr = mPtr.get();
            Modifies m = *modifiesPtr;
            EntRef modified = m.modified;
            StmtRef stmt = m.modifiesStmt;
            EntRefType entType = modified.getType();
            StmtRefType stmtType = stmt.getType();

            if (entType == EntRefType::DECLARATION) {
                //TODO: fill in when creating PKBField
            } else if (entType == EntRefType::VARIABLE_NAME) {
                //TODO: fill in when creating PKBField
            } else if (entType == EntRefType::WILDCARD) {
                //TODO: fill in when creating PKBField
            }

            if (stmtType == StmtRefType::DECLARATION) {

            } else if (stmtType == StmtRefType::LINE_NO) {

            } else if (stmtType == StmtRefType::WILDCARD) {

            }
            // TODO: modifies when PKB API is defined (support one clause only)
            //result = getRelationship(stmt, modified,  PKBRelationship::MODIFIES, returnType);
        } else if (type == RelRefType::USESS) {
            std::shared_ptr<Uses> uPtr = std::dynamic_pointer_cast<Uses>(r);
            Uses* usesPtr = uPtr.get();
            Uses u = *usesPtr;
            EntRef used = u.used;
            StmtRef stmt = u.useStmt;
            EntRefType entType = used.getType();
            StmtRefType stmtType = stmt.getType();

            if (entType == EntRefType::DECLARATION) {
                //TODO: fill in when creating PKBField
            } else if (entType == EntRefType::VARIABLE_NAME) {
                //TODO: fill in when creating PKBField
            } else if (entType == EntRefType::WILDCARD) {
                //TODO: fill in when creating PKBField
            }

            if (stmtType == StmtRefType::DECLARATION) {

            } else if (stmtType == StmtRefType::LINE_NO) {

            } else if (stmtType == StmtRefType::WILDCARD) {

            }
            // TODO: modifies when PKB API is defined
            //PKBReturnType can be the DesignEntity
            //result = getRelationship(stmt, used, PKBRelationship::USES, returnType);
        }
    }
    return result;
}

//Replace int by PKBField
std::set<int> processPattern(std::vector<Pattern> patterns, DesignEntity returnType) {
    std::set<int> result;
    for (auto p : patterns) {
        EntRef lhs = p.getEntRef();
        std::string rhs = p.getExpression();

        // modify when pattern is defined in PKB
        std::string lhsString;
        if (lhs.isWildcard()) {
            lhsString = "_";
        } else if (lhs.isDeclaration()) {
            lhsString = lhs.getDeclaration();
        } else if (lhs.isVarName()) {
            lhsString = lhs.getVariableName();
        }

        std::string pattern = "(" + lhsString + " , " + rhs + ")";
        //result = match(pattern, returnType);
    }
    return result;
}

//replace int by PKBField
std::string processResult(PKBResponse queryResult) {
    std::string stringResult = "";
    std::set<PKBField> content = queryResult.content;

    for (int i = 0; i < content.size(); i ++) {
        if (i == content.size() - 1) {
            stringResult += PKBFieldToString(content[i]);
        } else {
            stringResult = PKBFieldToString(content[i]) + ", ";
        }
    }
    DEBUG << "stringResult";
    return stringResult;
}

std::string evaluate(Query query) {
//    std::unordered_map<std::string, DesignEntity> declarations = query.getDeclarations();
    std::vector<std::string> variable = query.getVariable();
    std::vector<std::shared_ptr<RelRef>> suchthat = query.getSuchthat();
    std::vector<Pattern> pattern = query.getPattern();

    DesignEntity returnType = query.getDeclarationDesignEntity(variable[0]);
    //TODO: replace int with PKBField
    std::set<int> suchthatResult;
    std::set<int> patternResult;
    PKBResponse queryResult;

    PKBStub pkb;

//    if (!suchthat.empty()) {
//        suchthatResult = processSuchthat(suchthat, returnType);
//    }
//
//    if (!pattern.empty()) {
//        patternResult = processPattern(pattern, returnType);
//    }

    if (suchthat.empty() && pattern.empty()) {
        queryResult = getAll(returnType);

    } else {
//        int size = std::min(suchthatResult.size(), patternResult.size());
//        std::vector<int> v1(size);
//        std::vector<int>::iterator it, ls;
//        ls = std::set_intersection(suchthatResult.begin(), suchthatResult.end(), patternResult.begin(), patternResult.end(), v1.begin());
//        for (it = v1.begin(); it != ls; ++it) {
//            queryResult.insert(*it);
//        }
    }

    return processResult(queryResult);
}