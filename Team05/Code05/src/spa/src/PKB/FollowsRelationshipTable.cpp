#include <unordered_set>
#include <vector>

#include "FollowsRelationshipTable.h"

FollowsRelationshipTable::FollowsRelationshipTable() : RelationshipTable{ PKBRelationship::FOLLOWS } {
    followsGraph = std::make_unique<FollowsGraph>();
};

bool FollowsRelationshipTable::contains(PKBField entity1, PKBField entity2) {
    return rows.count(RelationshipRow(entity1, entity2)) == 1;
}

void FollowsRelationshipTable::insert(PKBField entity1, PKBField entity2) {
    bool checkEntTypeMatch = entity1.entityType == PKBEntityType::STATEMENT &&
        entity2.entityType == entity1.entityType;
    bool checkConcreteEnt = entity1.fieldType == PKBFieldType::CONCRETE &&
        entity2.fieldType == entity1.fieldType;

    if (checkEntTypeMatch && checkConcreteEnt) {
        rows.insert(RelationshipRow(entity1, entity2));
        followsGraph->addEdge(*entity1.getContent<STMT_LO>(), *entity2.getContent<STMT_LO>());
    } else {
        throw "Only concrete statements can be inserted into the Follows table!";
    }
}

FieldRowResponse FollowsRelationshipTable::retrieve(PKBField entity1, PKBField entity2) {
    // Both fields have to be a statement type
    if (entity1.entityType != PKBEntityType::STATEMENT || entity2.entityType != PKBEntityType::STATEMENT) {
        throw "Follows relationship is only defined for statements!";
    }

    PKBFieldType fieldType1 = entity1.fieldType;
    PKBFieldType fieldType2 = entity2.fieldType;

    STMT_LO* stmt1ptr = entity1.getContent<STMT_LO>();
    STMT_LO* stmt2ptr = entity2.getContent<STMT_LO>();

    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> res;

    if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
        if (!stmt1ptr || !stmt2ptr) {
            throw "Valid statement must be provided for a concrete field!";
        }
        /*for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow curr = *iter;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            if (first == entity1 && second == entity2) {
                temp.push_back(first);
                temp.push_back(second);
                res.insert(temp);
            }
        }*/
        if (this->contains(entity1, entity2)) {
            res.insert(std::vector<PKBField>{entity1, entity2});
        }
    } else if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 != PKBFieldType::CONCRETE) {
        if (!stmt1ptr) {
            throw "Valid statement must be provided for a concrete field!";
        }
        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow curr = *iter;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();
            StatementType secStmtType = second.getContent<STMT_LO>()->type.value_or(StatementType::All);
            StatementType entStmtType = entity2.statementType.value();

            if (first == entity1 && (entStmtType == StatementType::All || secStmtType == entStmtType)) {
                temp.push_back(first);
                temp.push_back(second);
                res.insert(temp);
            }
        }
    } else if (fieldType1 != PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
        if (!stmt2ptr) {
            throw "Valid statement must be provided for a concrete field!";
        }
        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow curr = *iter;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();
            StatementType firstStmtType = first.getContent<STMT_LO>()->type.value_or(StatementType::All);
            StatementType entStmtType = entity1.statementType.value();

            if (second == entity2 && (entStmtType == StatementType::All || firstStmtType == entStmtType)) {
                temp.push_back(first);
                temp.push_back(second);
                res.insert(temp);
            }
        }
    } else {
        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow curr = *iter;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            STMT_LO* firstStmt = first.getContent<STMT_LO>();
            STMT_LO* secStmt = second.getContent<STMT_LO>();
            StatementType firstStmtType = firstStmt->type.value_or(StatementType::All);
            StatementType secStmtType = secStmt->type.value_or(StatementType::All);
            StatementType entType1 = entity1.statementType.value();
            StatementType entType2 = entity2.statementType.value();

            bool entCond1 = (entType1 == firstStmtType || entType1 == StatementType::All);
            bool entCond2 = (entType2 == secStmtType || entType2 == StatementType::All);

            if (entCond1 && entCond2) {
                temp.push_back(first);
                temp.push_back(second);
                res.insert(temp);
            }
        }
    }

    return res;
}

bool FollowsRelationshipTable::containsT(PKBField entity1, PKBField entity2) {
    if (entity1.entityType != PKBEntityType::STATEMENT || entity2.entityType != PKBEntityType::STATEMENT) {
        throw "Input fields must be statements!";
    }
    if (entity1.fieldType != PKBFieldType::CONCRETE || entity2.fieldType != PKBFieldType::CONCRETE) {
        throw "Input fields must be concrete!";
    }

    // Base Cases
    if (entity1.getContent<STMT_LO>()->statementNum >= entity2.getContent<STMT_LO>()->statementNum) {
        return false;
    }
    if (this->contains(entity1, entity2)) {
        return true;
    }

    // Recursive Step
    PKBField declaration = PKBField::createDeclaration(StatementType::All);
    FieldRowResponse res = this->retrieve(entity1, declaration);
    if (res.size() == 0) {
        return false;
    }

    PKBField resEntity = (*res.begin()).at(1);

    return this->containsT(resEntity, entity2);
}

// TODO(Patrick): Use graphs to implement retrieveT
// FieldRowResponse FollowsRelationshipTable::retrieveT(PKBField entity1, PKBField entity2) {
//    
//}
