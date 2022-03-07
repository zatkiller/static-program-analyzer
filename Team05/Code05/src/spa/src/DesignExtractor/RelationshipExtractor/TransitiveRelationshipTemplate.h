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


/**
 * The base abstract template class that handles transitive properties of relationships.
 * 
 * The transitive relationship is derived by cascading up the container stack built from AST DFS traversal.
 * 
 * Call statements can be considered a deferred container for an external procedure. However, due to the limitation
 * of the tree walking implementation, such deferred result cannot be captured naturally. Therefore, a preprocessing 
 * is required to extract procedures in topological order for correct extractions.
 *  
 * Currently implemented by UsesExtractor and ModifiesExtractor.
 */
class TransitiveRelationshipTemplate : public RelExtractorTemplate {
public:
    using RelExtractorTemplate::RelExtractorTemplate;
    void visit(const ast::Call&) override;
    /**
     * @brief Preprocess the given AST to create a topological order of procedure to be extracted.
     * 
     * Extracting by the topological order of procedures, the variables of all procedures will be extracted before
     * they are called by other procedures.
     * 
     * @param node the starting node for extraction.
     */
    void extract(ast::ASTNode *node) override;

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
