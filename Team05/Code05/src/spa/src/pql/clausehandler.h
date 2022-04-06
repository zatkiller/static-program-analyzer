#pragma once

#include <memory>
#include <type_traits>
#include "query.h"
#include "optimizer.h"
#include "resulttable.h"
#include "pkbtypematcher.h"
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
     * Handles all the clauses with synonyms.
     *
     * @param clause a relationship clauses with synonyms
     */
    void handleSynRelRef(std::shared_ptr<query::RelRef> clause);

    /**
     * Handles all the clauses without synonyms.
     *
     * @param noSynClause a relationship clauses without synonyms
     */
    bool handleNoSynRelRef(const std::shared_ptr<query::RelRef>& noSynClause);

    /**
     * Handles pattern clauses
     *
     * @param patterns a group of pattern clauses
     */
    void handlePattern(query::Pattern pattern);

    bool handleNoAttrRefWith(const query::AttrCompare& noAttrClause);

    void handleAttrRefWith(query::AttrCompare attrClause);

    void handleTwoAttrRef(query::AttrRef lhs, query::AttrRef rhs);

    void handleOneAttrRef(query::AttrRef attr, query::AttrCompareRef concrete);

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

    void  handleResultCl(query::ResultCl resultCl);

    bool handleNoSynGroup(optimizer::ClauseGroup group);

    bool handleGroup(optimizer::ClauseGroup group);
};
}  // namespace qps::evaluator
