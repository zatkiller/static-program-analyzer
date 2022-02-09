#include <memory>
#include "query.h"
#include "resultTable.h"
#include "PKB/StatementType.h"
#include "PKB/PKBField.h"
#include "PKB/PKBRelationship.h"
#include "PKB/PKBResponse.h"
#include "PKB.h"

class ClauseHandler {
public:
    PKB* pkb;
    ResultTable& tableRef;
    ClauseHandler(PKB* pkb, ResultTable& tableRef) : pkb(pkb), tableRef(tableRef) {};

    void handleModifies(Modifies*);
    void handleUses(Uses*);
    void handleSynClauses(std::vector<std::shared_ptr<RelRef>>);
    bool evaluateNoSynClauses(std::vector<std::shared_ptr<RelRef>>);
};