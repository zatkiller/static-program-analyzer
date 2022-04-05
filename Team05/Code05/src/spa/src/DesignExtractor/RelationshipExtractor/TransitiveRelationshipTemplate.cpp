#include "DesignExtractor/EntityExtractor/EntityExtractor.h"
#include "TransitiveRelationshipTemplate.h"

namespace sp {
namespace design_extractor {
/**
 * Private strategy to only retrieve a local copy of variables.
 */
class VariablePKBStrategy : public NullPKBStrategy {
public:
    std::set<VAR_NAME> variables;
    void insertEntity(Content entity) override {
        try {
            variables.insert(std::get<VAR_NAME>(entity));
        } catch (std::bad_variant_access &ex) {
            Logger(Level::ERROR) << "Bad variant access in transitive relationship template extraction";
        }
    };
};

struct CallGraphPreProcessor {
    using AdjacencyList = std::map<std::string, std::vector<std::string>>;
    std::vector<std::string> topolst;
    std::map<std::string, const ast::Procedure*> procMap;
    struct CallGraphWalker : public TreeWalker {
        std::string currentProc = "";
        AdjacencyList callGraph;
        std::map<std::string, const ast::Procedure*> procMap;

        void visit(const ast::Procedure& node) {
            // unified starting point 1 which is a procedure name that cannot exist.
            callGraph["1"].push_back(node.getName());
            currentProc = node.getName();
            procMap[currentProc] = &node;
        }

        void visit(const ast::Call& node) {
            callGraph[currentProc].push_back(node.getName());
        }
    };


    void processReverseTopoOrder(
        std::string node, 
        const AdjacencyList& lst, 
        std::map<std::string, bool>& visited, 
        std::vector<std::string>& topolst
    ) {
        if (visited[node]) {
            return;
        }
        visited.insert_or_assign(node, true);

        if (lst.find(node) != lst.end()) {
            for (auto m : lst.at(node)) {
                processReverseTopoOrder(m, lst, visited, topolst);
            }
        }

        topolst.push_back(node);
    }

    void preprocess(const ast::ASTNode *node) {
        CallGraphWalker cgw;
        node->accept(&cgw);
        std::map<std::string, bool> visited;
        processReverseTopoOrder("1", cgw.callGraph, visited, topolst);

        // remove the place holder 
        topolst.pop_back();

        procMap = cgw.procMap;
    }
};


std::set<VAR_NAME> RelExtractorTemplate::extractVars(const ast::ASTNode *part) {
    VariablePKBStrategy vps;
    VariableExtractorModule vem(&vps);
    vem.extract(part);

    return vps.variables;
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
