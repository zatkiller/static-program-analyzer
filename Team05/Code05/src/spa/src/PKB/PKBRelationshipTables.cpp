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

PKBRelationship RelationshipTable::getType() {
    return type;
}

/** ===================== NONTRANSITIVERELATIONSHIPTABLE METHODS ======================= */

NonTransitiveRelationshipTable::NonTransitiveRelationshipTable(PKBRelationship rsType) : RelationshipTable(rsType) {}

bool NonTransitiveRelationshipTable::isRetrieveValid(PKBField field1, PKBField field2) {
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

bool NonTransitiveRelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
    if (!field1.isValidConcrete(PKBEntityType::STATEMENT) && !field1.isValidConcrete(PKBEntityType::PROCEDURE)) {
        return false;
    }

    if (!field2.isValidConcrete(PKBEntityType::VARIABLE)) {
        return false;
    }

    return true;
}

bool NonTransitiveRelationshipTable::contains(PKBField field1, PKBField field2) {
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

FieldRowResponse NonTransitiveRelationshipTable::retrieve(PKBField field1, PKBField field2) {
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

    } else if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 != PKBFieldType::CONCRETE) {
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            if (first == field1) {
                res.insert({ first, second });
            }
        }
        // either statement declaration or wildcard, or procedure wildcard/declaration
    } else if (fieldType1 != PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
        // field1 cannot be both statement and wildcard, invalid syntax
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            // If first declaration looking for Procedures
            if (field1.entityType == PKBEntityType::PROCEDURE && first.entityType == PKBEntityType::PROCEDURE) {
                if (second == field2) {
                    res.insert({ first, second });
                }
            }

            if (field1.entityType == PKBEntityType::STATEMENT && first.entityType == PKBEntityType::STATEMENT) {
                if (second == field2) {
                    if (field1.statementType.value() == StatementType::All ||
                        field1.statementType == first.getContent<STMT_LO>()->type.value()) {
                        res.insert({ first, second });
                    }
                }
            }
        }
    } else {  // both declaration/wild
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            // If first declaration looking for Procedures
            if (field1.entityType == PKBEntityType::PROCEDURE && first.entityType == PKBEntityType::PROCEDURE) {
                res.insert({ first, second });
            }

            if (field1.entityType == PKBEntityType::STATEMENT && first.entityType == PKBEntityType::STATEMENT) {
                if (field1.statementType.value() == StatementType::All ||
                    field1.statementType == first.getContent<STMT_LO>()->type.value()) {
                    res.insert({ first, second });
                }
            }
        }
    }

    return res;
}

int NonTransitiveRelationshipTable::getSize() {
    return rows.size();
}

/** ================================== GRAPH METHODS =================================== */
Graph::Graph(PKBRelationship type) : type(type) {}

Node* Graph::addNode(std::map<STMT_LO, Node*>& filtered, STMT_LO stmt) {
    bool hasExistingStatementNo = false;
    for (auto [s, node] : filtered) {
        // Case where a node for u already exists
        if (stmt == s) {
            return nodes.at(stmt);
        }

        // Invalid insert
        if (s.statementNum == stmt.statementNum && s.type.value() != stmt.type.value()) {
            return nullptr;
        }
    }

    if (!hasExistingStatementNo) {
        Node* node = new Node(stmt, nullptr, Node::NodeSet{});
        nodes.emplace(stmt, node);
        return node;
    }

    return nullptr;
}

void Graph::addEdge(STMT_LO u, STMT_LO v) {
    // Second statement in the relationship cannot come before the first
    if (u.statementNum >= v.statementNum) {
        return;
    }

    // First statement in a Parent relationship has to be a container statement 
    if (type == PKBRelationship::PARENT && u.type.value() != StatementType::If
        && u.type.value() != StatementType::While) {
        return;
    }

    // Filter nodes whose statement numbers are the same as inputs
    std::map<STMT_LO, Node*> filtered;
    std::copy_if(nodes.begin(), nodes.end(), std::inserter(filtered, filtered.end()),
        [u, v](decltype(nodes)::value_type const& pair) {
            return pair.first.statementNum == u.statementNum ||
                pair.first.statementNum == v.statementNum;
        });

    Node* uNode = addNode(filtered, u);
    Node* vNode = addNode(filtered, v);

    if (!uNode || !vNode) {
        return;
    }

    // Add the edge between uNode and vNode
    uNode->next.push_back(vNode);
    if (!vNode->prev) {
        vNode->prev = uNode;
    }
}

