#pragma once

#include <set>
#include <string>
#include <deque>
#include <map>

#include "DesignExtractor/Extractor.h"
#include "PKB/PKBField.h"
#include "PKB/PKBCommons.h"
#include "Parser/AST.h"


namespace sp {
namespace design_extractor {
class RelExtractorTemplate : public Extractor {
public:
    using Extractor::Extractor;
protected:
    virtual void extractAndInsert(Content, const ast::ASTNode*) = 0;
    virtual void insert(Content, Content) = 0;
    std::set<VAR_NAME> extract(const ast::ASTNode*);
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

protected:
    std::map<int, StatementType> stmtNumToType;
    void cascadeToContainer(const std::set<VAR_NAME>);
    void extractAndInsert(Content, const ast::ASTNode*) override;
private:
    std::deque<STMT_LO> container;
    PROC_NAME currentProcedure = PROC_NAME{ "" };

    void enterContainer(std::variant<int, std::string>) override;
    void exitContainer() override;
};
}  // namespace design_extractor
}  // namespace sp
