#include <unordered_map>
#include <vector>
#include <string>

#include "DesignExtractor/CallGraph.h"
#include "Validator.h"


bool isCyclicCalls(sp::ast::ASTNode* root) {
    sp::design_extractor::CallGraph grapher(root);
    auto graph = grapher.getCallGraph();
    return false;
};
