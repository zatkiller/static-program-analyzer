#pragma once

#include <list>
#include "resulttable.h"
#include "query.h"
#include "clausehandler.h"

namespace qps::evaluator {
/**
 * A struct contains the information of a element in result clause.
 */
struct SelectElemInfo {
    int columnNo;
    query::AttrName attrName;
    bool isAttr;

    static SelectElemInfo ofDeclaration(int columnNo);
    static SelectElemInfo ofAttr(int columnNo, query::AttrName attrName);
};

class ResultProjector {
public:
    /**
     * Wraps a certain PKBField into a string representation.
     *
     * @param pkbField the PKBField to wrap
     * @return string representation of the PKBField
     */
    static std::string PKBFieldToString(PKBField pkbField);

    /**
     * Wraps a PKBField with attribute into a string representation.
     * @param pkbField
     * @param attrName name of the attribute
     * @return string representation of the attribute
     */
    static std::string PKBFieldAttrToString(PKBField pkbField, query::AttrName attrName);

    /**
     * Projects the final result from the result table.
     *
     * @param table the reference the result table
     * @param variable the synonym in select part of the query
     * @return the list of string representation of the query result
     */
    static std::list<std::string> projectResult(ResultTable &table, query::ResultCl resultCl);
};
}  // namespace qps::evaluator
