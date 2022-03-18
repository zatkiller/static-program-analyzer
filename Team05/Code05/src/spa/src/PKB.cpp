#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "logging.h"
#include "PKB.h"

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

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
    nextTable = std::make_unique<NextRelationshipTable>();
}

// INSERT API
void PKB::insertEntity(Content entity) {
    std::visit(overloaded{
        [&](VAR_NAME& item) { insertVariable(item); },
        [&](STMT_LO& item) { insertStatement(item); },
        [&](PROC_NAME& item) { insertProcedure(item); },
        [&](CONST& item) { insertConstant(item); },
        [](auto& item) { Logger(Level::ERROR) << "PKB.cpp " << "Unsupported entity type"; }
    }, entity);
}

void PKB::insertStatement(STMT_LO stmt) {
    statementTable->insert(stmt);
}


void PKB::insertVariable(VAR_NAME var) {
    variableTable->insert(var);
}

void PKB::insertProcedure(PROC_NAME proc) {
    procedureTable->insert(proc);
}

void PKB::insertConstant(CONST constant) {
    constantTable->insert(constant);
}

void PKB::insertAST(std::unique_ptr<sp::ast::Program> root) {
    this->root = std::move(root);
}

bool PKB::validate(const PKBField field) const {
    switch (field.entityType) {
    case PKBEntityType::VARIABLE:
        return validateVariable(field);
    case PKBEntityType::PROCEDURE:
        return validateProcedure(field);
    case PKBEntityType::STATEMENT:
        return validateStatement(field);
    default:  // const
        return true;
    }
}

bool PKB::validateVariable(const PKBField field) const {
    if (field.fieldType == PKBFieldType::CONCRETE) {
        auto content = field.getContent<VAR_NAME>();
        auto varName = content->name;
        return variableTable->contains(varName);
    }

    return true;
}

bool PKB::validateProcedure(const PKBField field) const {
    if (field.fieldType == PKBFieldType::CONCRETE) {
        auto content = field.getContent<PROC_NAME>();
        auto varName = content->name;
        return procedureTable->contains(varName);
    }

    return true;
}

bool PKB::validateStatement(const PKBField field) const {
    if (field.fieldType == PKBFieldType::CONCRETE) {
        auto content = field.getContent<STMT_LO>();
        auto statementNum = content->statementNum;
        auto statementType = content->type;
        auto attribute = content->attribute;
        auto stmts = statementTable->getStmts(statementNum);

        if (stmts.size() != 1) {
            return false;
        }

        auto stmt = stmts.at(0);

        // If provided STMT_LO has a type, check if it is equivalent to the one in the table
        // StatementTypes of All or None will be updated to the correct one in the table by appendStatementInformation
        if (statementType.has_value() && statementType.value() != StatementType::All &&
            statementType.value() != StatementType::None) {
            return statementType == stmt.type;
        }
    }

    return true;
}

void PKB::appendStatementInformation(PKBField* field) {
    if (field->fieldType == PKBFieldType::CONCRETE && field->entityType == PKBEntityType::STATEMENT) {
        auto content = field->getContent<STMT_LO>();
        auto statementNum = content->statementNum;
        auto stmts = statementTable->getStmts(statementNum);
        auto stmt = stmts.at(0);
        field->content = stmt;
    }
}

void PKB::insertRelationship(PKBRelationship type, PKBField field1, PKBField field2) {
    // if both fields are not concrete, no insert can be done
    if (field1.fieldType != PKBFieldType::CONCRETE || field2.fieldType != PKBFieldType::CONCRETE) {
        Logger(Level::INFO) << "Both fields have to be concrete.\n";
        return;
    }

    if (!validate(field1) || !validate(field2)) {
        return;
    }

    appendStatementInformation(&field1);
    appendStatementInformation(&field2);

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
    case PKBRelationship::NEXT:
        nextTable->insert(field1, field2);
    default:
        Logger(Level::INFO) << "Inserted into an invalid relationship table\n";
        break;
    }
}

bool PKB::isRelationshipPresent(PKBField field1, PKBField field2, PKBRelationship rs) {
    if (field1.fieldType != PKBFieldType::CONCRETE || field2.fieldType != PKBFieldType::CONCRETE) {
        return false;
    }

    if (!validate(field1) || !validate(field2)) {
        return false;
    }

    appendStatementInformation(&field1);
    appendStatementInformation(&field2);

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
    case PKBRelationship::NEXT:
        return nextTable->contains(field1, field2);
    case PKBRelationship::NEXTT:
        return nextTable->containsT(field1, field2);
    default:
        Logger(Level::INFO) << "Checking for an invalid relationship table\n";
        return false;
    }
}

// GET API

PKBResponse PKB::getRelationship(PKBField field1, PKBField field2, PKBRelationship rs) {
    if (!validate(field1) || !validate(field2)) {
        return PKBResponse{ false, FieldRowResponse{} };
    }

    appendStatementInformation(&field1);
    appendStatementInformation(&field2);

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
    case PKBRelationship::NEXT:
        extracted = nextTable->retrieve(field1, field2);
        break;
    case PKBRelationship::NEXTT:
        extracted = nextTable->retrieveT(field1, field2);
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

    std::vector<STMT_LO> extracted = statementTable->getAllEntity();
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

    std::vector<VAR_NAME> extracted = variableTable->getAllEntity();
    for (auto row : extracted) {
        res.insert(PKBField::createConcrete(row));
    }

    return res.size() != 0 ? PKBResponse{ true, Response{res} } : PKBResponse{ false, Response{res} };
}

PKBResponse PKB::getProcedures() {
    std::unordered_set<PKBField, PKBFieldHash> res;

    std::vector<PROC_NAME> extracted = procedureTable->getAllEntity();
    for (auto row : extracted) {
        res.insert(PKBField::createConcrete(row));
    }

    return res.size() != 0 ? PKBResponse{ true, Response{res} } : PKBResponse{ false, Response{res} };
}

PKBResponse PKB::getConstants() {
    std::unordered_set<PKBField, PKBFieldHash> res;

    std::vector<CONST> extracted = constantTable->getAllEntity();
    for (auto row : extracted) {
        res.insert(PKBField::createConcrete(row));
    }

    return res.size() != 0 ? PKBResponse{ true, Response{res} } : PKBResponse{ false, Response{res} };
}

PKBResponse PKB::match(
    StatementType type,
    sp::design_extractor::PatternParam lhs,
    sp::design_extractor::PatternParam rhs,
    bool isStrict) {
    auto matchedStmts = sp::design_extractor::extractAssign(root.get(), lhs, rhs, isStrict);
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
