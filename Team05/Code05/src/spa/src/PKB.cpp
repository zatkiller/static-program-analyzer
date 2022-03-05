#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "logging.h"
#include "PKB.h"
#include <DesignExtractor/EntityExtractor/VariableExtractor.h>

PKB::PKB() {
    statementTable = std::make_unique<StatementTable>();
    variableTable = std::make_unique<VariableTable>();
    constantTable = std::make_unique<ConstantTable>();
    procedureTable = std::make_unique<ProcedureTable>();
    modifiesTable = std::make_unique<ModifiesRelationshipTable>();
    followsTable = std::make_unique<FollowsRelationshipTable>();
    parentTable = std::make_unique<ParentRelationshipTable>();
    usesTable = std::make_unique<UsesRelationshipTable>();
    callsTable = std::make_unique<CallsRelationshipTable>();
}

// INSERT API

void PKB::insertStatement(StatementType type, int statementNumber) {
    statementTable->insert(type, statementNumber);
}

void PKB::insertVariable(std::string name) {
    variableTable->insert(name);
}

void PKB::insertProcedure(std::string name) {
    procedureTable->insert(name);
}

void PKB::insertConstant(int constant) {
    constantTable->insert(constant);
}

void PKB::insertAST(std::unique_ptr<sp::ast::Program> root) {
    this->root = std::move(root);
}

void PKB::insertRelationship(PKBRelationship type, PKBField field1, PKBField field2) {
    // if both fields are not concrete, no insert can be done
    if (field1.fieldType != PKBFieldType::CONCRETE || field2.fieldType != PKBFieldType::CONCRETE) {
        Logger(Level::INFO) << "Both fields have to be concrete.\n";
        return;
    }

    switch (type) {
    case PKBRelationship::MODIFIES:
        modifiesTable->insert(field1, field2);
        break;
    case PKBRelationship::FOLLOWS:
        followsTable->insert(field1, field2);
        break;
    case PKBRelationship::PARENT:
        parentTable->insert(field1, field2);
        break;
    case PKBRelationship::USES:
        usesTable->insert(field1, field2);
        break;
    case PKBRelationship::CALLS:
        callsTable->insert(field1, field2);
        break;
    default:
        Logger(Level::INFO) << "Inserted into an invalid relationship table\n";
        break;
    }
}

bool PKB::isRelationshipPresent(PKBField field1, PKBField field2, PKBRelationship rs) {
    if (field1.fieldType != PKBFieldType::CONCRETE || field2.fieldType != PKBFieldType::CONCRETE) {
        return false;
    }

    if (!getStatementTypeOfConcreteField(&field1) || !getStatementTypeOfConcreteField(&field2)) {
        return false;
    }

    switch (rs) {
    case PKBRelationship::MODIFIES:
        return modifiesTable->contains(field1, field2);
    case PKBRelationship::FOLLOWS:
        return followsTable->contains(field1, field2);
    case PKBRelationship::PARENT:
        return parentTable->contains(field1, field2);
    case PKBRelationship::USES:
        return usesTable->contains(field1, field2);
    case PKBRelationship::FOLLOWST:
        return followsTable->containsT(field1, field2);
    case PKBRelationship::PARENTT:
        return parentTable->containsT(field1, field2);
    case PKBRelationship::CALLS:
        return callsTable->contains(field1, field2);
    case PKBRelationship::CALLST:
        return callsTable->containsT(field1, field2);
    default:
        Logger(Level::INFO) << "Checking for an invalid relationship table\n";
        return false;
    }
}

// GET API

// if false, it means the statement number does not exist in the statement table
bool PKB::getStatementTypeOfConcreteField(PKBField* field) {
    if (field->fieldType == PKBFieldType::CONCRETE && field->entityType == PKBEntityType::STATEMENT) {
        auto content = field->getContent<STMT_LO>();

        if (!content->hasStatementType() || content->type.value() == StatementType::All) {
            auto type = statementTable->getStmtTypeOfLine(content->statementNum);

            if (type.has_value()) {
                field->content = STMT_LO{ content->statementNum, type.value() };
                return true;
            } else {
                return false;
            }
        }
    }

    return true;
}

