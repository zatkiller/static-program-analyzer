#include "FollowsGraph.h"
#include <iterator>

void FollowsGraph::addEdge(STMT_LO u, STMT_LO v) {
    FollowsNode* uNode;
    FollowsNode* vNode;

    if (u.statementNum == v.statementNum) return; 
    if (u.statementNum > v.statementNum) return;

    // should have at most 2 
    std::map<STMT_LO, FollowsNode*> filtered;
    std::copy_if(nodes.begin(), nodes.end(), std::inserter(filtered, filtered.end()),
        [u, v](decltype(nodes)::value_type const& pair) {
            return pair.first.statementNum == u.statementNum || pair.first.statementNum == v.statementNum; 
        });

    bool hasExistingStatementNumber = false;
    for (auto [stmt, node] : filtered) {
        if (stmt == u) {
            uNode = nodes.at(u);
            hasExistingStatementNumber = true;
            break;
        }

        if (stmt.statementNum == u.statementNum) {
            if (stmt.type.value() != u.type.value()) {
                return;
            }
        }
    }

    // if there exists a node with the same statement number as u but different type,
    // it is an invalid insert. 
    if (!hasExistingStatementNumber) {
        uNode = new FollowsNode(u, nullptr, nullptr);
        nodes.emplace(u, uNode);
    } 

    hasExistingStatementNumber = false;
    for (auto [stmt, node] : filtered) {
        if (stmt == v) {
            vNode = nodes.at(v);
            hasExistingStatementNumber = true;
            break;
        }

        if (stmt.statementNum == v.statementNum) {
            if (stmt.type.value() != v.type.value()) {
                return;
            }
        }
    }

    // if there exists a node with the same statement number as u but different type,
    // it is an invalid insert. 
    if (!hasExistingStatementNumber) {
        vNode = new FollowsNode(v, nullptr, nullptr);
        nodes.emplace(v, vNode);
    }

    if (!uNode->next) uNode->next = vNode;
    if (!vNode->next) vNode->prev = uNode;
}

bool FollowsGraph::getContains(PKBField field1, PKBField field2) {
    STMT_LO stmt = *(field1.getContent<STMT_LO>());
    STMT_LO target = *(field2.getContent<STMT_LO>());
    if (nodes.count(stmt) != 0) {
        FollowsNode* curr = nodes.at(stmt);
        return curr->next && curr->next->stmt == target;
    }
    return false;
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
        traverseStartT(&stmtSet, nodes.at(s));

        for (auto stmt : stmtSet) {
            if (stmt.type.value() == type || type == StatementType::All) {
                res.insert(std::vector<PKBField>{field1, PKBField::createConcrete(stmt)});
            }
        }
    }

    return res;
}

void FollowsGraph::traverseStartT(std::set<STMT_LO>* stmtSetPtr, FollowsNode* node) {
    while (node->next) {
        stmtSetPtr->insert(node->next->stmt);
        node = node->next;
    }
}

Result FollowsGraph::traverseEndT(PKBField field1, PKBField field2) {
    std::set<STMT_LO> stmtSet;
    Result res;
    StatementType type = field1.statementType.value();
    STMT_LO s = *field2.getContent<STMT_LO>();

    if (nodes.count(s) != 0) {
        traverseEndT(&stmtSet, nodes.at(s));

        for (auto stmt : stmtSet) {
            if (stmt.type.value() == type || type == StatementType::All) {
                res.insert(std::vector<PKBField>{PKBField::createConcrete(stmt), field2});
            }
        }
    }

    return res;
}

void FollowsGraph::traverseEndT(std::set<STMT_LO>* stmtSetPtr, FollowsNode* node) {
    while (node->prev) {
        stmtSetPtr->insert(node->prev->stmt);
        node = node->prev;
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

Result FollowsGraph::traverseAllT(StatementType type1, StatementType type2) {
    std::set<STMT_LO> stmtSet;
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> res;

    for (auto const& [stmt_lo, node] : nodes) {
        stmtSet.clear();

        if ((type1 == StatementType::All || node->stmt.type.value() == type1)) {
            traverseStartT(&stmtSet, node);

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
