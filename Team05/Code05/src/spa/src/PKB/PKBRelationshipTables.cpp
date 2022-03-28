#include "PKBRelationshipTables.h"

/** ============================= RELATIONSHIPROW METHODS ============================== */

RelationshipRow::RelationshipRow(PKBField e1, PKBField e2) : field1(e1), field2(e2) {}

bool RelationshipRow::operator == (const RelationshipRow& row) const {
    return field1 == row.field1 && field2 == row.field2;
}

PKBField RelationshipRow::getFirst() const {
    return field1;
}

PKBField RelationshipRow::getSecond() const {
    return field2;
}

size_t RelationshipRowHash::operator() (const RelationshipRow& other) const {
    PKBField ent1 = other.getFirst();
    PKBField ent2 = other.getSecond();

    PKBEntityType entityType1 = ent1.entityType;
    PKBEntityType entityType2 = ent2.entityType;

    return (std::hash<PKBEntityType>()(entityType1) ^ std::hash<PKBEntityType>()(entityType2));
}

/** ============================ RELATIONSHIPTABLE METHODS ============================= */

RelationshipTable::RelationshipTable(PKBRelationship rsType) : type(rsType) {}

PKBRelationship RelationshipTable::getType() const {
    return type;
}

/** ===================== NONTRANSITIVERELATIONSHIPTABLE METHODS ======================= */

NonTransitiveRelationshipTable::NonTransitiveRelationshipTable(PKBRelationship rsType) : RelationshipTable(rsType) {}

bool NonTransitiveRelationshipTable::isRetrieveValid(PKBField field1, PKBField field2) const {
    // both Modifies and Uses cannot accept a wildcard as its first parameter
    if (field1.fieldType == PKBFieldType::WILDCARD) {
        return false;
    }

    if (field1.entityType != PKBEntityType::PROCEDURE && field1.entityType != PKBEntityType::STATEMENT) {
        return false;
    }

    if (field2.entityType != PKBEntityType::VARIABLE) {
        return false;
    }

    return true;
}

bool NonTransitiveRelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) const {
    if (!field1.isValidConcrete(PKBEntityType::STATEMENT) && !field1.isValidConcrete(PKBEntityType::PROCEDURE)) {
        return false;
    }

    if (!field2.isValidConcrete(PKBEntityType::VARIABLE)) {
        return false;
    }

    return true;
}

bool NonTransitiveRelationshipTable::contains(PKBField field1, PKBField field2) const {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "RelationshipTable can only contain concrete fields and STATEMENT or PROCEDURE entity types.";
        return false;
    }

    return rows.count(RelationshipRow(field1, field2)) == 1;
}

void NonTransitiveRelationshipTable::insert(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "RelationshipTable only allow inserts of concrete fields and STATEMENT or PROCEDURE entity types.";
        return;
    }

    rows.insert(RelationshipRow(field1, field2));
}

FieldRowResponse NonTransitiveRelationshipTable::retrieve(PKBField field1, PKBField field2) const {
    PKBFieldType fieldType1 = field1.fieldType;
    PKBFieldType fieldType2 = field2.fieldType;
    FieldRowResponse res;

    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "Only fields of STATEMENT or PROCEDURE entity types can be retrieved from RelationshipTable.";
        return res;
    }

    if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
        if (this->contains(field1, field2)) res.insert({ field1, field2 });

    } else {
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            auto isValidStatement = [&](auto field, auto first) {
                return field.statementType.value() == StatementType::All ||
                    field.statementType == (first.template getContent<STMT_LO>())->type.value();
            };

            if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 != PKBFieldType::CONCRETE) {
                if (first == field1) res.insert({ first, second });
            } else if (fieldType1 != PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
                if (field1.entityType == first.entityType && second == field2) {
                    if (field1.entityType == PKBEntityType::STATEMENT && !isValidStatement(field1, first)) continue;
                    res.insert({ first, second });
                }
            } else {
                // both fields are not concrete
                if (field1.entityType == first.entityType) {
                    if (field1.entityType == PKBEntityType::STATEMENT && !isValidStatement(field1, first)) continue;
                    res.insert({ first, second });
                }
            }
        }
    }

    return res;
}

