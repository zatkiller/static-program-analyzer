#include "evaluator.h"

//Replace int with PKBField
std::set<int> processSuchthat(std::vector<RelRef> clauses, DesignEntity returnType) {
    //TODO: Modifies when PKBResponse is definded Replace int with PKBFields
    std::set<int> result;
    //only one clause for now
    for (auto r : clauses) {
        RelRefType type = r.type;
        if (type != RelRefType::INVALID) {
            continue;
        } else {
            throw std::runtime_error("Relationship invalid");
        }

        if (type == RelRefType::MODIFIESS) {
            EntRef modified = r.modified;
            StmtRef stmt = r.modifiesStmt;
            EntRefType entType = modified.getType();
            StmtRefType stmtType = stmt.getType();

            if (entType == EntRefType::DECLARATION) {
                //TODO: fill in when creating PKBField
            } else if (entType == EntRefType::VARIABLE_NAME) {
                //TODO: fill in when creating PKBField
            } else if (entType == EntRefType::WILDCARD) {
                //TODO: fill in when creating PKBField
            } else {
                throw std::runtime_error("Entity Reference has incorrect format in Modifies relationship")
            }

            if (stmtType == StmtRefType::DECLARATION) {

            } else if (stmtType == StmtRefType::LINE_NO) {

            } else if (stmtType == StmtRefType::WILDCARD) {

            } else {
                throw std::runtime_error("Statement Reference has incorrect format in Modifeis relationship")
            }
            // TODO: modifies when PKB API is defined (support one clause only)
            result = getRelationship(stmt, modified,  PKBRelationship::MODIFIES, returnType)
        } else if (type == RelRefType::USESS) {
            EntRef used = r.used;
            StmtRef stmt = r.useStmt;
            EntRefType entType = used.getType();
            StmtRefType stmtType = stmt.getType();

            if (entType == EntRefType::DECLARATION) {
                //TODO: fill in when creating PKBField
            } else if (entType == EntRefType::VARIABLE_NAME) {
                //TODO: fill in when creating PKBField
            } else if (entType == EntRefType::WILDCARD) {
                //TODO: fill in when creating PKBField
            } else {
                throw std::runtime_error("Entity Reference has incorrect format in Modifies relationship")
            }

            if (stmtType == StmtRefType::DECLARATION) {

            } else if (stmtType == StmtRefType::LINE_NO) {

            } else if (stmtType == StmtRefType::WILDCARD) {

            } else {
                throw std::runtime_error("Statement Reference has incorrect format in Modifeis relationship")
            }
            // TODO: modifies when PKB API is defined
            //PKBReturnType can be the DesignEntity
            result = getRelationship(stmt, used, PKBRelationship::USES, returnType)
        }
        return result
    }
}

//Replace int with PKBField
std::set<int> processPattern(std::vector<Pattern> patterns, DesignEntity returnType) {
    for (auto p : patterns) {

    }
}

//select s
std::string evaluate(Query query) {
    std::unordered_map<std::string, DesignEntity> declarations = query.getDeclarations();
    std::vector<std::string> variable = query.getVariable();
    std::vector<RelRef> suchthat = query.getSuchthat();
    std::vector<Pattern> pattern = query.getPattern();

    DesignEntity returnType = getDeclarationDesignEntity(variable[0]);
    std::set<int> suchthatResult;
    if (!suchthat.empty()) {
        suchthatResult = processSuchthat(suchthat, returnType);
    }

    if (!pattern.empty()) {
        processPattern(pattern, returnType);
    }
}