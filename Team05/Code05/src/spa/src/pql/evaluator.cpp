#include "evaluator.h"
#include "../logging.h"

#define DEBUG Logger(Level::DEBUG) << "evaluator.cpp "

std::string PKBField::to_string() {
    if(tag == PKBType::STMT_LO) {
        return std::to_string(stmtLo);
    } else if (tag == PKBType::VAR_NAME) {
        return vName;
    } else if (tag == PKBType::PROC_NAME) {
        return pName;
    } else {
        return std::to_string(cName);
    }
}

std::vector<PKBField> PKBStub::getAll(DesignEntity type) {
    std::vector<PKBField> result;
    if(type == DesignEntity::PROCEDURE) {

    } else if (type == DesignEntity:: VARIABLE) {
        std::set<std::string> varlist = {"a", "x", "y", "present"};
        for (auto v : varlist) {
            PKBField vf;
            vf.tag = PKBType::VAR_NAME;
            vf.isConcrete = false;
            vf.vName = v;
            result.push_back(vf);
        }
    } else {
        std::set<int> stmtList = {1, 2, 3};
        for (auto s : stmtList) {
            PKBField sf;
            sf.tag = PKBType::STMT_LO;
            sf.isConcrete = false;
            sf.stmtLo = s;
            result.push_back(sf);
        }
    }
    return result;
}

//Replace int by PKBField
std::set<int> processSuchthat(std::vector<std::shared_ptr<RelRef>> clauses, DesignEntity returnType) {
    //TODO: Modifies when PKBResponse is definded Replace int with PKBFields
    std::set<int> result;
    //only one clause for now
    for (auto r : clauses) {
        RelRefType type = r.get()->getType();
        if (type == RelRefType::INVALID) {
            throw std::runtime_error("Relationship invalid");
        }

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
            } else {
                throw std::runtime_error("Entity Reference has incorrect format in Modifies relationship");
            }

            if (stmtType == StmtRefType::DECLARATION) {

            } else if (stmtType == StmtRefType::LINE_NO) {

            } else if (stmtType == StmtRefType::WILDCARD) {

            } else {
                throw std::runtime_error("Statement Reference has incorrect format in Modifeis relationship");
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
            } else {
                throw std::runtime_error("Entity Reference has incorrect format in Modifies relationship");
            }

            if (stmtType == StmtRefType::DECLARATION) {

            } else if (stmtType == StmtRefType::LINE_NO) {

            } else if (stmtType == StmtRefType::WILDCARD) {

            } else {
                throw std::runtime_error("Statement Reference has incorrect format in Modifeis relationship");
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
std::string processResult(std::vector<PKBField> queryResult) {
    std::string stringResult = "";
    for (int i = 0; i < queryResult.size(); i ++) {
        if (i == queryResult.size() - 1) {
            stringResult += queryResult[i].to_string();
        } else {
            stringResult = stringResult + queryResult[i].to_string() + ", ";
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
    std::vector<PKBField> queryResult;

    PKBStub pkb;

//    if (!suchthat.empty()) {
//        suchthatResult = processSuchthat(suchthat, returnType);
//    }
//
//    if (!pattern.empty()) {
//        patternResult = processPattern(pattern, returnType);
//    }

    if (suchthat.empty() && pattern.empty()) {
        queryResult = pkb.getAll(returnType);

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