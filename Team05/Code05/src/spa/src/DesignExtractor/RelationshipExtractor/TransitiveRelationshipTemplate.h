#pragma once

#include <set>
#include <string>
#include <deque>
#include <map>

#include "DesignExtractor/Extractor.h"
#include "PKB/PKBDataTypes.h"
#include "PKB/StatementType.h"
#include "Parser/AST.h"


namespace sp {
namespace design_extractor {
class RelExtractorTemplate : public Extractor {
public:
    using Extractor::Extractor;
protected:
    virtual void extractAndInsert(Content, const ast::ASTNode*) = 0;
    virtual void insert(Content, Content) = 0;
    std::set<VAR_NAME> extractVars(const ast::ASTNode*);
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
            if (currentProc == "") {
                // virtual starting point "1"
                callGraph["1"].push_back(node.getName());
            }
            currentProc = node.getName();
            procMap[currentProc] = &node;
        }

        void visit(const ast::Call& node) {
            callGraph[currentProc].push_back(node.getName());
        }
    };


    void topo(std::string node, const AdjacencyList& lst, std::map<std::string, bool>& visited, std::vector<std::string>& topolst) {
        if (visited[node]) {
            return;
        }
        visited.insert_or_assign(node, true);

        if (lst.find(node) != lst.end()) {
            for (auto m : lst.at(node)) {
                topo(m, lst, visited, topolst);
            }
        }

        topolst.push_back(node);
    }

    void preprocess(ast::ASTNode *node) {
        CallGraphWalker cgw;
        node->accept(&cgw);
        std::map<std::string, bool> visited;
        topo("1", cgw.callGraph, visited, topolst);

        // remove the place holder 
        topolst.pop_back();

        procMap = cgw.procMap;
    }
};


/**
 * The base abstract template class that handles transitive properties of relationships.
 * 
 * The transitive relationship is derived by cascading up the container stack built from AST DFS traversal.
 *  
 * Currently implemented by UsesExtractor and ModifiesExtractor.
 */
class TransitiveRelationshipTemplate : public RelExtractorTemplate {
public:
    using RelExtractorTemplate::RelExtractorTemplate;
    void visit(const ast::Call&) override;

    void extract(ast::ASTNode *node) {
        CallGraphPreProcessor cgpp;
        cgpp.preprocess(node);
        for (auto proc : cgpp.topolst) {
            cgpp.procMap[proc]->accept(this);
        }
    }

protected:
    std::map<int, StatementType> stmtNumToType;

    void cascadeToContainer(const std::set<VAR_NAME>);
    void extractAndInsert(Content, const ast::ASTNode*) override;
private:
    std::deque<STMT_LO> container;
    PROC_NAME currentProcedure = PROC_NAME{ "" };
    std::map<PROC_NAME, std::set<VAR_NAME>> procVarMap;

    void enterContainer(std::variant<int, std::string>) override;
    void exitContainer() override;
};

}  // namespace design_extractor
}  // namespace sp
