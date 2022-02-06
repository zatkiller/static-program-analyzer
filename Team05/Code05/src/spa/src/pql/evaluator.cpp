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

std::unordered_map<DesignEntity, StatementType> StatementTypeMap = {
        {DesignEntity::STMT, StatementType::Statement},
        {DesignEntity::ASSIGN, StatementType::Assignment},
        {DesignEntity::WHILE, StatementType::While},
        {DesignEntity::IF, StatementType::If},
        {DesignEntity::READ, StatementType::Read},
        {DesignEntity::PRINT, StatementType::Print},
        {DesignEntity::CALL, StatementType::Call}
};

std::set<PKBField> getAll(DesignEntity type) {
    std::set<PKBField> result;
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


// replace int by PKBField
std::string processResult(std::set<PKBField> queryResult) {
    std::string stringResult = "";
    int count = 0;
    for (auto field : queryResult) {
        if (count == queryResult.size() - 1) {
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
    std::set<PKBField> queryResult;

    if (suchthat.empty() && pattern.empty()) {
        queryResult = getAll(returnType);
    }

    return processResult(queryResult);
}