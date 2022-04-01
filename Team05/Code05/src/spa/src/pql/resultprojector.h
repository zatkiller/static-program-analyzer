#pragma once

#include <list>
#include "resulttable.h"
#include "query.h"
#include "clausehandler.h"

namespace qps::evaluator {
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
     * Wraps a certain PKBFiend into a string representation.
     *
     * @param pkbField the PKBField to wrap
     * @return string representation of the PKBField
     */
    static std::string PKBFieldToString(PKBField pkbField);

    static std::string PKBFieldAttrToString(PKBField pkbField, query::AttrName attrName);

    /**
     * Retrieves the final result from the result table.
     *
     * @param table the reference the result table
     * @param variable the synonym in select part of the query
     * @return the list of string representation of the query result
     */
    static std::list<std::string> projectResult(ResultTable &table, query::ResultCl resultCl);
};
}  // namespace qps::evaluator
