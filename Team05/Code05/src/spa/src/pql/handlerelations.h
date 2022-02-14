#include <memory>
#include <type_traits>
#include "query.h"
#include "resulttable.h"
#include "PKB/StatementType.h"
#include "PKB/PKBField.h"
#include "PKB/PKBRelationship.h"
#include "PKB/PKBResponse.h"
#include "PKB.h"

namespace qps::evaluator {
    using namespace qps::query;

    class ClauseHandler {
    public:
        PKB *pkb;
        ResultTable &tableRef;
        Query &query;

        ClauseHandler(PKB *pkb, ResultTable &tableRef, Query &query) : pkb(pkb), tableRef(tableRef), query(query) {};

        StatementType getStatementType(DesignEntity);

        PKBField wrapStmtRef(StmtRef);

        PKBField wrapEntRef(EntRef);

        template<typename T, typename F1, typename F2>
        void evaluateRelationships(T *ptr, F1 f1, F2 f2, PKBRelationship relationship) {
            PKBField f1Field;
            PKBField f2Field;
            std::vector<std::string> synonyms;

            //f1 is always stmtRef for now
            StmtRef f1Ref = ptr->*f1;
            if (f1Ref.isDeclaration()) {
                synonyms.push_back(f1Ref.getDeclaration());
            }
            f1Field = wrapStmtRef(f1Ref);

            if constexpr(std::is_same_v<F2, StmtRef>) {
                StmtRef f2Ref = ptr->*f2;
                if (f2Ref.isDeclaration()) {
                    synonyms.push_back(f2Ref.getDeclaration());
                }
                f2Field = wrapStmtRef(f2Ref);
                PKBResponse r = pkb->getRelationship(f1Field, f2Field, relationship);
                tableRef.join(r, synonyms);
            } else if constexpr(std::is_same_v<F2, EntRef>) {
                EntRef f2Ref = ptr->*f2;
                if (f2Ref.isDeclaration()) {
                    synonyms.push_back(f2Ref.getDeclaration());
                }
                f2Field = wrapEntRef(f2Ref);
                PKBResponse r = pkb->getRelationship(f1Field, f2Field, relationship);
                tableRef.join(r, synonyms);
            }
        }

        void handleSynClauses(std::vector<std::shared_ptr<RelRef>>);

        bool handleNoSynClauses(std::vector<std::shared_ptr<RelRef>>);

        template<typename T, typename F1, typename F2>
        bool evaluateNoSynRelRef(PKBRelationship r, T *ptr, F1 f1, F2 f2) {
            PKBField f1Field;
            PKBField f2Field;

            StmtRef s = ptr->*f1;
            f1Field = PKBField::createConcrete(STMT_LO{s.getLineNo()});

            if constexpr(std::is_same_v<F1, StmtRef>) {
                StmtRef s = ptr->*f1;
                f1Field = wrapStmtRef(s);
            }
            if constexpr(std::is_same_v<F2, StmtRef>) {
                StmtRef s = ptr->*f2;
                f2Field = wrapStmtRef(s);
            } else if constexpr(std::is_same_v<F2, EntRef>) {
                EntRef e = ptr->*f2;
                f2Field = wrapEntRef(e);
            }

            return pkb->isRelationshipPresent(f1Field, f2Field, r);
        }
    };
}