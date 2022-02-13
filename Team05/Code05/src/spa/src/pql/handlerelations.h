#include <memory>
#include <type_traits>
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

    template<typename T, typename F1, typename F2>
    bool evaluateNoSynRelRef(PKBRelationship r, T* ptr, F1 f1, F2 f2) {
        PKBField f1Field;
        PKBField f2Field;

        if (std::is_same_v<F1, StmtRef>) {
            StmtRef s = ptr->*f1;
            f1Field = PKBField::createConcrete(STMT_LO{s.getLineNo()});
        }
        if (std::is_same<F2, StmtRef>) {
            StmtRef s = ptr->*f2;
            f2Field = PKBField::createConcrete(STMT_LO{ptr->*f2.getLineNo()});
        } else if (std::is_same<F2, EntRef>) {
            EntRef e = ptr->*f2;
            f2Field = PKBField::createConcrete(VAR_NAME{e.getVariableName()});
        }

        return pkb->isRelationshipPresent(f1Field, f2Field, r);
    }
};