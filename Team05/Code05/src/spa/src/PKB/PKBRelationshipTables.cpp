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

int NonTransitiveRelationshipTable::getSize() {
    return rows.size();
}

/** ================================== GRAPH METHODS =================================== */
template <typename T>
Graph<T>::Graph(PKBRelationship type) : type(type) {}

template <typename T>
Node<T>* Graph<T>::createNode(T val) {
    // Filter nodes whose statement numbers are the same as inputs
    std::map<T, Node<T>*> filtered;
    std::copy_if(nodes.begin(), nodes.end(), std::inserter(filtered, filtered.end()),
        [val](decltype(nodes)::value_type const& pair) {
            return pair.first == val; // TO CHECK
        });

    bool hasExistingStatementNo = false;
    for (auto [v, node] : filtered) {
        // Case where a node for u already exists
        if (val == v) {
            return nodes.at(stmt);
        }

        // Invalid insert
        if (std::is_same_v<T, STMT_LO>::value &&
            v.statementNum == val.statementNum &&
            v.type.value() != val.type.value()) {
            return nullptr;
        }
    }

    if (!hasExistingStatementNo) {
        Node<T>* node = new Node<T>(val, nullptr, Node::NodeSet{});
        nodes.emplace(val, node);
        return node;
    }

    return nullptr;
}

template <typename T>
void Graph<T>::addEdge(T u, T v) {
    if (std::is_same_v<T, STMT_LO>::value) {
        // Second statement in the relationship cannot come before the first
        if (u.statementNum >= v.statementNum) {
            return;
        }

        // First statement in a Parent relationship has to be a container statement 
        if (type == PKBRelationship::PARENT && u.type.value() != StatementType::If
            && u.type.value() != StatementType::While) {
            return;
        }

    }

    Node<T>* uNode = createNode(u);
    Node<T>* vNode = createNode(v);

    if (!uNode || !vNode) {
        return;
    }

    // Add the edge between uNode and vNode
    uNode->next.push_back(vNode);
    if (!vNode->prev) {
        vNode->prev = uNode;
    }
}

template <typename T>
bool Graph<T>::contains(PKBField field1, PKBField field2) {
    T first = *field1.getContent<T>();
    T second = *field2.getContent<T>();

    if (nodes.count(first) != 0) {
        Node* curr = nodes.at(first);
        Node::NodeSet nextNodes = curr->next;
        Node::NodeSet filtered;
        std::copy_if(nextNodes.begin(), nextNodes.end(),
            std::back_inserter(filtered), [second](Node<T>* const& node) {
                return node->val == second;
            });
        return (filtered.size() == 1);
    }

    return false;
}

template <typename T>
bool Graph<T>::containsT(PKBField field1, PKBField field2) {
    T first = *field1.getContent<T>();
    T second = *field2.getContent<T>();

    // Base Case where Parent(field1, field2) holds
    if (this->contains(field1, field2)) {
        return true;
    }

    if (nodes.count(first) != 0) {
        Node* curr = nodes.at(first);
        Node::NodeSet nextNodes = curr->next;

        // Recursive lookup for each node in the NodeSet
        for (auto node : nextNodes) {
            T newVal = node->val;
            PKBField newField1 = PKBField::createConcrete(Content{ newVal });
            if (this->containsT(newField1, field2)) {
                return true;
            }
        }
    }
    return false;
}

template <typename T>
Result Graph<T>::retrieve(PKBField field1, PKBField field2) {
    bool isConcreteFirst = field1.fieldType == PKBFieldType::CONCRETE;
    bool isConcreteSec = field2.fieldType == PKBFieldType::CONCRETE;

    if (isConcreteFirst && isConcreteSec) {
        return contains(field1, field2) ? Result{ {{field1, field2}} } : Result{};
    } else if (isConcreteFirst && !isConcreteSec) {
        return traverseStart(field1, field2);
    } else if (!isConcreteFirst && isConcreteSec) {
        return traverseEnd(field1, field2);
    } else {
        return traverseAll(field1.statementType.value(), field2.statementType.value());
    }
}

