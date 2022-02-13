#pragma once

#include <map>
#include <list>
#include <set>

class FollowsGraph {
public:
    void addEdge(STMT_LO u, STMT_LO v) {
        auto it = adjList.find(u);

        if (it != adjList.end()) {
            std::set<STMT_LO>* uSet = &(it->second);
            uSet->insert(v);
        }
        else {
            adjList.emplace(u, std::set<STMT_LO>{v});
        }
    }

private:
    std::map<STMT_LO, std::set<STMT_LO>> adjList;
};