int NonTransitiveRelationshipTable::getSize() const {
    return rows.size();
}

/** ======================== MODIFIESRELATIONSHIPTABLE METHODS ========================= */

ModifiesRelationshipTable::ModifiesRelationshipTable() : NonTransitiveRelationshipTable{ PKBRelationship::MODIFIES } {};

/** ========================== USESRELATIONSHIPTABLE METHODS =========================== */

UsesRelationshipTable::UsesRelationshipTable() : NonTransitiveRelationshipTable{ PKBRelationship::USES } {};

/** ======================== FOLLOWSRELATIONSHIPTABLE METHODS ========================== */

FollowsRelationshipTable::FollowsRelationshipTable() : 
    TransitiveRelationshipTable<STMT_LO>{ PKBRelationship::FOLLOWS } {}

/** ======================== PARENTRELATIONSHIPTABLE METHODS ========================== */

ParentRelationshipTable::ParentRelationshipTable() : TransitiveRelationshipTable<STMT_LO>{ PKBRelationship::PARENT } {}

/** ========================= CALLSRELATIONSHIPTABLE METHODS =========================== */

CallsRelationshipTable::CallsRelationshipTable() : TransitiveRelationshipTable<PROC_NAME>{ PKBRelationship::CALLS } {}

/** =========================NEXTRELATIONSHIPTABLE METHODS ============================== */

NextRelationshipTable::NextRelationshipTable() : TransitiveRelationshipTable<STMT_LO>{ PKBRelationship::NEXT } {}

/** ========================AFFECTSEVALUATOR METHODS ==================================== */

AffectsEvaluator::AffectsEvaluator(std::shared_ptr<NextRelationshipTable> nextTable,
    std::shared_ptr<ModifiesRelationshipTable> modifiesTable,
    std::shared_ptr<UsesRelationshipTable> usesTable) {
    this->nextTable = nextTable;
    this->modifiesTable = modifiesTable;
    this->usesTable = usesTable;
};

bool AffectsEvaluator::contains(PKBField field1, PKBField field2) const {
    // Validate fields
    if (!isContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "An Affects relationship is only between 2 concrete statements!";
        return false;
    }

    // Check if fields are assignments 
    if (field1.statementType.value() != StatementType::Assignment) {
        Logger(Level::ERROR) << "An Affects relationship is only between 2 assignment statements!";
        return false;
    }
    if (field2.statementType.value() != StatementType::Assignment) {
        Logger(Level::ERROR) << "An Affects relationship is only between 2 assignment statements!";
        return false;
    }

    // Check if variable modified by field1 is used by field2
    PKBField moddedVar;
    FieldRowResponse modVarRes = modifiesTable->retrieve(field1, PKBField::createDeclaration(PKBEntityType::VARIABLE));
    for (auto item : modVarRes) {
        moddedVar = item.back();
    }
    if (!usesTable->contains(field2, moddedVar)) {
        return false;
    }
}

bool AffectsEvaluator::containsT(PKBField field1, PKBField field2) const {

}

FieldRowResponse AffectsEvaluator::retrieve(PKBField field1, PKBField field2) const {

}

FieldRowResponse AffectsEvaluator::retrieveT(PKBField field1, PKBField field2) const {

}

bool AffectsEvaluator::isContainsValid(PKBField field1, PKBField field2) const {
    if (!field1.isValidConcrete(PKBEntityType::STATEMENT) || !field2.isValidConcrete(PKBEntityType::STATEMENT)) {
        return false;
    }

    return true;
}

bool AffectsEvaluator::isRetrieveValid(PKBField field1, PKBField field2) const {
    // Both fields must be statements
    if (field1.entityType != PKBEntityType::STATEMENT && field2.entityType != PKBEntityType::STATEMENT) {
        return false;
    }
    return true;
}
