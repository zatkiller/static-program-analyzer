#pragma once

#include <memory>
#include <type_traits>
#include "Query.h"
#include "Optimizer.h"
#include "ResultTable.h"
#include "PKBTypeMatcher.h"
#include "PKB/PKBCommons.h"
#include "PKB/PKBField.h"
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

    /** Constructor of the ClauseHandler */
    ClauseHandler(PKB *pkb, ResultTable &tableRef) : pkb(pkb), tableRef(tableRef) {}

    /**
     * Retrieves all the results of a certain design entity from PKB database.
     *
     * @param type the design entity type to retrieve
     * @return PKBResponse contains all results of the design entity type
     */
    PKBResponse getAll(query::DesignEntity type);

    /**
     * Selects the corresponding values of synonyms in the PKBResponse.
     *
     * @param response PKBResponse
     * @param isFirstDec bool checks whether the first field is a synonym
     * @return new selected PKBResponse
     */
    PKBResponse selectDeclaredValue(PKBResponse &response, bool isFirstSyn);

    /**
     * Filters the correct response from PKB. If two fields in relationship clause have the same synonym name,
     * only return the record when two PKBFields are the same.
     *
     * @param response PKBResponse
     */
    void filterPKBResponse(PKBResponse& response);

    /**
     * Handles a RelRef clause with synonyms.
     *
     * @param clause a relationship clause with synonyms
     */
    void handleSynRelRef(std::shared_ptr<query::RelRef> clause);

    /**
     * Handles a RelRef clause without synonyms.
     *
     * @param noSynClause a relationship clause without synonyms
     */
    bool handleNoSynRelRef(const std::shared_ptr<query::RelRef>& noSynClause);

    /**
     * Handles a pattern clause
     *
     * @param patterns a group of pattern clauses
     */
    void handlePattern(query::Pattern pattern);

    /**
     * Handles a with clause without attribute reference
     *
     * @param noAttrClause a with clause without attribute reference
     * @return true if the with clause holds, false otherwise
     */
    bool handleNoAttrRefWith(const query::AttrCompare& noAttrClause);

    /**
     * Handles a with clause with attribute reference
     *
     * @param attrClause a with clause with attribute reference
     */
    void handleAttrRefWith(query::AttrCompare attrClause);

    /**
     * Handles a with clause whose lhs and rhs are both attRef
     *
     * @param lhs lhs attribute reference
     * @param rhs rhs attribute reference
     */
    void handleTwoAttrRef(query::AttrRef lhs, query::AttrRef rhs);

    /**
     * Handles a with clause only contains one attRef
     * @param attr attribute reference
     * @param concrete string or int value of a parameter of the with clause
     */
    void handleOneAttrRef(query::AttrRef attr, query::AttrCompareRef concrete);

    /**
     * Retrieves the attribute value from a PKBField
     *
     * @tparam T type of the attribute value
     * @param field pkbField
     * @return attribute value
     */
    template<typename T>
    static T getPKBFieldAttr(PKBField field) {
        T value;
        if constexpr(std::is_same_v<T, std::string>) {
            if (auto pptr = field.getContent<PROC_NAME>()) value = pptr->name;
            else if (auto vptr = field.getContent<VAR_NAME>()) value = vptr->name;
            else if (auto sptr = field.getContent<STMT_LO>()) value = sptr->attribute.value();
        } else {
            if (auto cptr = field.getContent<CONST>()) value = *cptr;
            if (auto sptr = field.getContent<STMT_LO>()) value = sptr->statementNum;
        }
        return value;
    }

    /**
     * Filters the PKBResponse so that the attribute value in PKBResponse is equal to value
     *
     * @tparam T type of the value
     * @param response PKBResponse
     * @param value value to compare with
     * @return filtered PKBResponse
     */
    template<typename T>
    PKBResponse filterAttrValue(PKBResponse response, T value) {
        PKBResponse newResponse;
        std::unordered_set<PKBField, PKBFieldHash> res;
        auto *ptr = std::get_if<SingleResponse>(&response.res);

        for (auto v : *ptr) {
            T checkValue = getPKBFieldAttr<T>(v);
            if (checkValue == value) res.insert(v);
        }
        newResponse = PKBResponse{response.hasResult, Response {res}};
        return newResponse;
    }

    /**
     * Merges two PKBResponse by the same attribute value
     * @tparam T type of the attribute value
     * @param lhsResponse PKBResponse of lhs param in with clause
     * @param rhsResponse PKBResponse of rhs param in with clause
     * @return merged PKBResponse
     */
    template<typename T>
    PKBResponse twoAttrMerge(SingleResponse &lhsResponse, SingleResponse &rhsResponse) {
        VectorResponse res;
        for (auto lhsRecord : lhsResponse) {
            T lhsValue = getPKBFieldAttr<T>(lhsRecord);
            for (auto rhsRecord : rhsResponse) {
                T rhsValue = getPKBFieldAttr<T>(rhsRecord);
                if (lhsValue == rhsValue) res.insert(std::vector<PKBField>{lhsRecord, rhsRecord});
            }
        }
        bool hasResult = !res.empty();
        return PKBResponse{hasResult, Response{res}};
    }

    /**
     * Handles the ResultCl in query ADT
     *
     * @param resultCl
     */
    void  handleResultCl(query::ResultCl resultCl);

    /**
     * Handles a group of clause without synonyms
     *
     * @param group the group of clause without synonyms
     * @return true if every clause inside the group holds, false otherwise
     */
    bool handleNoSynGroup(optimizer::ClauseGroup group);

    /**
     * Handles a group of clause with synonyms
     *
     * @param group the group of clause with synonyms
     * @return true if every clause inside the group has result, false otherwise
     */
    bool handleGroup(optimizer::ClauseGroup group);
};
}  // namespace qps::evaluator
