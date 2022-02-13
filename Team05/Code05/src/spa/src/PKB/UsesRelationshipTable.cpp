#include "UsesRelationshipTable.h"

UsesRelationshipTable::UsesRelationshipTable() : RelationshipTable{ PKBRelationship::USES } {};

bool UsesRelationshipTable::contains(PKBField entity1, PKBField entity2) {
    bool checkConcreteEnt = entity1.fieldType == PKBFieldType::CONCRETE &&
        entity2.fieldType == PKBFieldType::CONCRETE;
    bool checkEntTypeMatch = entity2.entityType == PKBEntityType::VARIABLE &&
        entity1.entityType != PKBEntityType::VARIABLE &&
        entity1.entityType != PKBEntityType::CONST;

    if (!checkConcreteEnt || !checkEntTypeMatch) {
        throw "Both fields must be concrete and be of the correct types!";
    }

    return rows.count(RelationshipRow(entity1, entity2));
}

void UsesRelationshipTable::insert(PKBField entity1, PKBField entity2) {
    bool checkConcreteEnt = entity1.fieldType == PKBFieldType::CONCRETE &&
        entity2.fieldType == PKBFieldType::CONCRETE;
    bool checkEntTypeMatch = entity2.entityType == PKBEntityType::VARIABLE &&
        entity1.entityType != PKBEntityType::VARIABLE &&
        entity1.entityType != PKBEntityType::CONST;

    if (!checkConcreteEnt || !checkEntTypeMatch) {
        throw "Only concrete fields of the correct types can be inserted into the Uses table!";
    }

    rows.insert(RelationshipRow(entity1, entity2));
}

FieldRowResponse UsesRelationshipTable::retrieve(PKBField entity1, PKBField entity2) {
    bool checkEntTypeMatch = entity2.entityType == PKBEntityType::VARIABLE &&
        entity1.entityType != PKBEntityType::VARIABLE &&
        entity1.entityType != PKBEntityType::CONST;
    if (!checkEntTypeMatch) {
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