template <typename T>
Result Graph<T>::retrieveT(PKBField field1, PKBField field2) {
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

template <typename T>
Result Graph<T>::traverseStart(PKBField field1, PKBField field2) {
    T target = *(field1.getContent<T>());
    Result res{};

    if (!target.type.has_value()) {
        return res;
    }


    if (nodes.count(target) != 0) {
        Node<T>* curr = nodes.at(target);

        if (!curr->next.empty()) {
            Node::NodeSet nextNodes = curr->next;
            Node::NodeSet filtered;
            std::copy_if(nextNodes.begin(), nextNodes.end(), std::back_inserter(filtered),
                [targetType](Node* const& node) {
                    // filter for statements. for all other type no filtering is required.
                    if (std::is_same_v<T, STMT_LO>::value) {
                        StatementType targetType = field2.statementType.value();
                        return (node->val.type.value() == targetType || targetType == StatementType::All);
                    }
                    return true;
                });

            for (Node<T>* node : filtered) {
                std::vector<PKBField> temp;
                PKBField second = PKBField::createConcrete(Content{ node->val });
                temp.push_back(field1);
                temp.push_back(second);
                res.insert(temp);
            }
        }
    }
    return res;
}

template <typename T>
Result Graph<T>::traverseStartT(PKBField field1, PKBField field2) {
    std::set<T> found;
    Result res{};
    T start = *(field1.getContent<T>());

    if (nodes.count(start) != 0) {
        if (std::is_same_v<T, STMT_LO>::value) { 
            StatementType targetType = field2.statementType.value(); 
            traverseStartT(&found, nodes.at(start), targetType);
        } else {
            traverseStartT(&found, nodes.at(start));
        }
    }

    if (!found.empty()) {
        std::transform(found.begin(), found.end(), std::insert_iterator<Result>(res, res.end()),
            [field1](T const& val) {
                PKBField second = PKBField::createConcrete(Content{ val });
                return std::vector<PKBField>{field1, second};
            });
    }

    return res;
}

template <typename T>
void Graph<T>::traverseStartT(std::set<T>* found, Node<T>* node, StatementType targetType) {
    Node::NodeSet nextNodes = node->next;

    if (!nextNodes.empty()) {
        for (auto nextNode : nextNodes) {
            // targetType is specified for statements
            if (targetType) {
                bool typeMatch = nextNode->val.type.value() == targetType || targetType == StatementType::All;
                if (typeMatch) {
                    found->insert(nextNode->stmt);
                }

                traverseStartT(found, nextNode, targetType);
            } else {
                traverseStartT(found, nextNode);
            }
        }
    }
    return;
}

template <typename T>
Result Graph<T>::traverseEnd(PKBField field1, PKBField field2) {
    T target = *(field2.getContent<T>());
    Result res{};

    if (std::is_same_v<T,STMT_LO>::value && !target.type.has_value()) {
        return res;
    }

    StatementType targetType = field1.statementType.value();

    if (nodes.count(target) != 0) {
        Node* curr = nodes.at(target);

        if (curr->prev != nullptr) {
            T prev = curr->prev->val;

            if (std::is_same_v<T, STMT_LO>::value) {
                if (prev.type.value() != targetType && targetType != StatementType::All) {
                    return res;
                }
            }

            PKBField first = PKBField::createConcrete(Content{ prev });
            res.insert(std::vector<PKBField>{first, field2});
        }
    }
    return res;
}

template <typename T>
Result Graph<T>::traverseEndT(PKBField field1, PKBField field2) {
    std::set<T> found;
    Result res{};
    T start = *(field2.getContent<T>());

    if (nodes.count(start) != 0) {
        if (std::is_same_v<T, STMT_LO>::value) {
            StatementType targetType = field1.statementType.value();
            traverseEndT(&found, nodes.at(start), targetType);
        } else {
            traverseEndT(&found, nodes.at(start));
        }
    }

    if (!found.empty()) {
        std::transform(found.begin(), found.end(), std::insert_iterator<Result>(res, res.end()),
            [field2](T const& val) {
                PKBField first = PKBField::createConcrete(Content{ val });
                return std::vector<PKBField>{first, field2};
            });
    }

    return res;
}

template <typename T>
void Graph<T>::traverseEndT(std::set<T>* found, Node<T>* node, StatementType targetType) {
    while (node->prev) {
        if (std::is_same_v<T, STMT_LO>::value) {
            bool typeMatch = node->prev->val.type.value() == targetType || targetType == StatementType::All;
            if (!typeMatch) {
                return;
            }
        }

        found->insert(node->prev->val);
        node = node->prev;
    }
}

template <typename T>
Result Graph<T>::traverseAll(StatementType type1, StatementType type2) {
    Result res{};

    if (type1 && !type2) {
        return res;
    }

    if (!type1 && type2) {
        return res;
    }

    for (auto const& [key, node] : nodes) {
        Node<T>* curr = node;

        if (!curr->next.empty()) {
            if (std::is_same_v<T, STMT_LO>::value) {
                if (curr->val.type.value() == type1 || type1 == StatementType::All) {
                    return res;
                }
            }

            Node::NodeSet nextNodes = curr->next;

            // Filter nodes that match second statement type
            Node::NodeSet filtered;
            std::copy_if(nextNodes.begin(), nextNodes.end(), std::back_inserter(filtered),
                [type2](Node<T>* const& node) {
                    if (is_same_v<T, STMT_LO>::value) {
                        return node->stmt.type.value() == type2 || type2 == StatementType::All;
                    }
                    return true;
                });

            // Populate result
            std::transform(filtered.begin(), filtered.end(), std::insert_iterator<Result>(res, res.end()),
                [curr](Node<T>* const& node) {
                    PKBField first = PKBField::createConcrete(Content{ curr->val });
                    PKBField second = PKBField::createConcrete(Content{ node->val });
                    return std::vector<PKBField>{first, second};
                });
        }
    }
    return res;
}

template <typename T>
Result Graph<T>::traverseAllT(StatementType type1, StatementType type2) {
    Result res;
    
    if (type1 && !type2) {
        return res;
    }

    if (!type1 && type2) {
        return res;
    }

    std::set<T> found;

    for (auto const& [key, node] : nodes) {
        Node* curr = node;
        found.clear();

        if (std::is_same_v<T, STMT_LO>::value) {
            if (curr->val.type.value() == type1 || type1 == StatementType::All) {
                return res;
            }

            traverseStartT(&found, node, type2);
        } else {
            traverseStartT(&found, node);
        }

        std::transform(found.begin(), found.end(), std::insert_iterator<Result>(res, res.end()),
            [curr](T const& val) {
                PKBField first = PKBField::createConcrete(Content{ curr->val });
                PKBField second = PKBField::createConcrete(Content{ val });
                return std::vector<PKBField>{first, second};
            });
    }

    return res;
}

template <typename T>
int Graph<T>::getSize() {
    return nodes.size();
}

/** ======================= TRANSITIVERELATIONSHIPTABLE METHODS ======================== */

void TransitiveRelationshipTable::insert(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "Only concrete statements can be inserted into a Follows or Parent table!";
        return;
    }

    graph->addEdge(*field1.getContent<STMT_LO>(), *field2.getContent<STMT_LO>());
}

TransitiveRelationshipTable::TransitiveRelationshipTable(PKBRelationship type) : RelationshipTable(type),
graph(std::make_unique<Graph>(type)) {
}

bool TransitiveRelationshipTable::contains(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "Only concrete statements can be inserted into a Follows or Parent table!";
        return false;
    }

    return graph->contains(field1, field2);
}

bool TransitiveRelationshipTable::containsT(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "a Follows or Parent table can only contain concrete fields and STATEMENT entity types.";
        return false;
    }

    return graph->containsT(field1, field2);
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
            "Only STATEMENT entity types can be retrieved from a Follows or Parent table.";
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
            "Only STATEMENT entity types can be retrieved from a Follows or Parent table.";
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