bool Graph::getContains(PKBField field1, PKBField field2) {
    STMT_LO parent = *field1.getContent<STMT_LO>();
    STMT_LO child = *field2.getContent<STMT_LO>();

    if (nodes.count(parent) != 0) {
        Node* curr = nodes.at(parent);
        Node::NodeSet nextNodes = curr->next;
        Node::NodeSet filtered;
        std::copy_if(nextNodes.begin(), nextNodes.end(),
            std::back_inserter(filtered), [child](Node* const& node) {
                return node->stmt == child;
            });
        return (filtered.size() == 1);
    }
    return false;
}

bool Graph::getContainsT(PKBField field1, PKBField field2) {
    STMT_LO parent = *field1.getContent<STMT_LO>();
    STMT_LO child = *field2.getContent<STMT_LO>();

    // Base Case where Parent(field1, field2) holds
    if (this->getContains(field1, field2)) {
        return true;
    }

    if (nodes.count(parent) != 0) {
        Node* curr = nodes.at(parent);
        Node::NodeSet nextNodes = curr->next;

        // Recursive lookup for each node in the NodeSet
        for (auto node : nextNodes) {
            STMT_LO newStmt = node->stmt;
            PKBField newField1 = PKBField::createConcrete(Content{ newStmt });
            if (this->getContainsT(newField1, field2)) {
                return true;
            }
        }
    }
    return false;
}

Result Graph::retrieve(PKBField field1, PKBField field2) {
    bool isConcreteFirst = field1.fieldType == PKBFieldType::CONCRETE;
    bool isConcreteSec = field2.fieldType == PKBFieldType::CONCRETE;

    if (isConcreteFirst && isConcreteSec) {
        return getContains(field1, field2) ? Result{ {{field1, field2}} } : Result{};
    } else if (isConcreteFirst && !isConcreteSec) {
        return traverseStart(field1, field2);
    } else if (!isConcreteFirst && isConcreteSec) {
        return traverseEnd(field1, field2);
    } else {
        return traverseAll(field1.statementType.value(), field2.statementType.value());
    }
}

Result Graph::retrieveT(PKBField field1, PKBField field2) {
    bool isConcreteFirst = field1.fieldType == PKBFieldType::CONCRETE;
    bool isDeclarationFirst = field1.fieldType == PKBFieldType::DECLARATION;
    bool isConcreteSec = field2.fieldType == PKBFieldType::CONCRETE;
    bool isDeclarationSec = field2.fieldType == PKBFieldType::DECLARATION;

    if (isConcreteFirst && isDeclarationSec) {
        return traverseStartT(field1, field2);
    } else if (isDeclarationFirst && isConcreteSec) {
        return traverseEndT(field1, field2);
    } else if (isDeclarationFirst && isDeclarationSec) {
        return traverseAllT(field1.statementType.value(), field2.statementType.value());
    } else {
        return Result{};
    }
}

Result Graph::traverseStart(PKBField field1, PKBField field2) {
    STMT_LO target = *(field1.getContent<STMT_LO>());
    Result res{};

    if (!target.type.has_value()) {
        return res;
    }

    StatementType targetType = field2.statementType.value();

    if (nodes.count(target) != 0) {
        Node* curr = nodes.at(target);

        if (!curr->next.empty()) {
            Node::NodeSet nextNodes = curr->next;
            Node::NodeSet filtered;
            std::copy_if(nextNodes.begin(), nextNodes.end(), std::back_inserter(filtered),
                [targetType](Node* const& node) {
                    return (node->stmt.type.value() == targetType || targetType == StatementType::All);
                });

            for (Node* node : filtered) {
                std::vector<PKBField> temp;
                PKBField second = PKBField::createConcrete(Content{ node->stmt });
                temp.push_back(field1);
                temp.push_back(second);
                res.insert(temp);
            }
        }
    }
    return res;
}

