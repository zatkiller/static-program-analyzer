//#include "ParentRelationshipTable.h"
//
//ParentRelationshipTable::ParentRelationshipTable() : RelationshipTable{ PKBRelationship::PARENT } {};
//
//// ParentRelationshipTable::ParentRelationshipTable() : TransitiveRelationshipTable{ PKBRelationship::PARENT } {
////     parentGraph = std::make_unique<ParentGraph>();
//// };
//
//bool ParentRelationshipTable::isRetrieveValid(PKBField field1, PKBField entity2) {
//    return field1.entityType == PKBEntityType::STATEMENT &&
//        entity2.entityType == PKBEntityType::STATEMENT;
//}
//
//bool ParentRelationshipTable::contains(PKBField field1, PKBField entity2) {
//    if (!isInsertOrContainsValid(field1, entity2)) {
//        throw "Only concrete statements are allowed!";
//    } 
//    
//    return rows.count(RelationshipRow(field1, entity2)) == 1;
//}
//
//void ParentRelationshipTable::insert(PKBField field1, PKBField entity2) {
//    if (!isInsertOrContainsValid(field1, entity2)) {
//        throw "Only concrete statements can be inserted into the Parent table!";
//    }
//
//    rows.insert(RelationshipRow(field1, entity2));
//}
//
//FieldRowResponse ParentRelationshipTable::retrieve(PKBField field1, PKBField entity2) {
//    // Check both fields are statement types
//    if (!isRetrieveValid(field1, entity2)) {
//        throw "Parent relationship is only defined for statements!";
//    }
//
//    PKBFieldType fieldType1 = field1.fieldType;
//    PKBFieldType fieldType2 = entity2.fieldType;
//
//    STMT_LO* stmt1ptr = field1.getContent<STMT_LO>();
//    STMT_LO* stmt2ptr = entity2.getContent<STMT_LO>();
//
//    FieldRowResponse res;
//
//    if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
//        // Check both fields contain content
//        if (!stmt1ptr || !stmt2ptr) {
//            throw "Valid statements must be provided for a concrete field!";
//        }
//
//        if (this->contains(field1, entity2)) {
//            res.insert(std::vector<PKBField>{field1, entity2});
//        }
//
//    } else if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 != PKBFieldType::CONCRETE) {
//        // Check first field contains content
//        if (!stmt1ptr) {
//            throw "Valid statement must be provided for a concrete field!";
//        }
//
//        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
//            std::vector<PKBField> temp;
//
//            RelationshipRow curr = *iter;
//            PKBField rowFirst = curr.getFirst();
//            PKBField rowSecond = curr.getSecond();
//            StatementType declStmtType = entity2.statementType.value();
//            StatementType rowSecStmtType = rowSecond.getContent<STMT_LO>()->type.value_or(StatementType::All);
//
//            if (rowFirst == field1 && (declStmtType == StatementType::All || declStmtType == rowSecStmtType)) {
//                temp.push_back(rowFirst);
//                temp.push_back(rowSecond);
//                res.insert(temp);
//            }
//        }
//
//    } else if (fieldType1 != PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
//        // Check second field contains content
//        if (!stmt2ptr) {
//            throw "Valid statement must be provided for a concrete field!";
//        }
//
//        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
//            std::vector<PKBField> temp;
//
//            RelationshipRow curr = *iter;
//            PKBField rowFirst = curr.getFirst();
//            PKBField rowSecond = curr.getSecond();
//            StatementType declStmtType = field1.statementType.value();
//            StatementType rowFirstStmtType = rowFirst.getContent<STMT_LO>()->type.value_or(StatementType::All);
//
//            if ((declStmtType == StatementType::All || declStmtType == rowFirstStmtType) && rowSecond == entity2) {
//                temp.push_back(rowFirst);
//                temp.push_back(rowSecond);
//                res.insert(temp);
//            }
//        }
//
//    } else {
//        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
//            std::vector<PKBField> temp;
//
//            RelationshipRow curr = *iter;
//            PKBField rowFirst = curr.getFirst();
//            PKBField rowSecond = curr.getSecond();
//            StatementType entStmtType1 = field1.statementType.value();
//            StatementType entStmtType2 = entity2.statementType.value();
//            StatementType rowFirstType = rowFirst.getContent<STMT_LO>()->type.value_or(StatementType::All);
//            StatementType rowSecType = rowSecond.getContent<STMT_LO>()->type.value_or(StatementType::All);
//
//            bool entCond1 = (entStmtType1 == rowFirstType || entStmtType1 == StatementType::All);
//            bool entCond2 = (entStmtType2 == rowSecType || entStmtType2 == StatementType::All);
//
//            if (entCond1 && entCond2) {
//                temp.push_back(rowFirst);
//                temp.push_back(rowSecond);
//                res.insert(temp);
//            }
//        }
//    }
//
//    return res;
//}
//
//bool ParentRelationshipTable::containsT(PKBField field1, PKBField entity2) {
//    if (field1.entityType != PKBEntityType::STATEMENT || entity2.entityType != PKBEntityType::STATEMENT) {
//        throw "Input fields must be statements!";
//    }
//    if (field1.fieldType != PKBFieldType::CONCRETE || entity2.fieldType != PKBFieldType::CONCRETE) {
//        throw "Input fields must be concrete!";
//    }
//
//    // Base Cases
//    if (field1.getContent<STMT_LO>()->statementNum >= entity2.getContent<STMT_LO>()->statementNum) {
//        return false;
//    }
//    if (this->contains(field1, entity2)) {
//        return true;
//    }
//
//    // Recursive Step
//    PKBField declaration = PKBField::createDeclaration(StatementType::All);
//    FieldRowResponse res = this->retrieve(field1, declaration);
//    if (res.size() == 0) {
//        return false;
//    }
//
//    PKBField resEntity = (*res.begin()).at(1);
//
//    return this->containsT(resEntity, entity2);
//}
//
//// FieldRowResponse ParentRelationshipTable::retrieveT(PKBField field1, PKBField entity2) {
////
//// }
//
//bool ParentRelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
//    return field1.isValidConcrete(PKBEntityType::STATEMENT) && field2.isValidConcrete(PKBEntityType::STATEMENT);
//}
