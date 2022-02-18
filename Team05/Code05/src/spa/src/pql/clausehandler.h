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
     * Maps PKBRelationships based on RelRef type.
     *
     * @param r relRefType
     * @return PKBRelationship
     */
    PKBRelationship getPKBRelationship(query::RelRefType r);

    /**
     * Maps DesignEntity to corresponding PKB StatementType.
     *
     * @param d DesignEntity
     * @return StatementType
     */
    static StatementType getStatementType(query::DesignEntity d);

    /**
     * Processes statement declarations, create PKBField with StatementType.
     *
     * @param fields PKBFields returned from RelRef
     * @param synonyms all of the synonyms in RelRef
     */
    void processStmtField(std::vector<PKBField> &fields, std::vector<std::string> synonyms);

    /**
     * Selects the corresponding values of synonyms in the PKBResponse.
     *
     * @param response PKBResponse
     * @param isFirstDec bool checks whether the first field is a synonym
     * @return new selected PKBResponse
     */
    PKBResponse selectDeclarationValue(PKBResponse &response, bool isFirstSyn);

    /**
     * Filters the correct response from PKB. If two fields in relationship clause have the same synonym name,
     * only return the record when two PKBFields are the same.
     *
     * @param response PKBResponse
     * @param isSame bool checks whether two synonyms are the same
     * @return filtered PKBResponse
     */
    PKBResponse filterPKBResponse(PKBResponse& response, bool isSame);

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
};
}  // namespace qps::evaluator
