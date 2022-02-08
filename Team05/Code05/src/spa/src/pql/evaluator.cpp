#include "evaluator.h"

#define DEBUG Logger(Level::DEBUG) << "evaluator.cpp "

std::string Evaluator::PKBFieldToString(PKBField pkbField) {
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


PKBResponse Evaluator::getAll(DesignEntity type) {
    std::unordered_map<DesignEntity, StatementType> StatementTypeMap = {
            {DesignEntity::ASSIGN, StatementType::Assignment},
            {DesignEntity::WHILE, StatementType::While},
            {DesignEntity::IF, StatementType::If},
            {DesignEntity::READ, StatementType::Read},
            {DesignEntity::PRINT, StatementType::Print},
            {DesignEntity::CALL, StatementType::Call}
    };
    PKBResponse result;

    if (type == DesignEntity::PROCEDURE) {
        result = pkb->getProcedures();
    } else if (type == DesignEntity::CONSTANT) {
        result = pkb->getConstants();
    } else if (type == DesignEntity::VARIABLE) {
        result = pkb->getVariables();
    } else if (type == DesignEntity::STMT) {
        result = pkb->getStatements();
    } else {
        StatementType sType = StatementTypeMap.find(type)->second;
        result = pkb->getStatements(sType);
    }
    return result;
}

std::list<std::string> Evaluator::getListOfResult(PKBResponse queryResult) {
    std::list<std::string> listResult{};
    if(!queryResult.hasResult) {
        return listResult;
    }

    std::unordered_set<PKBField, PKBFieldHash> result = *(std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&queryResult.res));

    for (auto field : result) {
        listResult.push_back(PKBFieldToString(field));
    }

    return listResult;
}

std::list<std::string > Evaluator::evaluate(Query query) {
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

    return getListOfResult(queryResult);
}
