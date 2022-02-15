#include "UsesRelationshipTable.h"

UsesRelationshipTable::UsesRelationshipTable() : RelationshipTable{ PKBRelationship::USES } {};

bool UsesRelationshipTable::isRetrieveValid(PKBField entity1, PKBField entity2) {
    return (entity1.entityType == PKBEntityType::PROCEDURE ||
        entity1.entityType == PKBEntityType::STATEMENT) &&
        (entity2.entityType == PKBEntityType::VARIABLE);
}

bool UsesRelationshipTable::contains(PKBField entity1, PKBField entity2) {
    if (!isInsertOrContainsValid(entity1, entity2)) {
        throw "Both fields must be concrete and be of the correct types!";
    }

    return rows.count(RelationshipRow(entity1, entity2));
}

void UsesRelationshipTable::insert(PKBField entity1, PKBField entity2) {
    if (!isInsertOrContainsValid(entity1, entity2)) {
        throw "Only concrete fields of the correct types can be inserted into the Uses table!";
    }

    rows.insert(RelationshipRow(entity1, entity2));
}

FieldRowResponse UsesRelationshipTable::retrieve(PKBField entity1, PKBField entity2) {
    if (!isRetrieveValid(entity1, entity2)) {
        throw "First field in Uses must either be a statement or a procedure, and the second must be a variable!";
    }
    
    bool isConcreteFirst = entity1.fieldType == PKBFieldType::CONCRETE;
    bool isConcreteSec = entity2.fieldType == PKBFieldType::CONCRETE;
    bool isProcedureFirst = (entity1.entityType == PKBEntityType::PROCEDURE);

    FieldRowResponse res;

    if (!isConcreteFirst && !isConcreteSec) {
        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow curr = *iter;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            // If first declaration is looking for Procedures
            if (isProcedureFirst) {
                if (first.entityType == PKBEntityType::PROCEDURE) {
                    temp.push_back(first);
                    temp.push_back(second);
                    res.insert(temp);
                }
                continue;
            }

            StatementType firstStmtType = first.getContent<STMT_LO>()->type.value_or(StatementType::All);
            StatementType requiredStmtType = entity1.statementType.value();
            if (firstStmtType == requiredStmtType) {
                temp.push_back(first);
                temp.push_back(second);
                res.insert(temp);
            }
        }

    } else if (isConcreteFirst && !isConcreteSec) {
        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow curr = *iter;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();
            bool isVarSec = (second.entityType == PKBEntityType::VARIABLE);

            if (first == entity1 && isVarSec) {
                temp.push_back(first);
                temp.push_back(second);
                res.insert(temp);
            }
        }

    } else if (!isConcreteFirst && isConcreteSec) {
        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow curr = *iter;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            // If first declaration looking for Procedures
            if (isProcedureFirst) {
                if (first.entityType == PKBEntityType::PROCEDURE && second == entity2) {
                    temp.push_back(first);
                    temp.push_back(second);
                    res.insert(temp);
                }
                continue;
            }

            StatementType firstStmtType = first.getContent<STMT_LO>()->type.value_or(StatementType::All);
            StatementType requiredStmtType = entity1.statementType.value();
            if (firstStmtType == requiredStmtType && second == entity2) {
                temp.push_back(first);
                temp.push_back(second);
                res.insert(temp);
            }
        }

    } else {
        if (this->contains(entity1, entity2)) {
            res.insert(std::vector<PKBField>{entity1, entity2});
        }
    }
}

bool UsesRelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
    return (field1.isValidConcrete(PKBEntityType::STATEMENT) || field1.isValidConcrete(PKBEntityType::PROCEDURE))
        && field2.isValidConcrete(PKBEntityType::VARIABLE);
}
