#pragma once

#include <string>
#include <vector>
#include <iterator>
#include <unordered_set>
#include <unordered_map>

#include "PKB/PKBResponse.h"

namespace qps::evaluator {
using VectorResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;
using SingleResponse = std::unordered_set<PKBField, PKBFieldHash>;
using Table = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

/**
 * A data structure to store the query result from PKB.
 */
class ResultTable {
private:
    std::unordered_map<std::string, int> synSequenceMap;
    Table table;

public:
    /**
     * A data structure to represent the necessary parameters to perform an inner join.
     * isFrist and isSecond represent whether the first synonyns or the second synonyms appears in the
     * current resultTable.
     */
    struct InnerJoinParam {
        std::vector<std::string> syns;
        bool isFirst;
        bool isSecond;
        PKBResponse response;
        VectorResponse vectorResponse;
    };

    /**
     * Checks whether a synonym exists in current result table.
     *
     * @param name the synonym name
     * @return whether the synonyms exists in the table.
     */
    bool synExists(std::string name);

    /**
     * Retrieves the synonym map which contains the mapping information between synonym name and it's location.
     *
     * @return the synonym map
     */
    std::unordered_map<std::string, int> getSynMap();

    /**
     * Retrieves the synonym column number in the table.
     *
     * @param synonym name of the synonym to search
     * @return the column number of the synonym
     */
    int getSynLocation(std::string synonym);

    /**
     * Allocates the last column of the table to the new synonym
     *
     * @param name the synonym name
     */
    void insertSynLocationToLast(std::string name);

    /**
     * Retrieves the entire result table.
     *
     * @return the result table
     */
    Table getResult();

    /**
     * @return whether the result table is empty.
     */
    bool isEmpty();

    /**
     * Transfers the PKBResponse in a type of set<PKBField> into set<vector<PKBField>>.
     *
     * @param response the PKBResponse in set<PKBField> format
     * @return the transferred PKBResponse in set<vector<PKBField>> format
     */
    VectorResponse transToVectorResponse(SingleResponse response);

    /**
     * Inserts the PKBResponse to the result table when the table is empty.
     *
     * @param r the PKBResponse from PKB side
     */
    void insert(PKBResponse r);

    /**
     * If there is one synonym in clause and it already appears in the evaluated clauses, inner join the result
     * to the table with one synonym.
     *
     * @param params the parameters to perform an inner join
     * @param newTable store the correct result filtered from the original result table
     */
    void oneSynInnerJoin(InnerJoinParam params, Table& newTable);

    /**
     * If there are two synonyms in the clause and two of them already appears in evaluated clauses,
     * inner join the result to the table with two synonyms.
     *
     * @param params the parameters to perform an inner join
     * @param newTable store the correct result filtered from the original result table
     */
    void twoSynInnerJoinTwo(InnerJoinParam params, Table& newTable);



    /**
     * If there are two synonyms in the clause but only one of the synonyms has appeared in evaluated clauses,
     * inner join the result to the table with two one synonym but insert the value of the second synonym into the
     * new result table.
     *
     * @param params the parameters to perform an inner join
     * @param newTable store the correct result filtered from the original result table
     */
    void twoSynInnerJoinOne(InnerJoinParam params, Table& newTable);

    /**
     * CrossJoins the PKBResponse to the current response table when the synonyms of the result are different from
     * all synonyms in the table.
     *
     * @param r the PKBResponse
     */
    void crossJoin(PKBResponse r);

    /**
     * InnerJoins the PKBResponse to the current response table if table already contains the synonyms in the response.
     *
     * @param params the parameters to perform an inner join
     */
    void innerJoin(InnerJoinParam params);

    /**
     * Joins the PKBResponse to the result table using either crossJoin or innerJoin.
     *
     * @param response the PKBResponse
     * @param synonyms the list of all synonyms from the query
     */
    void join(PKBResponse response, std::vector<std::string> synonyms);
};
}  // namespace qps::evaluator
