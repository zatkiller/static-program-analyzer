#include <memory>
#include "query.h"
#include "resultTable.h"
#include "PKB/StatementType.h"
#include "PKB/PKBField.h"
#include "PKB/PKBRelationship.h"
#include "PKB/PKBResponse.h"
#include "PKB.h"

class RelationshipHandler {
public:
    PKB* pkb;
    ResultTable& tableRef;
    RelationshipHandler(PKB* pkb, ResultTable& tableRef) : pkb(pkb), tableRef(tableRef) {};

    bool handleModifies(Modifies*);
    void handleSynClauses(std::vector<std::shared_ptr<RelRef>>);
};