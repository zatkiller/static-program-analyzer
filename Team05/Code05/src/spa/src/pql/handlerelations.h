#include <memory>
#include "query.h"
#include "resulttable.h"
#include "PKB/StatementType.h"
#include "PKB/PKBField.h"
#include "PKB/PKBRelationship.h"
#include "PKB/PKBResponse.h"
#include "PKB.h"

class ClauseHandler {
public:
    PKB* pkb;
    ResultTable& tableRef;
    Query& query;

    ClauseHandler(PKB* pkb, ResultTable& tableRef, Query& query) : pkb(pkb), tableRef(tableRef), query(query) {};

    StatementType getStatementType(DesignEntity);
    void handleModifies(Modifies*);
    void handleUses(Uses*);
    void handleFollows(Follows*);
    void handleFollowsT(FollowsT*);
    void handleParent(Parent*);
    void handleParentT(ParentT*);
    void handleSynClauses(std::vector<std::shared_ptr<RelRef>>);
    bool evaluateNoSynClauses(std::vector<std::shared_ptr<RelRef>>);
};