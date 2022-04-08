#include <unordered_map>
#include <vector>
#include <string>

#include "DesignExtractor/CallGraph.h"
#include "Validator.h"

// Reference from https://www.geeksforgeeks.org/detect-cycle-in-a-graph/
bool isCyclicCallsRec(
    std::string node,
    sp::design_extractor::CallGraph::AdjacencyList& graph,
    std::unordered_map<std::string, bool>& visited,
    std::unordered_map<std::string, bool>& onStack
) {
    if (!visited[node]) {
        visited[node] = true;
        onStack[node] = true;
        for(auto newNode : graph[node]) {
            if (!visited[newNode] && isCyclicCallsRec(newNode, graph, visited, onStack)) {
                return true;
            } else if (onStack[newNode]) {
                return true;
            }
        }
    }
    onStack[node] = false;
    return false;
}

bool isCyclicCalls(sp::ast::ASTNode* root) {
    sp::design_extractor::CallGraph grapher(root);
    auto graph = grapher.getCallGraph();

    // Initialising variables
    std::vector<std::string> procedures;
    std::unordered_map<std::string, bool> visited;
    std::unordered_map<std::string, bool> onStack;
    for (auto const& [proc, _] : grapher.getProcMap()) {
        procedures.push_back(proc);
        visited.insert_or_assign(proc, false);
        onStack.insert_or_assign(proc, false);
    }

    
    for (auto const& proc : procedures) {
        if (!visited[proc] && isCyclicCallsRec(proc, graph, visited, onStack)) {
            return true;
        }
    }

    return false;
};
