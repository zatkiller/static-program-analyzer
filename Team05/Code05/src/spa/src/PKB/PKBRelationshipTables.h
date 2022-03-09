#pragma once

#include <set>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <map>
#include <iterator>
#include <optional>
#include <memory>
#include <type_traits>
#include <cstdarg>
#include "logging.h"
#include "PKBField.h"
#include "PKBCommons.h"

/**
* A data structure to store a program design abstraction (relationship) which has two fields.
*/
class RelationshipRow {
public:
    RelationshipRow(PKBField, PKBField);

    /**
    * Retrieves the first field in the relationship.
    */
    PKBField getFirst() const;

    /**
    * Retrieves the second field in the relationship.
    */
    PKBField getSecond() const;

    bool operator==(const RelationshipRow&) const;

private:
    PKBField field1;
    PKBField field2;
};

/**
* Hash function for RelationshipRow.
*/
class RelationshipRowHash {
public:
    size_t operator()(const RelationshipRow&) const;
};

using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

/**
* A data structure to store program design abstractions. Base class of *RelationshipTables.
*/
class RelationshipTable {
public:
    explicit RelationshipTable(PKBRelationship);

    /**
    * Checks whether the RelationshipTable contains a RelationshipRow representing
    * Relationship(field1, field2). If either fields are not concrete or if a statement field
    * has no corresponding row in statement table, return false.
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    *
    * @return whether the relationship is present in the RelationshipTable
    */
    virtual bool contains(PKBField field1, PKBField field2) const = 0;

    /**
    * Inserts a RelationshipRow representing Relationship(field1, field2) into the RelationshipTable.
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    */
    virtual void insert(PKBField field1, PKBField field2) = 0;

    /**
    * Retrieves all pairs of PKBFields in table that satisfies the parameters. If a statement field
    * has no corresponding row in statement table, or if the first parameter is a wildcard, return false.
    *
    * @param field1 the first program design entity in the query
    * @param field2 the second program design entity in the query
    * @return FieldRowResponse
    *
    * @see PKBField
    */
    virtual FieldRowResponse retrieve(PKBField field1, PKBField field2) const = 0;

    /**
    * Retrieves the type of relationships the RelationshipTable stores.
    *
    * @return type of relationship
    */
    PKBRelationship getType() const;

    /**
    * Retrieves the number of relationships in the table.
    *
    * @return int number of relationships
    */
    virtual int getSize() const = 0;

protected:
    PKBRelationship type;

    /**
    * Checks if the two PKBFields provided can be inserted into the table (whether it can exist in the table).
    * Checks that the fields are concrete and have valid STMT_LO if they are statementss.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    * @see PKBField
    */
    virtual bool isInsertOrContainsValid(PKBField field1, PKBField field2) const = 0;

    /**
    * Checks if the two PKBFields provided can be retrieved from the table.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    * @see PKBField
    */
    virtual bool isRetrieveValid(PKBField field1, PKBField field2) const = 0;
};

/**
* A data structure to store program design abstractions as RelationshipRows. Inherits RelationshipTable.
*/
class NonTransitiveRelationshipTable : public RelationshipTable {
public:
    explicit NonTransitiveRelationshipTable(PKBRelationship);

    /**
    * Checks whether the RelationshipTable contains a RelationshipRow representing
    * Relationship(field1, field2). If either fields are not concrete or if a statement field
    * has no corresponding row in statement table, return false.
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    *
    * @return whether the relationship is present in the RelationshipTable
    */
    bool contains(PKBField field1, PKBField field2) const override;

    /**
    * Inserts a RelationshipRow representing Relationship(field1, field2) into the RelationshipTable.
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    */
    void insert(PKBField field1, PKBField field2) override;

    /**
    * Retrieves all pairs of PKBFields in table that satisfies the parameters. If a statement field
    * has no corresponding row in statement table, or if the first parameter is a wildcard, return false.
    *
    * @param field1 the first program design entity in the query
    * @param field2 the second program design entity in the query
    * @return FieldRowResponse
    *
    * @see PKBField
    */
    FieldRowResponse retrieve(PKBField field1, PKBField field2) const override;

