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
void convertWildcardToDeclaration(PKBField* field) {
    if (field->fieldType == PKBFieldType::WILDCARD) {
        field->fieldType = PKBFieldType::DECLARATION;

        if (field->entityType == PKBEntityType::STATEMENT) {
            field->statementType = StatementType::All;
        }
    }
}

bool AffectsEvaluator::contains(PKBField field1, PKBField field2, bool isTransitive) {
    // Check if CFG initialized
    if (!isInit) {
        Logger(Level::ERROR) << "CFG has not been initialized!";
        return false;
    }
    
    // Validate fields
    if (!isContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "An Affects relationship is only between 2 concrete statements!";
        return false;
    }

    // Check if fields are assignments 
    if (!isAssignment(field1) || !isAssignment(field2)) {
        Logger(Level::ERROR) << "An Affects relationship is only between 2 assignment statements!";
        return false;
    }

    // If cache not populated compute and cache Affects
    if (!isCacheActive) {
        this->extractAndCacheAffects();
    }

    return isTransitive 
        ? affCache->containsT(field1, field2) 
        : affCache->contains(field1, field2);
}

FieldRowResponse AffectsEvaluator::retrieve(PKBField field1, PKBField field2, bool isTransitive) {
    FieldRowResponse res;
    
    // Check if CFG initialized
    if (!isInit) {
        Logger(Level::ERROR) << "CFG has not been initialized!";
        return res;
    }

    // Validate fields
    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) << "An Affects relationship is only between assignment statements!";
        return res;
    }

    convertWildcardToDeclaration(&field1);
    convertWildcardToDeclaration(&field2);

    // If cache not populated compute and cache Affects
    if (!isCacheActive) {
        this->extractAndCacheAffects();
    }

    return isTransitive
        ? affCache->retrieveT(field1, field2)
        : affCache->retrieve(field1, field2);
}

void AffectsEvaluator::initCFG(ProcToCfgMap cfgRoots) {
    this->roots = cfgRoots;
    this->isInit = true;
}

void AffectsEvaluator::clearCache() {
    this->affCache.reset();
    this->isCacheActive = false;
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

    // Check if fields are assignments
    bool isConcreteFirst = field1.fieldType == PKBFieldType::CONCRETE;
    bool isConcreteSec = field2.fieldType == PKBFieldType::CONCRETE;

    if (isConcreteFirst && !isAssignment(field1)) {
        return false;
    }

    if (isConcreteSec && !isAssignment(field2)) {
        return false;
    }
    return true;
}

void AffectsEvaluator::extractAndCacheAffects() {
    // Initialize cache
    this->affCache = std::make_unique<Graph<STMT_LO>>(PKBRelationship::AFFECTS);
    this->isCacheActive = true;

    // Initialize visited set for main traversal
    CfgNodeSet visited;

    // Traverse and extract relationships from each procedure's CFG
    for (auto varCfgPair : roots) {
        // Skip first dummy node
        auto root = varCfgPair.second;
        if (!root->stmt.has_value() && root->getChildren().size() != 0) {
            root = root->getChildren().at(0);
        }

        this->extractAndCacheFrom(root, &visited);
    }
}

void AffectsEvaluator::extractAndCacheFrom(NodePtr start, CfgNodeSet* visited) {
    auto curr = start.get();
    
    // Case where node has already been visited
    if (visited->find(curr) != visited->end()) {
        return;
    }
    
    // Deal with case where node is dummy
    if (!curr->stmt.has_value()) {
        // Get children
        std::vector<NodePtr> children = curr->getChildren();
        for (auto child : children) {
            this->extractAndCacheFrom(child, visited);
        }
    } else {
        // Add to visited list
        visited->insert(start.get());

        // Get statement at curr
        STMT_LO currStmt = curr->stmt.value();

        // Check type of stmt
        bool isAssignNode = currStmt.type.value() == StatementType::Assignment;

        // Do secondary traversal if node is assignment statement
        if (isAssignNode) {
            // Extract variable of interest (i.e. var being modified)
            auto voi = curr->modifies;

            // Get children
            std::vector<NodePtr> nextNodes = curr->getChildren();
            for (auto nextNode : nextNodes) {
                CfgNodeSet secVisited;
                this->walkAndExtract(nextNode, voi, start, &secVisited);
            }
        }

        // Continue traversal
        std::vector<NodePtr> children = curr->getChildren();
        for (auto child : children) {
            this->extractAndCacheFrom(child, visited);
        }
    }
}

void AffectsEvaluator::walkAndExtract(NodePtr curr, std::unordered_set<VAR_NAME> voi, 
    NodePtr src, CfgNodeSet* visited) {
    // Get children
    auto children = curr->getChildren();
    
    // Check if dummy node
    if (!curr->stmt.has_value()) {
        // If children exist walk them
        if (children.empty()) {
            return;
        }
        for (auto child : children) {
            this->walkAndExtract(child, voi, src, visited);
        }
        return;
    }

    // Check for affects relationship
    STMT_LO currStmt = curr->stmt.value();
    bool isAssignNode = currStmt.type.value() == StatementType::Assignment;
    bool hasRs = false;
    auto usedVars = curr->uses;
    for (auto var : voi) {
        if (usedVars.find(var) != usedVars.end()) {
            hasRs = true;
            break;
        }
    }
    if (hasRs && isAssignNode) {
        STMT_LO srcStmt = src->stmt.value();
        affCache->addEdge(srcStmt, currStmt);
    }

    // Check if node modifies voi
    if (!curr->modifies.empty()) {
        auto modVars = curr.get()->modifies;
        for (auto var : voi) {
            if (modVars.find(var) != modVars.end()) {
                visited->insert(curr.get());
                return;
            }
        }
    }

    // Check if node already visited
    if (visited->find(curr.get()) != visited->end()) {
        return;
    }

    // Update visited list and continue traversal
    visited->insert(curr.get());
    for (auto child : children) {
        this->walkAndExtract(child, voi, src, visited);
    }
}

bool AffectsEvaluator::isAssignment(PKBField field) const {
    return field.getContent<STMT_LO>()->type.value() == StatementType::Assignment;
}