Result Graph::traverseStartT(PKBField field1, PKBField field2) {
    std::set<STMT_LO> found;
    Result res{};
    StatementType targetType = field2.statementType.value();
    STMT_LO startStmt = *(field1.getContent<STMT_LO>());

    if (nodes.count(startStmt) != 0) {
        traverseStartT(&found, nodes.at(startStmt), targetType);
    }

    if (!found.empty()) {
        std::transform(found.begin(), found.end(), std::insert_iterator<Result>(res, res.end()),
            [field1](STMT_LO const& stmt) {
                PKBField second = PKBField::createConcrete(Content{ stmt });
                return std::vector<PKBField>{field1, second};
            });
    }

    return res;
}

void Graph::traverseStartT(std::set<STMT_LO>* found, Node* node, StatementType targetType) {
    Node::NodeSet nextNodes = node->next;

    if (!nextNodes.empty()) {
        for (auto nextNode : nextNodes) {
            bool typeMatch = nextNode->stmt.type.value() == targetType || targetType == StatementType::All;
            if (typeMatch) {
                found->insert(nextNode->stmt);
            }
            traverseStartT(found, nextNode, targetType);
        }
    }
    return;
}

Result Graph::traverseEnd(PKBField field1, PKBField field2) {
    STMT_LO target = *(field2.getContent<STMT_LO>());
    Result res{};

    if (!target.type.has_value()) {
        return res;
    }

    StatementType targetType = field1.statementType.value();

    if (nodes.count(target) != 0) {
        Node* curr = nodes.at(target);

        if (curr->prev != nullptr) {
            STMT_LO prevStmt = curr->prev->stmt;
            if (prevStmt.type.value() == targetType || targetType == StatementType::All) {
                PKBField first = PKBField::createConcrete(Content{ prevStmt });
                res.insert(std::vector<PKBField>{first, field2});
            }
        }
    }
    return res;
}

Result Graph::traverseEndT(PKBField field1, PKBField field2) {
    std::set<STMT_LO> found;
    Result res{};
    StatementType targetType = field1.statementType.value();
    STMT_LO startStmt = *(field2.getContent<STMT_LO>());

    if (nodes.count(startStmt) != 0) {
        traverseEndT(&found, nodes.at(startStmt), targetType);
    }

    if (!found.empty()) {
        std::transform(found.begin(), found.end(), std::insert_iterator<Result>(res, res.end()),
            [field2](STMT_LO const& stmt) {
                PKBField first = PKBField::createConcrete(Content{ stmt });
                return std::vector<PKBField>{first, field2};
            });
    }

    return res;
}

void Graph::traverseEndT(std::set<STMT_LO>* found, Node* node, StatementType targetType) {
    while (node->prev) {
        bool typeMatch = node->prev->stmt.type.value() == targetType || targetType == StatementType::All;
        if (typeMatch) {
            found->insert(node->prev->stmt);
        }
        node = node->prev;
    }
}

Result Graph::traverseAllT(StatementType type1, StatementType type2) {
    std::set<STMT_LO> found;
    Result res;

    for (auto const& [stmt_lo, node] : nodes) {
        Node* curr = node;
        found.clear();

        bool typeMatch = type1 == StatementType::All || node->stmt.type.value() == type1;
        if (typeMatch) {
            traverseStartT(&found, node, type2);
        }

        std::transform(found.begin(), found.end(), std::insert_iterator<Result>(res, res.end()),
            [curr](STMT_LO const& stmt) {
                PKBField first = PKBField::createConcrete(Content{ curr->stmt });
                PKBField second = PKBField::createConcrete(Content{ stmt });
                return std::vector<PKBField>{first, second};
            });
    }

    return res;
}

Result Graph::traverseAll(StatementType type1, StatementType type2) {
    Result res{};
    for (auto const& [stmt_lo, node] : nodes) {
        Node* curr = node;
        bool typeMatch1 = curr->stmt.type.value() == type1 || type1 == StatementType::All;

        if (!curr->next.empty() && typeMatch1) {
            Node::NodeSet nextNodes = curr->next;

            // Filter nodes that match second statement type
            Node::NodeSet filtered;
            std::copy_if(nextNodes.begin(), nextNodes.end(), std::back_inserter(filtered),
                [type2](Node* const& node) {
                    return node->stmt.type.value() == type2 || type2 == StatementType::All;
                });

            // Populate result
            std::transform(filtered.begin(), filtered.end(), std::insert_iterator<Result>(res, res.end()),
                [curr](Node* const& node) {
                    PKBField first = PKBField::createConcrete(Content{ curr->stmt });
                    PKBField second = PKBField::createConcrete(Content{ node->stmt });
                    return std::vector<PKBField>{first, second};
                });
        }
    }
    return res;
}