    /**
    * Retrieves the number of relationships in the table.
    *
    * @return int number of relationships
    */
    int getSize() const override;

private:
    std::unordered_set<RelationshipRow, RelationshipRowHash> rows;

    /**
    * Checks if the two PKBFields provided can be inserted into the table (whether it can exist in the table).
    * Checks that the fields are concrete and have valid STMT_LOs if they are statements.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    * @see PKBField
    */
    bool isInsertOrContainsValid(PKBField field1, PKBField field2) const override;

    /**
    * Checks if the two PKBFields provided can be retrieved from the table.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    * @see PKBField
    */
    bool isRetrieveValid(PKBField field1, PKBField field2) const override;
};

using Result = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

/**
* A bi-directional node inside a Graph, where there can be any number of next nodes and one previous node.
*
* @tparam T the type each node stores
* @see Graph
*/
template<typename T>
struct Node {
    using NodeSet = std::unordered_set<Node<T>*>;

    Node(T val, NodeSet prev, NodeSet next) : val(val), next(next), prev(prev) {}

    T val;
    NodeSet next; /**< The descendant(s) of this Node. */
    NodeSet prev; /**< The predecessor(s) of this Node. */
};

/**
* A data structure that consists of Node, where each edge represents a valid relationship. For brevity, 
* program design abstractions (Follows, Parent, Calls) will be denoted by rs(u, v),
* where rs is the type of transitive relationship this graph holds.
* 
* @tparam T the type each node in the graph stores
* @see Node
*/
template<typename T>
class Graph {
public:
    explicit Graph<T>(PKBRelationship type) : type(type) {}

