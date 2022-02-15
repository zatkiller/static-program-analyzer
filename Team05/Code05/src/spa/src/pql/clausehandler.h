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
/**
 * A class to handle the relationship clauses.
 */
class ClauseHandler {
public:
    PKB *pkb;
    ResultTable &tableRef;
    query::Query &query;

    /** Constructor of the ClauseHandler */
    ClauseHandler(PKB *pkb, ResultTable &tableRef, query::Query &query) : pkb(pkb), tableRef(tableRef), query(query) {}

    /**
     * Maps the corresponding PKB StatementType from the Design Entity type.
     *
     * @param d a DesignEntity
     * @return StatementType corresponds to the given DesignEntity
     */
    static StatementType getStatementType(query::DesignEntity d);

    /**
     * Wraps a stmtRef into a PKBField.
     *
     * @param stmtRef
     * @return a PKBField of the given stmtRef
     */
    PKBField wrapStmtRef(query::StmtRef stmtRef);

    /**
     * Wraps a entRef into a PKBField.
     *
     * @param entRef
     * @return a PKBField of the given entRef
     */
    PKBField wrapEntRef(query::EntRef entRef);

    /**
     * Evaluates the given relationship clause and joins the results to the result table.
     *
     * @param ptr the relRef pointer
     * @param f1 the memory address of the first RelRef variable
     * @param f2 the memory address of the second RelRef variable
     * @param relationship PKBRelationship type of the current relationship clause
     */
    template<typename T, typename F1, typename F2>
    void evaluateRelationships(T *ptr, F1 (T::*f1), F2 (T::*f2), PKBRelationship relationship) {
        PKBField f1Field;
        PKBField f2Field;
        std::vector<std::string> synonyms;

        // f1 is always stmtRef for now
        query::StmtRef f1Ref = ptr->*f1;
        if (f1Ref.isDeclaration()) {
            synonyms.push_back(f1Ref.getDeclaration());
        }
        f1Field = wrapStmtRef(f1Ref);

        if constexpr(std::is_same_v<F2, query::StmtRef>) {
            query::StmtRef f2Ref = ptr->*f2;
            if (f2Ref.isDeclaration()) {
                synonyms.push_back(f2Ref.getDeclaration());
            }
            f2Field = wrapStmtRef(f2Ref);
            PKBResponse r = pkb->getRelationship(f1Field, f2Field, relationship);
            tableRef.join(r, synonyms);
        } else if constexpr(std::is_same_v<F2, query::EntRef>) {
            query::EntRef f2Ref = ptr->*f2;
            if (f2Ref.isDeclaration()) {
                synonyms.push_back(f2Ref.getDeclaration());
            }
            f2Field = wrapEntRef(f2Ref);
            PKBResponse r = pkb->getRelationship(f1Field, f2Field, relationship);
            tableRef.join(r, synonyms);
        }
    }

    /**
     * Handles all the clauses with synonyms.
     *
     * @param clauses a group of relationship clauses with synonyms
     */
    void handleSynClauses(std::vector<std::shared_ptr<query::RelRef>> clauses);

    /**
     * Handles all the clauses without synonyms.
     *
     * @param noSynClauses a group of relationship clauses without synonyms
     */
    bool handleNoSynClauses(std::vector<std::shared_ptr<query::RelRef>> noSynClauses);

    /**
     * Evaluates the given relationship clause without synonyms. Returns true if the relationship holds else returns false.
     *
     * @param r PKBRelationship type of the current relationship clause
     * @param ptr the relRef pointer
     * @param f1 the memory address of the first RelRef variable
     * @param f2 the memory address of the second RelRef variable
     * @return bool value indicates whether the relationship holds
     */
    template<typename T, typename F1, typename F2>
    bool evaluateNoSynRelRef(PKBRelationship r, T *ptr, F1 (T::*f1), F2 (T::*f2)) {
        PKBField f1Field;
        PKBField f2Field;

        query::StmtRef s = ptr->*f1;
        f1Field = PKBField::createConcrete(STMT_LO{s.getLineNo()});

        if constexpr(std::is_same_v<F1, query::StmtRef>) {
            query::StmtRef s = ptr->*f1;
            f1Field = wrapStmtRef(s);
        }
        if constexpr(std::is_same_v<F2, query::StmtRef>) {
            query::StmtRef s = ptr->*f2;
            f2Field = wrapStmtRef(s);
        } else if constexpr(std::is_same_v<F2, query::EntRef>) {
            query::EntRef e = ptr->*f2;
            f2Field = wrapEntRef(e);
        }

        return pkb->isRelationshipPresent(f1Field, f2Field, r);
    }
};
}  // namespace qps::evaluator