int Graph::getSize() {
    return nodes.size();
}

/** ======================= TRANSITIVERELATIONSHIPTABLE METHODS ======================== */

void TransitiveRelationshipTable::insert(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "Only concrete statements can be inserted into the Follows table!";
        return;
    }

    graph->addEdge(*field1.getContent<STMT_LO>(), *field2.getContent<STMT_LO>());
}

TransitiveRelationshipTable::TransitiveRelationshipTable(PKBRelationship type) : RelationshipTable(type),
graph(std::make_unique<Graph>(type)) {
}

bool TransitiveRelationshipTable::contains(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "Only concrete statements can be inserted into the Follows table!";
        return false;
    }

    return graph->getContains(field1, field2);
}

bool TransitiveRelationshipTable::containsT(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "FollowsRelationshipTable can only contain concrete fields and STATEMENT entity types.";
        return false;
    }

    return graph->getContainsT(field1, field2);
}

void convertWildcardToDeclaration(PKBField* field) {
    if (field->fieldType == PKBFieldType::WILDCARD) {
        field->fieldType = PKBFieldType::DECLARATION;
        field->statementType = StatementType::All;
    }
}

FieldRowResponse TransitiveRelationshipTable::retrieve(PKBField field1, PKBField field2) {
    // Both fields have to be a statement type
    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "Only STATEMENT entity types can be retrieved from FollowsRelationshipTable.";
        return FieldRowResponse{ };
    }

    // for any fields that are wildcards, convert them into declarations of all types
    convertWildcardToDeclaration(&field1);
    convertWildcardToDeclaration(&field2);

    if (field1.fieldType == PKBFieldType::CONCRETE &&
        field2.fieldType == PKBFieldType::CONCRETE) {
        return this->contains(field1, field2)
            ? FieldRowResponse{ {{field1, field2}} }
        : FieldRowResponse{};
    }

    return graph->retrieve(field1, field2);
}

FieldRowResponse TransitiveRelationshipTable::retrieveT(PKBField field1, PKBField field2) {
    // Both fields have to be a statement type
    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "Only STATEMENT entity types can be retrieved from FollowsRelationshipTable.";
        return FieldRowResponse{ };
    }

    // for any fields that are wildcards, convert them into declarations of all types
    convertWildcardToDeclaration(&field1);
    convertWildcardToDeclaration(&field2);

    if (field1.fieldType == PKBFieldType::CONCRETE &&
        field2.fieldType == PKBFieldType::CONCRETE) {
        return this->containsT(field1, field2)
            ? FieldRowResponse{ {{field1, field2}} }
        : FieldRowResponse{};
    }

    return graph->retrieveT(field1, field2);
}

bool TransitiveRelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
    return field1.isValidConcrete(PKBEntityType::STATEMENT) && field2.isValidConcrete(PKBEntityType::STATEMENT);
}

bool TransitiveRelationshipTable::isRetrieveValid(PKBField field1, PKBField field2) {
    return field1.entityType == PKBEntityType::STATEMENT &&
        field2.entityType == PKBEntityType::STATEMENT;
}

int TransitiveRelationshipTable::getSize() {
    return graph->getSize();
}

/** ======================== MODIFIESRELATIONSHIPTABLE METHODS ========================= */

ModifiesRelationshipTable::ModifiesRelationshipTable() : NonTransitiveRelationshipTable{ PKBRelationship::MODIFIES } {};

/** ========================== USESRELATIONSHIPTABLE METHODS =========================== */

UsesRelationshipTable::UsesRelationshipTable() : NonTransitiveRelationshipTable{ PKBRelationship::USES } {};

/** ======================== FOLLOWSRELATIONSHIPTABLE METHODS ========================== */

FollowsRelationshipTable::FollowsRelationshipTable() : TransitiveRelationshipTable(PKBRelationship::FOLLOWS) {}

/** ======================== PARENTRELATIONSHIPTABLE METHODS ========================== */

ParentRelationshipTable::ParentRelationshipTable() : TransitiveRelationshipTable(PKBRelationship::PARENT) {}