    /**
    * Adds an edge between two nodes to represent a relationship. Initialises Nodes for
    * if they are not present in the graph.
    *
    * @param u the first program design entity in a rs(u,v) relationship
    * @param v the second program design entity in a rs(u,v) relationship
    */
    void addEdge(T u, T v) {
        if constexpr (std::is_same_v<T, STMT_LO>) {
            // Second statement in the relationship cannot come before the first
            if (type == PKBRelationship::PARENT || type == PKBRelationship::FOLLOWS) {
                if (u.statementNum >= v.statementNum) {
                    return;
                }
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
        uNode->next.insert(vNode);

        // Add the edge between vNode and uNode
        vNode->prev.insert(uNode);
    }

    /**
    * Checks if rs(field1, field2) is in the graph.
    *
    * @param field1 the first program design entity  in a rs(u,v) query wrapped in a PKBField
    * @param field2 the second program design entity  in a rs(u,v) query wrapped in a PKBField
    *
    * @return bool true if rs(field1, field2) is in the graph and false otherwise
    * @see PKBField
    */
    bool contains(PKBField field1, PKBField field2) const {
        T first = *field1.getContent<T>();
        T second = *field2.getContent<T>();

        if (nodes.count(first) != 0) {
            Node<T>* curr = nodes.at(first);
            typename Node<T>::NodeSet nextNodes = curr->next;
            typename Node<T>::NodeSet filtered;
            std::copy_if(nextNodes.begin(), nextNodes.end(),
                std::inserter(filtered, filtered.end()), [second](Node<T>* const& node) {
                    return node->val == second;
                });
            return (filtered.size() == 1);
        }

        return false;
    }

    /**
    * Checks if rs*(field1, field2) is in the graph.
    *
    * @param field1 the first program design entity  in a rs*(u,v) query wrapped in a PKBField
    * @param field2 the second program design entity  in a rs*(u,v) query wrapped in a PKBField
    *
    * @return bool true if rs*(field1, field2) is in the graph and false otherwise
    * @see PKBField
    */
    bool containsT(PKBField field1, PKBField field2) const {
        T first = *field1.getContent<T>();
        T second = *field2.getContent<T>();

        // Base Case where rs(field1, field2) holds
        if (this->contains(field1, field2)) {
            return true;
        }

        if (nodes.count(first) != 0) {
            Node<T>* curr = nodes.at(first);
            typename Node<T>::NodeSet nextNodes = curr->next;

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


    /**
    * Gets all pairs of PKBFields that satisfy the provided rs relationship, rs(field1, field2).
    *
    * @param field1 the first program design entity  in a rs(u,v) query wrapped in a PKBField
    * @param field2 the second program design entity  in a rs(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    * that satisfy rs(field1, field2)
    * @see PKBField
    */
    Result retrieve(PKBField field1, PKBField field2) const {
        bool isConcreteFirst = field1.fieldType == PKBFieldType::CONCRETE;
        bool isConcreteSec = field2.fieldType == PKBFieldType::CONCRETE;

        if (isConcreteFirst && isConcreteSec) {
            return contains(field1, field2) ? Result{ {{field1, field2}} } : Result{};
        } else if (isConcreteFirst && !isConcreteSec) {
            return traverseStart(field1, field2);
        } else if (!isConcreteFirst && isConcreteSec) {
            return traverseEnd(field1, field2);
        } else {
            return traverseAll(field1, field2);
        }
    }

    /**
    * Gets all pairs of PKBFields that satisfy the provided rs* relationship, rs*(field1, field2).
    *
    * @param field1 the first program design entity  in a rs*(u,v) query wrapped in a PKBField
    * @param field2 the second program design entity  in a rs*(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    *   that satisfy rs*(field1, field2)
    */
    Result retrieveT(PKBField field1, PKBField field2) const {
        bool isConcreteFirst = field1.fieldType == PKBFieldType::CONCRETE;
        bool isDeclarationFirst = field1.fieldType == PKBFieldType::DECLARATION;
        bool isConcreteSec = field2.fieldType == PKBFieldType::CONCRETE;
        bool isDeclarationSec = field2.fieldType == PKBFieldType::DECLARATION;

        if (isConcreteFirst && isDeclarationSec) {
            return traverseStartT(field1, field2);
        } else if (isDeclarationFirst && isConcreteSec) {
            return traverseEndT(field1, field2);
        } else if (isDeclarationFirst && isDeclarationSec) {
            return traverseAllT(field1, field2);
        } else {
            return containsT(field1, field2) ? Result{ {{field1, field2}} } : Result{};
        }
    }

    /**
    * Retrieves the number of relationships in the table.
    *
    * @return int number of relationships
    */
    int getSize() const {
        return nodes.size();
    }

private:
    PKBRelationship type; /**< The type of relationships this Graph holds */
    std::map<T, Node<T>*> nodes; /**< The list of nodes in this Graph */

    /**
    * Add a node represented by program design entity to the list of nodes stored in the graph if it does not exist 
    * and is valid. Return a pointer to existing or newly created node. If val is invalid, return a nullptr.
    *
    * @param val a program design entity  representing the statement
    */
    Node<T>* createNode(T val) {
        // Filter nodes whose val are the same as the inputs
        std::map<T, Node<T>*> filtered;
        std::copy_if(nodes.begin(), nodes.end(), std::inserter(filtered, filtered.end()),
            [val](auto const& pair) {
                if constexpr (std::is_same_v<T, STMT_LO>) {
                    return pair.first.statementNum == val.statementNum;
                }

                return pair.first == val;  // TO CHECK
            });

        for (auto [v, node] : filtered) {
            // Case where a node for u already exists
            if (val == v) {
                return nodes.at(v);
            }

            // Invalid insert
            if constexpr (std::is_same_v<T, STMT_LO>) {
                if (v.statementNum == val.statementNum &&
                    v.type.value() != val.type.value()) {
                    return nullptr;
                }
            }
        }

        Node<T>* node = new Node<T>(val, typename Node<T>::NodeSet{}, typename Node<T>::NodeSet{});
        nodes.emplace(val, node);
        return node;
    }

    /**
    * Gets all pairs of PKBFields that satisfy the provided non-transitive relationship, rs(field1, field2),
    * where field1 is a concrete field and field2 is either a declaration or wildcard.
    * Statement wildcards are treated as a statement declaration for any statement type (StatementType::ALL).
    *
    * @param field1 a concrete field to begin the traversal from
    * @param field2 a declaration
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where the
    * second item in each pair satisfies the declaration.
    *
    * @see PKBField
    */
    Result traverseStart(PKBField field1, PKBField field2) const {
        T target = *(field1.getContent<T>());
        Result res{};

        if constexpr (std::is_same_v<T, STMT_LO>) {
            if (!target.type.has_value()) {
                return res;
            }
        }


        if (nodes.count(target) != 0) {
            Node<T>* curr = nodes.at(target);

            if (!curr->next.empty()) {
                typename Node<T>::NodeSet nextNodes = curr->next;
                typename Node<T>::NodeSet filtered;
                std::copy_if(nextNodes.begin(), nextNodes.end(), std::inserter(filtered, filtered.end()),
                    [&](Node<T>* const& node) {
                        // filter for statements. for all other type no filtering is required.
                        if constexpr (std::is_same_v<T, STMT_LO>) {
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

    /**
    * Gets all pairs of PKBFields that satisfy the provided transitive relationship, rs*(field1, field2),
    * where field1 is a concrete field and field2 is either a declaration or wildcard.
    * Statement wildcards are treated as a statement declaration for any statement type.
    *
    * @param field1 a concrete field to begin the traversal from
    * @param field2 a declaration
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where the
    *   second item in each pair satisfies the declaration.
    *
    * @see PKBField
    */
    Result traverseStartT(PKBField field1, PKBField field2) const {
        std::set<T> found;
        Result res{};
        T start = *(field1.getContent<T>());

        if (nodes.count(start) != 0) {
            if constexpr (std::is_same_v<T, STMT_LO>) {
                StatementType targetType = field2.statementType.value();
                traverseStartT(&found, nodes.at(start), nodes.at(start), targetType);
            } else {
                traverseStartT(&found, nodes.at(start), nodes.at(start));
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

    /**
    * An overloaded helper function that traverses the graph forward starting at the provided node
    * until there are no next nodes left.
    *
    * @param found a pointer to a set of program design entity  that stores the possible program design 
    * entity for the second field in a transitive relationship.
    * @param node a pointer to the node to begin traversal from
    * @param targetType an optional statement type if the graphs contains STMT_LOs
    *
    * @see PKBField
    */
    void traverseStartT(std::set<T>* found, Node<T>* node, Node<T>* initial, StatementType targetType = StatementType::None) const {
        typename Node<T>::NodeSet nextNodes = node->next;
        bool isTerminating = std::count(nextNodes.begin(), nextNodes.end(), initial) == 1;

        for (auto nextNode : nextNodes) {
            // targetType is specified for statements
            if constexpr (std::is_same_v<T, STMT_LO>) {
                bool typeMatch = nextNode->val.type.value() == targetType || targetType == StatementType::All;
                if (typeMatch) {
                    found->insert(nextNode->val);
                }

                if (!isTerminating) {
                    traverseStartT(found, nextNode, initial, targetType);
                } else {
                    return;
                }
            } else {
                found->insert(nextNode->val);
                traverseStartT(found, nextNode, initial);
            }
        }
        return;
    }

    /**
    * Gets all pairs of PKBFields that satisfy the provided non-transitive relationship, rs(field1, field2),
    * where field1 is either a declaration or wildcard and field2 is a concrete field.
    * Statement wildcards are treated as a statement declaration for any statement type.
    *
    * @param field1 a declaration
    * @param field2 a concrete field to begin the traversal from
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where the
    * second item in each pair satisfies the statement declaration.
    *
    * @see PKBField
    */
    Result traverseEnd(PKBField field1, PKBField field2) const {
        T target = *(field2.getContent<T>());
        Result res{};

        if constexpr (std::is_same_v<T, STMT_LO>) {
            if (!target.type.has_value()) {
                return res;
            }
        }

        if (nodes.count(target) != 0) {
            Node<T>* curr = nodes.at(target);

            if (!curr->prev.empty()) {
                typename Node<T>::NodeSet prevNodes = curr->prev;
                typename Node<T>::NodeSet filtered;
                std::copy_if(prevNodes.begin(), prevNodes.end(), std::inserter(filtered, filtered.end()),
                    [&](Node<T>* const& node) {
                        // filter for statements. for all other type no filtering is required.
                        if constexpr (std::is_same_v<T, STMT_LO>) {
                            StatementType targetType = field1.statementType.value();
                            return (node->val.type.value() == targetType || targetType == StatementType::All);
                        }
                        return true;
                    });

                for (Node<T>* node : filtered) {
                    std::vector<PKBField> temp;
                    PKBField first = PKBField::createConcrete(Content{ node->val });
                    temp.push_back(first);
                    temp.push_back(field2);
                    res.insert(temp);
                }
            }
        }
        return res;
    }

    /**
    * Gets all pairs of PKBFields that satisfy the provided transitive relationship, rs*(field1, field2),
    * where field1 is either a declaration or wildcard and field2 is a concrete field.
    * Statement wildcards are treated as a statement declaration for any statement type.
    *
    * @param field1 a declaration
    * @param field2 a concrete field to begin the traversal from
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where the
    * second item in each pair satisfies the statement declaration.
    *
    * @see PKBField
    */
    Result traverseEndT(PKBField field1, PKBField field2) const {
        std::set<T> found;
        Result res{};
        T start = *(field2.getContent<T>());

        if (nodes.count(start) != 0) {
            if (std::is_same_v<T, STMT_LO>) {
                StatementType targetType = field1.statementType.value();
                traverseEndT(&found, nodes.at(start), nodes.at(start), targetType);
            } else {
                traverseEndT(&found, nodes.at(start), nodes.at(start));
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

    /**
    * An overloaded helper function that traverses the graph backwards starting at the provided node
    * until there are no prev nodes left.
    *
    * @param found a pointer to a set of program design entity that stores the possible program design 
    * entity for the first field in a transitive relationship.
    * @param node a pointer to the node to begin traversal from
    * @param targetType an optional statement type if the graphs contains STMT_LOs
    *
    * @see PKBField
    */
    void traverseEndT(std::set<T>* found, Node<T>* node, Node<T>* initial, StatementType targetType = StatementType::None) const {
        typename Node<T>::NodeSet prevNodes = node->prev;
        bool isTerminating = std::find(prevNodes.begin(), prevNodes.end(), initial) != prevNodes.end();
        for (auto prevNode : prevNodes) {
            // targetType is specified for statements
            if constexpr (std::is_same_v<T, STMT_LO>) {
                bool typeMatch = prevNode->val.type.value() == targetType || targetType == StatementType::All;
                if (typeMatch) {
                    found->insert(prevNode->val);
                }
                if (!isTerminating) {
                    traverseEndT(found, prevNode, initial, targetType);
                } else {
                    return;
                }
            } else {
                found->insert(prevNode->val);
                traverseEndT(found, prevNode, initial);
            }
        }
        return;
    }

    /**
    * Gets all pairs (field1, field2) of PKBFields that satisfy the provided relationship, rs(field1, field2).
    *
    * Internally, iterates through the nodes in the graph calls traverseStart with each node.
    *
    * @param field1 the first field
    * @param field2 the second field
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where
    * each item in each pair satisfies the parameters.
    * @see PKBField
    */
    Result traverseAll(PKBField field1, PKBField field2) const {
        Result res{};

        for (auto const& [key, node] : nodes) {
            Node<T>* curr = node;

            if (!curr->next.empty()) {
                if constexpr (std::is_same_v<T, STMT_LO>) {
                    // for statement declarations or wildcards, its statement type will be initialized
                    if (curr->val.type.value() != field1.statementType.value() &&
                        field1.statementType.value() != StatementType::All) {
                        continue;
                    }
                }

                typename Node<T>::NodeSet nextNodes = curr->next;

                // Filter nodes that match second statement type
                typename Node<T>::NodeSet filtered;
                std::copy_if(nextNodes.begin(), nextNodes.end(), std::inserter(filtered, filtered.end()),
                    [&](Node<T>* const& node) {
                        if constexpr (std::is_same_v<T, STMT_LO>) {
                            return node->val.type.value() == field2.statementType.value() ||
                                field2.statementType.value() == StatementType::All;
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

    /**
    * Gets all pairs (field1, field2) of PKBFields that satisfy the provided transitive relationship.
    *
    * Internally, iterates through the nodes in the graph and calls traverseStart on each node.
    *
    * @param type1 the first field
    * @param type2 the second field
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where each
    * item in each pair satisfies the corresponding the parameters.
    *
    * @see PKBField
    */
    Result traverseAllT(PKBField field1, PKBField field2) const {
        Result res;

        std::set<T> found;

        for (auto const& [key, node] : nodes) {
            Node<T>* curr = node;
            found.clear();

            if constexpr (std::is_same_v<T, STMT_LO>) {
                // for statement declarations or wildcards, its statement type would have been initialized
                if (curr->val.type.value() != field1.statementType.value() &&
                    field1.statementType.value() != StatementType::All) {
                    continue;
                }

                traverseStartT(&found, node, node, field2.statementType.value());
            } else {
                traverseStartT(&found, node, node);
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
};

/**
* A data structure to store program design abstractions as Graphs. Inherits RelationshipTable.
* 
* @tparam T the type each node in the graph stores
* @see Node
*/
template<typename T>
class TransitiveRelationshipTable : public RelationshipTable {
public:
    explicit TransitiveRelationshipTable(PKBRelationship type) : RelationshipTable(type) {
        graph = std::make_unique<Graph<T>>(type);
    }

    /**
    * Checks whether the TransitiveRelationshipTable contains rs(field1, field2).
    *
    * @param field1 the first program design entity in a rs(u,v) query wrapped in a PKBField
    * @param field2 the second program design entity in a rs(u,v) query wrapped in a PKBField
    *
    * @return bool whether rs(field1, field2) is in the graph
    *
    * @see PKBField
    */
    bool contains(PKBField field1, PKBField field2) const override {
        if (!isInsertOrContainsValid(field1, field2)) {
            Logger(Level::ERROR) << "Invalid contains on a TransitiveRelationshipTable.";
            return false;
        }

        return graph->contains(field1, field2);
    }

    /**
    * Inserts into Graph an edge representing rs(field1, field2).
    * If the two provided entities are not valid statements or are not concrete, no insert will be done
    * and an error will be thrown.
    *
    * @param field1 the first program design entity in a rs(u,v) relationship wrapped in a PKBField
    * @param field2 the second program design entity in a rs(u,v) relationship wrapped in a PKBField
    *
    * @see PKBField
    */
    void insert(PKBField field1, PKBField field2) override {
        if (!isInsertOrContainsValid(field1, field2)) {
            Logger(Level::ERROR) << "Invalid insert on a TransitiveRelationshipTable.";
            return;
        }

        graph->addEdge(*field1.getContent<T>(), *field2.getContent<T>());
    }

    void convertWildcardToDeclaration(PKBField* field) const {
        if (field->fieldType == PKBFieldType::WILDCARD) {
            field->fieldType = PKBFieldType::DECLARATION;

            if (field->entityType == PKBEntityType::STATEMENT) {
                field->statementType = StatementType::All;
            }
        }
    }

    /**
    * Retrieves all pairs of program design entities that satisfies rs(field1, field2).
    *
    * @param field1 the first program design entity in a rs(u,v) query wrapped in a PKBField
    * @param field2 the second program design entity in a rs(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> an unordered set of vectors of PKBFields,
    * where each vector represents the two program design entities in the relationship,
    * i.e. rs(field1, field2) -> [field1, field2].
    *
    * @see PKBField
    */
    FieldRowResponse retrieve(PKBField field1, PKBField field2) const override {
        // Both fields have to be a statement type
        if (!isRetrieveValid(field1, field2)) {
            Logger(Level::ERROR) <<
                "Invalid retrieve from a TransitiveRelationshipTable.";
            return FieldRowResponse{};
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

    /**
    * Checks whether the TransitiveRelationshipTable contains rs*(field1, field2).
    *
    * @param field1 the first program design entity in a rs*(u,v) query wrapped in a PKBField
    * @param field2 the second program design entity in a rs*(u,v) query wrapped in a PKBField
    *
    * @return bool whether rs*(field1, field2) is in the graph
    *
    * @see PKBField
    */
    bool containsT(PKBField field1, PKBField field2) const {
        if (!isInsertOrContainsValid(field1, field2)) {
            Logger(Level::ERROR) <<
                "Invalid insert/contains on a TransitiveRelationshipTable.";
            return false;
        }

        return graph->containsT(field1, field2);
    }

    /**
    * Retrieves all pairs of program design entities that satisfies rs*(field1, field2).
    *
    * @param field1 the first program design entity in a rs*(u,v) query wrapped in a PKBField
    * @param field2 the second program design entity in a rs*(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> an unordered set of vectors of PKBFields,
    *  each vector represents the two program design entities in a rs* relationship,
    * i.e. rs*(field1, field2) -> [field1, field2].
    *
    * @see PKBField
    */
    FieldRowResponse retrieveT(PKBField field1, PKBField field2) const {
        // Both fields have to be a statement type
        if (!isRetrieveValid(field1, field2)) {
            Logger(Level::ERROR) <<
                "Invalid retrieve from a TransitiveRelationshipTable.";
            return FieldRowResponse{};
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

    int getSize() const override {
        return graph->getSize();
    }

private:
    std::unique_ptr<Graph<T>> graph;

    /**
    * Checks if the two PKBFields provided can be inserted into the table (whether it can exist in the table).
    * Checks that the fields are concrete and valid.
    * 
    * The two fields must be of the same program design entity type and must be concrete. Additionally, the value
    * stored in the graph must match the one stored in the PKBFields.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    *
    * @see PKBField
    */
    bool isInsertOrContainsValid(PKBField field1, PKBField field2) const override {
        if (field1.entityType != field2.entityType) {
            return false;
        }

        if (!field1.isValidConcrete(field1.entityType)) {
            return false;
        }

        if (!field2.isValidConcrete(field2.entityType)) {
            return false;
        }

        if (!std::holds_alternative<T>(field1.content)) {
            return false;
        }

        return true;
    }

    /**
    * Checks if the two PKBFields provided can be retrieved from the table.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    *
    * @see PKBField
    */
    bool isRetrieveValid(PKBField field1, PKBField field2) const override {
        // for transitive tables, both fields have to be the same type
        if (field1.entityType != field2.entityType) {
            return false;
        }

        if (std::is_same_v<T, STMT_LO>) {
            return  field1.entityType == PKBEntityType::STATEMENT;
        } else if (std::is_same_v<T, PROC_NAME>) {
            return  field1.entityType == PKBEntityType::PROCEDURE;
        } else if (std::is_same_v<T, VAR_NAME>) {
            return  field1.entityType == PKBEntityType::VARIABLE;
        } else {  // CONST
            return  field1.entityType == PKBEntityType::CONST;
        }
    }
};

/**
* A data structure to store Modifies program design abstractions as RelationshipRows. Inherits RelationshipTable.
*/
class ModifiesRelationshipTable : public NonTransitiveRelationshipTable {
public:
    ModifiesRelationshipTable();
};

/**
* A data structure to store Uses program design abstractions as RelationshipRows. Inherits RelationshipTable.
*/
class UsesRelationshipTable : public NonTransitiveRelationshipTable {
public:
    UsesRelationshipTable();
};

/**
 * A data structure to store Follows and FollowsT program design abstractions as Nodes<STMT_LO> in a Graph<STMT_LO>.
 * Inherits from TransitiveRelationshipTable.
 *
 * @see Node, Graph, TransitiveRelationshipTable
 */
class FollowsRelationshipTable : public TransitiveRelationshipTable<STMT_LO> {
public:
    FollowsRelationshipTable();
};

/**
* A data structure to store Parent and Parent* program design abstractions as Nodes<STMT_LO> in a Graph<STMT_LO>.
* Inherits from TransitiveRelationshipTable
*
* @see Node, Graph, TransitiveRelationshipTable
*/
class ParentRelationshipTable : public TransitiveRelationshipTable<STMT_LO> {
public:
    ParentRelationshipTable();
};

/**
* A data structure to store Calls and Calls* program design abstractions as Nodes<PROC_NAME> in a Graph<PROC_NAME>.
* Inherits from TransitiveRelationshipTable
* 
* @see Node, Graph, TransitiveRelationshipTable
*/
class CallsRelationshipTable : public TransitiveRelationshipTable<PROC_NAME> {
public:
    CallsRelationshipTable();
};

/**
* A data structure to store Next and Next* program design abstractions as Nodes<STMT_LO> in a Graph<STMT_LO>.
* Inherits from TransitiveRelationshipTable
* 
* @see Node, Graph, TransitiveRelationshipTable
*/
class NextRelationshipTable : public TransitiveRelationshipTable<STMT_LO> {
public:
    NextRelationshipTable();
};