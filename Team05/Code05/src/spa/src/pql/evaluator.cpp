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


//classify the clauses into has Synonym and without Synonym
void Evaluator::processSuchthat(std::vector<std::shared_ptr<RelRef>> clauses, std::vector<std::shared_ptr<RelRef>>& noSyn, std::vector<std::shared_ptr<RelRef>>& hasSyn) {
    for (auto r : clauses) {
        RelRef* relRefPtr = r.get();
        if (relRefPtr->getType() == RelRefType::MODIFIESS) {
            Modifies* mPtr= dynamic_cast<Modifies*>(relRefPtr);
            EntRef modified = mPtr->modified;
            StmtRef stmt = mPtr->modifiesStmt;
            if ((modified.isVarName() || modified.isWildcard()) && (stmt.isLineNo() || stmt.isWildcard())) {
                noSyn.push_back(r);
            } else {
                hasSyn.push_back(r);
            }
        } else if (relRefPtr->getType() == RelRefType::USESS) {
            Uses* uPtr = dynamic_cast<Uses*>(relRefPtr);
            EntRef used = uPtr->used;
            StmtRef stmt = uPtr->useStmt;
            if ((used.isVarName() || used.isWildcard()) && (stmt.isLineNo() || stmt.isWildcard())) {
                noSyn.push_back(r);
            } else {
                hasSyn.push_back(r);
            }
        }
    }

}



std::list<std::string> Evaluator::getListOfResult(ResultTable& table, std::string variable) {
    std::list<std::string> listResult{};
    if(table.getResult().empty()) {
        return listResult;
    }
    int synPos = table.getSynLocation(variable);
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> resultTable = table.getResult();
    for (auto field : resultTable) {
        listResult.push_back(PKBFieldToString(field[synPos]));
    }

    return listResult;
}

std::list<std::string > Evaluator::evaluate(Query query) {
    // std::unordered_map<std::string, DesignEntity> declarations = query.getDeclarations();
    std::vector<std::string> variable = query.getVariable();
    std::vector<std::shared_ptr<RelRef>> suchthat = query.getSuchthat();
    std::vector<std::shared_ptr<RelRef>> noSyn;
    std::vector<std::shared_ptr<RelRef>> hasSyn;
    std::vector<Pattern> pattern = query.getPattern();

    DesignEntity returnType = query.getDeclarationDesignEntity(variable[0]);
    // TO DO: replace int with PKBField
    ResultTable resultTable;
    ResultTable& tableRef = resultTable;


    if (!suchthat.empty()) {
        processSuchthat(suchthat, noSyn, hasSyn);
        ClauseHandler handler = ClauseHandler(pkb, tableRef);
        if (!handler.evaluateNoSynClauses(noSyn)) return std::list<std::string>{};
        handler.handleSynClauses(hasSyn);
    }

    //after process suchthat and pattern if select variable not in result table, add all
    if (suchthat.empty() && pattern.empty() || !tableRef.synExists(variable[0])) {
        PKBResponse queryResult = getAll(returnType);
        std::vector<std::string> synonyms{variable[0]};
        tableRef.join(queryResult, synonyms);
    }

    return getListOfResult(tableRef, variable[0]);

}
