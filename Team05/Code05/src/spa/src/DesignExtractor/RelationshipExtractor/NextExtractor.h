#pragma once

#include <set>
#include <unordered_set>
#include <queue>
#include <map>

#include "DesignExtractor/Extractor.h"
#include "DesignExtractor/CFG/CFG.h"

namespace sp {
namespace design_extractor {
/**
 * Extracts all Next relationship from the CFG and return them as a set of entries
 */
struct NextExtractor : public Extractor<const cfg::PROC_CFG_MAP*> {
    std::set<Entry> extractOne(cfg::CFGNode *node) {
        // a little helper to check if a node has been visited before.
        std::unordered_set<cfg::CFGNode*> visited;
        auto hasVisited = [&visited](auto node) {
            return visited.find(node) != visited.end();
        };

        // helper to turn insert nodes as entries.
        std::set<Entry> entries;
        auto collect = [&entries](cfg::CFGNode *n1, cfg::CFGNode *n2) {
            if (n1->stmt.has_value() && n2->stmt.has_value()) {
                auto s1 = n1->stmt.value();
                auto s2 = n2->stmt.value();
                entries.insert(Relationship(PKBRelationship::NEXT, s1, s2));
            }
        };

        std::queue<cfg::CFGNode*> queue;  // bfs queue
        queue.push(node);
        while(!queue.empty()) {
            auto curNode = queue.front();
            queue.pop();

            for (auto child : curNode->getChildren()) {
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

                // if this child is a dummy node,
                // extract the dummy node's children instead.
                if (!child->stmt.has_value()) {
                    for (auto realChild : child->getChildren()) {
                        collect(curNode, realChild.get());
                    }
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
 * Extracts all Next relationship from the CFG and send them to the PKBStrategy
 */
struct NextExtractorModule : public ExtractorModule<const cfg::PROC_CFG_MAP*> {
public:
    explicit NextExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<NextExtractor>(), pkb) {}
};

}  // namespace design_extractor
}  // namespace sp
