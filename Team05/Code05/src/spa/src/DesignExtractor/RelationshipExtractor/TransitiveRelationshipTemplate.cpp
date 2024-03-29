#include "DesignExtractor/CallGraph.h"
#include "DesignExtractor/EntityExtractor/EntityExtractor.h"
#include "TransitiveRelationshipTemplate.h"

namespace sp {
namespace design_extractor {

struct CallGraphPreProcessor {
    std::vector<std::string> topolst;
    std::unordered_map<std::string, const ast::Procedure*> procMap;
    void processReverseTopoOrder(
        std::string node, 
        const CallGraph::AdjacencyList& lst,
        std::map<std::string, bool>& visited
    ) {
        if (visited[node]) {
            return;
        }
        visited.insert_or_assign(node, true);

        if (lst.find(node) != lst.end()) {
            for (auto m : lst.at(node)) {
                processReverseTopoOrder(m, lst, visited);
            }
        }

        topolst.push_back(node);
    }

    void preprocess(const ast::ASTNode *node) {
        CallGraph grapher(node);
        
        std::map<std::string, bool> visited;
        processReverseTopoOrder("1", grapher.getCallGraph(), visited);

        // remove the place holder 
        topolst.pop_back();

        procMap = grapher.getProcMap();
    }
};


std::set<VAR_NAME> RelExtractorTemplate::extractVars(const ast::ASTNode *part) {
    EntityExtractor<VariableCollector> ve;
    auto results = ve.extract(part);
    std::set<VAR_NAME> vars;
    std::transform(
        results.begin(),
        results.end(), 
        std::inserter(vars, vars.begin()),
        [](auto entry) {
            return std::get<VAR_NAME>(std::get<Entity>(entry));
        }
    );
    return vars;
}

void TransitiveRelationshipTemplate::visit(const ast::Call &node) {
    auto proc = PROC_NAME{node.getName()};
    try {
        auto vars = procVarMap.at(proc);
        for (auto var : vars) {
            insert(STMT_LO{node.getStmtNo(), StatementType::Call}, var);
        }
        cascadeToContainer(vars);
    } catch (std::out_of_range &ex) {
        // if out of range exception thrown by at, that means something wrong with topo ordering.
        Logger(Level::ERROR) << "TransitiveRelationshipTemplate.cpp " << 
        "empty proc at target " << proc.name << " something wrong with topo order";
    }
}

void TransitiveRelationshipTemplate::extract(const ast::ASTNode *node) {
    CallGraphPreProcessor cgpp;
    cgpp.preprocess(node);
    for (auto proc : cgpp.topolst) {
        cgpp.procMap[proc]->accept(this);
    }
}

void TransitiveRelationshipTemplate::cascadeToContainer(const std::set<VAR_NAME> varNames) {
    for (auto stmt : container) {
        for (auto var : varNames) {
            insert(stmt, var);
        }
    }
    if (!currentProcedure.name.empty()) {
        for (auto var : varNames) {
            insert(currentProcedure, var);
        }
        procVarMap[currentProcedure].insert(varNames.begin(), varNames.end());
    }
}

void TransitiveRelationshipTemplate::extractAndInsert(Content a1, const ast::ASTNode *part) {
    auto vars = extractVars(part);
    for (auto var : vars) {
        insert(a1, var);
    }
    cascadeToContainer(vars);
}

void TransitiveRelationshipTemplate::enterContainer(std::variant<int, std::string> containerId) {
    int* pNum = std::get_if<int>(&containerId);
    if (pNum) {
        container.push_front(STMT_LO{ *pNum, stmtNumToType[*pNum] });
    } else {
        currentProcedure = PROC_NAME{ std::get<std::string>(containerId) };
    }
}
void TransitiveRelationshipTemplate::exitContainer() {
    container.pop_front();
}
}  // namespace design_extractor
}  // namespace sp
