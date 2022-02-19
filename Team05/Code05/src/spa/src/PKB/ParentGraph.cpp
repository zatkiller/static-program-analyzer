#include "ParentGraph.h"

void ParentGraph::addEdge(STMT_LO u, STMT_LO v) {
    ParentNode* uNode;
    ParentNode* vNode;

    // Statement's parent cannot come after the statement
    if (u.statementNum >= v.statementNum) {
        return;
    }

    // First stmt has to be a container statement
    if (u.type.value() != StatementType::If && u.type.value() != StatementType::While) {
        return;
    }

    // Filter nodes whose statement numbers are the same as inputs
    std::map<STMT_LO, ParentNode*> filtered;
    std::copy_if(nodes.begin(), nodes.end(), std::inserter(filtered, filtered.end()),
        [u, v](decltype(nodes)::value_type const& pair) {
            return pair.first.statementNum == u.statementNum ||
                pair.first.statementNum == v.statementNum;
        });

    bool hasExistingStatementNo = false;
    for (auto [stmt, node] : filtered) {
        
        // Case where a node for u already exists
        if (stmt == u) {
            uNode = nodes.at(u);
            hasExistingStatementNo = true;
            break;
        }

        // Invalid insert
        if (stmt.statementNum == u.statementNum && stmt.type.value() != u.type.value()) {
            return;
        }
    }

    if (!hasExistingStatementNo) {
        uNode = new ParentNode(u, nullptr, ParentNode::NodeSet{});
        nodes.emplace(u, uNode);
    }

    hasExistingStatementNo = false;
    for (auto [stmt, node] : filtered) {
        
        // Case where a node for v already exists
        if (stmt == v) {
            vNode = nodes.at(v);
            hasExistingStatementNo = true;
            break;
        }

        // Invalid insert
        if (stmt.statementNum == v.statementNum && stmt.type.value() != v.type.value()) {
            return;
        }
    }

    if (!hasExistingStatementNo) {
        vNode = new ParentNode(v, nullptr, ParentNode::NodeSet{});
        nodes.emplace(v, vNode);
    }

    // Add the edge between uNode and vNode
    uNode->next.push_back(vNode);
    if (!vNode->prev) {
        vNode->prev = uNode;
    }
}

bool ParentGraph::getContains(PKBField field1, PKBField field2) {
    STMT_LO parent = *field1.getContent<STMT_LO>();
    STMT_LO child = *field2.getContent<STMT_LO>();

    if (nodes.count(parent) != 0) {
        ParentNode* curr = nodes.at(parent);
        ParentNode::NodeSet nextNodes = curr->next;
        ParentNode::NodeSet filtered;
        std::copy_if(nextNodes.begin(), nextNodes.end(),
            std::back_inserter(filtered), [child](ParentNode* const& node) {
                return node->stmt == child;
            });
        return (filtered.size() == 1);
    }
    return false;
}

