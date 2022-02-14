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
        return traverseStart(field1, field2);
    } else if (field1.fieldType == PKBFieldType::DECLARATION && field2.fieldType == PKBFieldType::CONCRETE) {
        return traverseEnd(field1, field2);
    } else if (field1.fieldType == PKBFieldType::DECLARATION && field2.fieldType == PKBFieldType::DECLARATION) {
        return traverseAll(field1.statementType.value(), field2.statementType.value());
    } else {
        return Result{};
    }
}

Result FollowsGraph::traverseStart(PKBField field1, PKBField field2) {
    std::set<STMT_LO> stmtSet;
    Result res;
    StatementType type = field2.statementType.value();
    STMT_LO s = *field1.getContent<STMT_LO>();

    if (nodes.count(s) != 0) {
        traverseStart(&stmtSet, type, nodes.at(s));

        for (auto stmt : stmtSet) {
            if (stmt.type.value() == type || type == StatementType::All) {
                res.insert(std::vector<PKBField>{field1, PKBField::createConcrete(stmt)});
            }
        }
    }

    return res;
}

void FollowsGraph::traverseStart(std::set<STMT_LO>* stmtSetPtr, StatementType type, FollowsNode* node) {
    if (node->next) {
        stmtSetPtr->insert(node->next->stmt);
        traverseStart(stmtSetPtr, type, node->next);
    }
}

Result FollowsGraph::traverseEnd(PKBField field1, PKBField field2) {
    std::set<STMT_LO> stmtSet;
    Result res;
    StatementType type = field1.statementType.value();
    STMT_LO s = *field2.getContent<STMT_LO>();

    if (nodes.count(s) != 0) {
        traverseEnd(&stmtSet, type, nodes.at(s));

        for (auto stmt : stmtSet) {
            if (stmt.type.value() == type || type == StatementType::All) {
                res.insert(std::vector<PKBField>{PKBField::createConcrete(stmt), field2});
            }
        }
    }

    return res;
}

void FollowsGraph::traverseEnd(std::set<STMT_LO>* stmtSetPtr, StatementType type, FollowsNode* node) {
    if (node->prev) {
        stmtSetPtr->insert(node->prev->stmt);
        traverseEnd(stmtSetPtr, type, node->prev);
    }
}


Result FollowsGraph::traverseAll(StatementType type1, StatementType type2) {
    std::set<STMT_LO> stmtSet;
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> res;

    for (auto const& [stmt_lo, node] : nodes) {
        stmtSet.clear();

        if ((type1 == StatementType::All || node->stmt.type.value() == type1)) {
            traverseStart(&stmtSet, type2, node);

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

