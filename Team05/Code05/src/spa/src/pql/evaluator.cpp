#include "evaluator.h"

#define DEBUG Logger(Level::DEBUG) << "evaluator.cpp "

std::string PKBFieldToString(PKBField pkbField) {
    std::string res = "";
    if(pkbField.tag == PKBType::STATEMENT) {
        int lineNo = std::get<STMT_LO>(pkbField.content).statementNum;
        res = std::to_string(lineNo);
    } else if ( pkbField.tag == PKBType::CONST) {
        int c = std::get<CONST>(pkbField.content);
        res = std::to_string(c);
    } else if (pkbField.tag == PKBType::VARIABLE) {
        res = std::get<VAR_NAME>(pkbField.content).name;
    } else if (pkbField.tag == PKBType::PROCEDURE) {
        res = std::get<PROC_NAME>(pkbField.content).name;
    }
    return res;
}

PKBResponse getAll(DesignEntity type) {
    std::unordered_map<DesignEntity, StatementType> StatementTypeMap = {
//            {DesignEntity::STMT, StatementType::Statement},
            {DesignEntity::ASSIGN, StatementType::Assignment},
            {DesignEntity::WHILE, StatementType::While},
            {DesignEntity::IF, StatementType::If},
            {DesignEntity::READ, StatementType::Read},
            {DesignEntity::PRINT, StatementType::Print},
            {DesignEntity::CALL, StatementType::Call}
    };
    PKBResponse result;
    PKB pkb = PKB();
    if (type == DesignEntity::PROCEDURE) {
        result = pkb.getProcedures();
    } else if (type == DesignEntity::CONSTANT) {
        result = pkb.getConstants();
    } else if (type == DesignEntity::VARIABLE) {
        result = pkb.getVariables();
    } else {
        StatementType sType = StatementTypeMap.find(type)->second;
        result = pkb.getStatements(sType);
    }
    return result;
}


// replace int by PKBField
std::string processResult(PKBResponse queryResult) {
    std::string stringResult = "";
    if(!queryResult.hasResult) {
        return stringResult;
    }
    std::unordered_set<PKBField, PKBFieldHash> result = std::get<std::unordered_set<PKBField, PKBFieldHash>>(queryResult.res);
    int count = 0;
    for (auto field : result) {
        if (count == result.size() - 1) {
            stringResult += PKBFieldToString(field);
        } else {
            stringResult = stringResult + PKBFieldToString(field) + ", ";
        }
    }

    return stringResult;
}

std::string evaluate(Query query) {
    // std::unordered_map<std::string, DesignEntity> declarations = query.getDeclarations();
    std::vector<std::string> variable = query.getVariable();
    std::vector<std::shared_ptr<RelRef>> suchthat = query.getSuchthat();
    std::vector<Pattern> pattern = query.getPattern();

    DesignEntity returnType = query.getDeclarationDesignEntity(variable[0]);
    // TO DO: replace int with PKBField
    std::set<int> suchthatResult;
    std::set<int> patternResult;
    PKBResponse queryResult;

    if (suchthat.empty() && pattern.empty()) {
        queryResult = getAll(returnType);
    }

    return processResult(queryResult);
}