PKBResponse PKB::getRelationship(PKBField field1, PKBField field2, PKBRelationship rs) {
    if (!getStatementTypeOfConcreteField(&field1) || !getStatementTypeOfConcreteField(&field2)) {
        return PKBResponse{ false, FieldRowResponse{} };
    }

    FieldRowResponse extracted;

    switch (rs) {
    case PKBRelationship::MODIFIES:
        extracted = modifiesTable->retrieve(field1, field2);
        break;
    case PKBRelationship::FOLLOWS:
        extracted = followsTable->retrieve(field1, field2);
        break;
    case PKBRelationship::PARENT:
        extracted = parentTable->retrieve(field1, field2);
        break;
    case PKBRelationship::USES:
        extracted = usesTable->retrieve(field1, field2);
        break;
    case PKBRelationship::FOLLOWST:
        extracted = followsTable->retrieveT(field1, field2);
        break;
    case PKBRelationship::PARENTT:
        extracted = parentTable->retrieveT(field1, field2);
        break;
    case PKBRelationship::CALLS:
        extracted = callsTable->retrieve(field1, field2);
        break;
    case PKBRelationship::CALLST:
        extracted = callsTable->retrieveT(field1, field2);
        break;
    default:
        throw "Invalid relationship type used!";
        break;
    }

    return extracted.size() != 0
        ? PKBResponse{ true, Response{extracted} }
    : PKBResponse{ false, Response{extracted} };
}

PKBResponse PKB::getStatements() {
    std::unordered_set<PKBField, PKBFieldHash> res;

    std::vector<STMT_LO> extracted = statementTable->getAllStmt();
    for (auto row : extracted) {
        res.insert(PKBField::createConcrete(row));
    }

    return res.size() != 0 ? PKBResponse{ true, Response{res} } : PKBResponse{ false, Response{res} };
}

PKBResponse PKB::getStatements(StatementType stmtType) {
    std::unordered_set<PKBField, PKBFieldHash> res;

    std::vector<STMT_LO> extracted = statementTable->getStmtOfType(stmtType);
    for (auto row : extracted) {
        res.insert(PKBField::createConcrete(row));
    }

    return res.size() != 0 ? PKBResponse{ true, Response{res} } : PKBResponse{ false, Response{res} };
}

PKBResponse PKB::getVariables() {
    std::unordered_set<PKBField, PKBFieldHash> res;

    std::vector<VAR_NAME> extracted = variableTable->getAllVars();
    for (auto row : extracted) {
        res.insert(PKBField::createConcrete(row));
    }

    return res.size() != 0 ? PKBResponse{ true, Response{res} } : PKBResponse{ false, Response{res} };
}

PKBResponse PKB::getProcedures() {
    std::unordered_set<PKBField, PKBFieldHash> res;

    std::vector<PROC_NAME> extracted = procedureTable->getAllProcs();
    for (auto row : extracted) {
        res.insert(PKBField::createConcrete(row));
    }

    return res.size() != 0 ? PKBResponse{ true, Response{res} } : PKBResponse{ false, Response{res} };
}

PKBResponse PKB::getConstants() {
    std::unordered_set<PKBField, PKBFieldHash> res;

    std::vector<CONST> extracted = constantTable->getAllConst();
    for (auto row : extracted) {
        res.insert(PKBField::createConcrete(row));
    }

    return res.size() != 0 ? PKBResponse{ true, Response{res} } : PKBResponse{ false, Response{res} };
}

PKBResponse PKB::match(
    StatementType type, 
    sp::design_extractor::PatternParam lhs, 
    sp::design_extractor::PatternParam rhs) {
    auto matchedStmts = sp::design_extractor::extractAssign(root.get(), lhs, rhs);
    FieldRowResponse res;

    if (matchedStmts.size() == 0) {
        return PKBResponse{ false, Response{res} };
    }

    for (auto& node : matchedStmts) {
        std::vector<PKBField> stmtRes;

        int statementNumber = node.get().getStmtNo();
        auto stmt = PKBField::createConcrete(
            STMT_LO{ statementNumber, statementTable->getStmtTypeOfLine(statementNumber).value() });
        stmtRes.emplace_back(stmt);

        auto varname = node.get().getLHS()->getVarName();
        stmtRes.emplace_back(PKBField::createConcrete(VAR_NAME{ varname }));

        res.insert(stmtRes);
    }

    return PKBResponse{ true, Response{res} };
}
