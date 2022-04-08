#pragma once

#include <set>
#include <unordered_set>
#include <queue>
#include <map>

#include "DesignExtractor/CFG/CFG.h"
#include "DesignExtractor/Extractor.h"

namespace sp {
namespace design_extractor {
/**
 * Extracts all Next relationship from the CFG and return them as a set of entries
 */
struct NextExtractor : public Extractor<const cfg::PROC_CFG_MAP*> {
private:
    std::set<Entry> entries;

    /**
     * @brief helper function to insert 2 cfg nodes as a next relationship entry. Next(n1, n2).
     * 
     * @param n1 the LHS of the Next relationship
     * @param n2 the  RHS of the Next relationship
     */
    void collect(cfg::CFGNode *n1, cfg::CFGNode *n2) {
        if (n1->stmt.has_value() && n2->stmt.has_value()) {
            auto s1 = n1->stmt.value();
            auto s2 = n2->stmt.value();
            entries.insert(Relationship(PKBRelationship::NEXT, s1, s2));
        }
    }

public:
    /**
     * @brief Extracts the Next relationship from CFG for 1 procedure.
     * 
     * @param node the root of the procedure's CFG.
     * @return std::set<Entry> the set of next relationships extracted
     */
    std::set<Entry> extractOne(cfg::CFGNode *node) {
        // a little helper to check if a node has been visited before.
        std::unordered_set<cfg::CFGNode*> visited;
        auto hasVisited = [&visited](auto node) {
            return visited.find(node) != visited.end();
        };

        std::queue<cfg::CFGNode*> queue;  // bfs queue
        queue.push(node);
        while(!queue.empty()) {
            auto curNode = queue.front();
            queue.pop();

            for (auto c : curNode->getChildren()) {
                auto child = c.lock();
                // standard bfs stuff
                if (!hasVisited(child.get())) {
                    visited.insert(child.get());
                    queue.push(child.get());
                }

                // current node is a dummy node, we just move on.
                if (!curNode->stmt.has_value()) {
                    continue;
                }

                // current node is not a dummy node.

                // if this child is a dummy node, and since dummy node
                // only can have 1 child. Walk the hierarchy until a 
                // real node is found as child.
                if (!child->stmt.has_value() && !child->getChildren().empty()) {
                    auto realChild = child->getChildren()[0].lock();
                    while(!realChild->stmt.has_value() && !realChild->getChildren().empty()) {
                        realChild = realChild->getChildren()[0].lock();
                    }
                    collect(curNode, realChild.get());
                } else {
                    collect(curNode, child.get());
                }
            }
        }
        
        return entries;
    }

    std::set<Entry> extract(const cfg::PROC_CFG_MAP* cfgs) override {
        std::set<Entry> entries;
        for (auto const& [_, cfg] : *cfgs) {
            auto thisOne = extractOne(cfg.get());
            entries.insert(thisOne.begin(), thisOne.end());
        }
        return entries;
    };
};

/**
 * Extracts all Next relationship from the CFG and send them to the PKB
 */
struct NextExtractorModule : public ExtractorModule<const cfg::PROC_CFG_MAP*> {
public:
    explicit NextExtractorModule(PKB *pkb) : 
        ExtractorModule(std::make_unique<NextExtractor>(), pkb) {}
};

}  // namespace design_extractor
}  // namespace sp