bool ParentGraph::getContainsT(PKBField field1, PKBField field2) {
    STMT_LO parent = *field1.getContent<STMT_LO>();
    STMT_LO child = *field2.getContent<STMT_LO>();

    // Base Case where Parent(field1, field2) holds
    if (this->getContains(field1, field2)) {
        return true;
    }

    if (nodes.count(parent) != 0) {
        ParentNode* curr = nodes.at(parent);
        ParentNode::NodeSet nextNodes = curr->next;

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

Result ParentGraph::getParent(PKBField field1, PKBField field2) {
    bool isConcreteFirst = field1.fieldType == PKBFieldType::CONCRETE;
    bool isConcreteSec = field2.fieldType == PKBFieldType::CONCRETE;

    if (isConcreteFirst && isConcreteSec) {
        return this->getContains(field1, field2) 
            ? Result{ {{field1, field2}} } 
            : Result{};

    } else if (isConcreteFirst && !isConcreteSec) {
        STMT_LO target = *(field1.getContent<STMT_LO>());
        Result res{};

        if (!target.type.has_value() && !field1.statementType.has_value()) {
            return res;
        }

        StatementType targetType = field2.statementType.value();

        if (nodes.count(target) != 0) {
            ParentNode* curr = nodes.at(target);

            if (!curr->next.empty()) {
                ParentNode::NodeSet nextNodes = curr->next;
                ParentNode::NodeSet filtered;
                std::copy_if(nextNodes.begin(), nextNodes.end(), std::back_inserter(filtered),
                    [targetType](ParentNode* const& node) {
                        return (node->stmt.type.value() == targetType || targetType == StatementType::All);
                    });
                for (ParentNode *node : filtered) {
                    std::vector<PKBField> temp;
                    PKBField second = PKBField::createConcrete(Content{ node->stmt });
                    temp.push_back(field1);
                    temp.push_back(second);
                    res.insert(temp);
                }
            }
        }
        return res;

    } else if (!isConcreteFirst && isConcreteSec) {
        STMT_LO target = *(field2.getContent<STMT_LO>());
        Result res{};

        if (!target.type.has_value() && !field2.statementType.has_value()) {
            return res;
        }

        StatementType targetType = field1.statementType.value();

        if (nodes.count(target) != 0) {
            ParentNode* curr = nodes.at(target);

            if (curr->prev != nullptr) {
                STMT_LO prevStmt = curr->prev->stmt;
                if (prevStmt.type.value() == targetType || targetType == StatementType::All) {
                    PKBField first = PKBField::createConcrete(Content{ prevStmt });
                    res.insert(std::vector<PKBField>{first, field2});
                }
            }
        }
        return res;

    } else {
        return this->traverseAll(field1.statementType.value(), field2.statementType.value());
    }
}

Result ParentGraph::getParentT(PKBField field1, PKBField field2) {
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

Result ParentGraph::traverseStartT(PKBField field1, PKBField field2) {
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

void ParentGraph::traverseStartT(std::set<STMT_LO>* found, ParentNode* node, StatementType targetType) {
    ParentNode::NodeSet nextNodes = node->next;
    
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

Result ParentGraph::traverseEndT(PKBField field1, PKBField field2) {
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

void ParentGraph::traverseEndT(std::set<STMT_LO>* found, ParentNode* node, StatementType targetType) {
    while (node->prev) {
        bool typeMatch = node->prev->stmt.type.value() == targetType || targetType == StatementType::All;
        if (typeMatch) {
            found->insert(node->prev->stmt);
        }
        node = node->prev;
    }
}

Result ParentGraph::traverseAllT(StatementType type1, StatementType type2) {
    std::set<STMT_LO> found;
    Result res;

    for (auto const& [stmt_lo, node] : nodes) {
        ParentNode* curr = node;
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

Result ParentGraph::traverseAll(StatementType type1, StatementType type2) {
    Result res{};
    for (auto const& [stmt_lo, node] : nodes) {
        ParentNode* curr = node;
        bool typeMatch1 = curr->stmt.type.value() == type1 || type1 == StatementType::All;

        if (!curr->next.empty() && typeMatch1) {
            ParentNode::NodeSet nextNodes = curr->next;

            // Filter nodes that match second statement type
            ParentNode::NodeSet filtered;
            std::copy_if(nextNodes.begin(), nextNodes.end(), std::back_inserter(filtered),
                [type2](ParentNode* const& node) {
                    return node->stmt.type.value() == type2 || type2 == StatementType::All;
                });

            // Populate result
            std::transform(filtered.begin(), filtered.end(), std::insert_iterator<Result>(res, res.end()),
                [curr](ParentNode* const& node) {
                    PKBField first = PKBField::createConcrete(Content{ curr->stmt });
                    PKBField second = PKBField::createConcrete(Content{ node->stmt });
                    return std::vector<PKBField>{first, second};
                });
        }
    }
    return res;
}
