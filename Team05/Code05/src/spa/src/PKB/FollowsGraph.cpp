#include "FollowsGraph.h"

void FollowsGraph::addEdge(STMT_LO u, STMT_LO v) {
    // handle the case where it is u -> v already present
    // and want to add u -> w

    FollowsNode* uNode;
    FollowsNode* vNode;
    bool isUNodePresent = true;
    bool isVNodePresent = true;

    if (nodes.count(u) == 0) {
        isUNodePresent = false;
        uNode = new FollowsNode(u, nullptr, nullptr);
        nodes.emplace(u, uNode);
    } else {
        uNode = nodes.at(u);
    }

    if (nodes.count(v) == 0) {
        isVNodePresent = false;
        vNode = new FollowsNode(v, nullptr, nullptr);
        nodes.emplace(v, vNode);
    } else {
        vNode = nodes.at(v);
    }

    // for u -> v, prev of v (if any) must be nullptr and next of u (if any) must be nullptr
    uNode->next = vNode;
    vNode->prev = uNode;
}

// wildcard should be converted to Statement::All
Result FollowsGraph::getFollowsT(PKBField field1, PKBField field2) {
    if (field1.fieldType == PKBFieldType::CONCRETE && field2.fieldType == PKBFieldType::DECLARATION) {
        return traverseStartT(field1, field2);
    } else if (field1.fieldType == PKBFieldType::DECLARATION && field2.fieldType == PKBFieldType::CONCRETE) {
        return traverseEndT(field1, field2);
    } else if (field1.fieldType == PKBFieldType::DECLARATION && field2.fieldType == PKBFieldType::DECLARATION) {
        return traverseAllT(field1.statementType.value(), field2.statementType.value());
    } else {
        return Result{};
    }
}

Result FollowsGraph::traverseStartT(PKBField field1, PKBField field2) {
    std::set<STMT_LO> stmtSet;
    Result res;
    StatementType type = field2.statementType.value();
    STMT_LO s = *field1.getContent<STMT_LO>();

    if (nodes.count(s) != 0) {
        traverseStartT(&stmtSet, type, nodes.at(s));

        for (auto stmt : stmtSet) {
            if (stmt.type.value() == type || type == StatementType::All) {
                res.insert(std::vector<PKBField>{field1, PKBField::createConcrete(stmt)});
            }
        }
    }

    return res;
}

void FollowsGraph::traverseStartT(std::set<STMT_LO>* stmtSetPtr, StatementType type, FollowsNode* node) {
    if (node->next) {
        stmtSetPtr->insert(node->next->stmt);
        traverseStartT(stmtSetPtr, type, node->next);
    }
}

Result FollowsGraph::traverseEndT(PKBField field1, PKBField field2) {
    std::set<STMT_LO> stmtSet;
    Result res;
    StatementType type = field1.statementType.value();
    STMT_LO s = *field2.getContent<STMT_LO>();

    if (nodes.count(s) != 0) {
        traverseEndT(&stmtSet, type, nodes.at(s));

        for (auto stmt : stmtSet) {
            if (stmt.type.value() == type || type == StatementType::All) {
                res.insert(std::vector<PKBField>{PKBField::createConcrete(stmt), field2});
            }
        }
    }

    return res;
}

void FollowsGraph::traverseEndT(std::set<STMT_LO>* stmtSetPtr, StatementType type, FollowsNode* node) {
    if (node->prev) {
        stmtSetPtr->insert(node->prev->stmt);
        traverseEndT(stmtSetPtr, type, node->prev);
    }
}

bool FollowsGraph::getContainsT(PKBField field1, PKBField field2) {
    STMT_LO stmt = *(field1.getContent<STMT_LO>());
    STMT_LO target = *(field2.getContent<STMT_LO>());
    if (nodes.count(stmt) != 0) {
        FollowsNode* curr = nodes.at(stmt);
        while (curr->next != nullptr) {
            if (curr->next->stmt == target) {
                return true;
            }
            curr = curr->next;
        }
    }
    return false;
}

Result FollowsGraph::traverseAllT(StatementType type1, StatementType type2) {
    std::set<STMT_LO> stmtSet;
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> res;

    for (auto const& [stmt_lo, node] : nodes) {
        stmtSet.clear();

        if ((type1 == StatementType::All || node->stmt.type.value() == type1)) {
            traverseStartT(&stmtSet, type2, node);

            for (auto stmt : stmtSet) {
                if (type2 == StatementType::All || type2 == stmt.type.value()) {
                    res.insert(std::vector<PKBField>{PKBField::createConcrete(node->stmt), 
                        PKBField::createConcrete(stmt)});
                }
            }
        }
    }

    return res;
}

bool FollowsGraph::getContains(PKBField field1, PKBField field2) {
    STMT_LO stmt = *(field1.getContent<STMT_LO>());
    STMT_LO target = *(field2.getContent<STMT_LO>());
    if (nodes.count(stmt) != 0) {
        FollowsNode* curr = nodes.at(stmt);
        return curr->next->stmt == target;
    }
    return false;
}

Result FollowsGraph::getFollows(PKBField field1, PKBField field2) {
    bool isConcreteFirst = field1.fieldType == PKBFieldType::CONCRETE;
    bool isConcreteSec = field2.fieldType == PKBFieldType::CONCRETE;

    if (isConcreteFirst && isConcreteSec) {
        return this->getContains(field1, field2) ? Result{ {{field1, field2}} } : Result{};

    } else if (isConcreteFirst && !isConcreteSec) {
        STMT_LO stmt = *(field1.getContent<STMT_LO>());
        if (nodes.count(stmt) != 0) {
            FollowsNode* curr = nodes.at(stmt);
            if (curr->next != nullptr) {
                STMT_LO nextStmt = curr->next->stmt;
                StatementType type = field2.statementType.value();
                return (nextStmt.type.value() == type || type == StatementType::All) 
                    ? Result{ {{ field1, PKBField::createConcrete(Content{nextStmt}) }} }
                    : Result{};
            }
        }
        return Result{};

    } else if (!isConcreteFirst && isConcreteSec) {
        STMT_LO stmt = *(field2.getContent<STMT_LO>());
        if (nodes.count(stmt) != 0) {
            FollowsNode* curr = nodes.at(stmt);
            if (curr->prev != nullptr) {
                STMT_LO prevStmt = curr->prev->stmt;
                StatementType type = field1.statementType.value();
                return (prevStmt.type.value() == type || type == StatementType::All)
                    ? Result{ {{ PKBField::createConcrete(Content{prevStmt}), field2 }} }
                    : Result{};
            }
        }
        return Result{};
    } else {
        return traverseAll(field1.statementType.value(), field2.statementType.value());
    }
}

Result FollowsGraph::traverseAll(StatementType type1, StatementType type2) {
    Result res;
    for (auto const& [stmt_lo, node] : nodes) {
        std::vector<PKBField> temp;
        FollowsNode* curr = node;
        if (curr->next != nullptr) {
            STMT_LO currStmt = curr->stmt;
            STMT_LO nextStmt = curr->next->stmt;
            bool typeMatch1 = currStmt.type.value() == type1 || type1 == StatementType::All;
            bool typeMatch2 = nextStmt.type.value() == type2 || type2 == StatementType::All;
            if (typeMatch1 && typeMatch2) {
                temp.push_back(PKBField::createConcrete(Content{ currStmt }));
                temp.push_back(PKBField::createConcrete(Content{ nextStmt }));
                res.insert(temp);
            }
        }
    }
    return res;
}
