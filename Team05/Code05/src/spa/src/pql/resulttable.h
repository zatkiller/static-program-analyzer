#pragma once

#include <string>
#include <vector>
#include <iterator>
#include <unordered_set>
#include <unordered_map>

#include "PKB/PKBResponse.h"

namespace qps::evaluator {
/**
 * A data structure to store the query result from PKB.
 */
class ResultTable {
private:
    std::unordered_map<std::string, int> synSequenceMap;
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> table;

public:
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
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> getResult();

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
     * @param synName synonym in clause
     * @param queryRes the query result from PKB
     * @param newTable store the correct result filtered from the original result table
     */
    void oneSynInnerJoin(std::string synName, std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> queryRes,
                         std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>& newTable);

    /**
     * If there are two synonyms in the clause and two of them already appears in evaluated clauses,
     * inner join the result to the table with two synonyms.
     *
     * @param syn1 first synonym in clause
     * @param syn2 second synonym in clause
     * @param queryRes the query result from PKB
     * @param newTable store the correct result filtered from the original result table
     */
    void twoSynInnerJoinTwo(std::string syn1, std::string syn2,
                         std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> queryRes,
                         std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>& newTable);



    /**
     * If there are two synonyms in the clause but only one of the synonyms has appeared in evaluated clauses,
     * inner join the result to the table with two one synonym but insert the value of the second synonym into the
     * new result table.
     *
     * @param syn1 first synonym in clause
     * @param syn2 second synonym in clause
     * @param isFirst bool value indicates whether the first synonym is already inside the result table
     * @param queryRes the query result from PKB
     * @param newTable store the correct result filtered from the original result table
     */
    void twoSynInnerJoinOne(std::string syn1, std::string syn2, bool isFirst,
                            std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> queryRes,
                            std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>& newTable);

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
     * @param r the PKBResponse
     * @param isFirst the bool value indicates whether the first synonym is in the result table
     * @param isSecond the bool value indicates whether the second synonym is in the result table
     * @param allSyn the list of all synonyms in the query
     */
    void innerJoin(PKBResponse r, bool isFirst, bool isSecond, std::vector<std::string> allSyn);

    /**
     * Joins the PKBResponse to the result table using either crossJoin or innerJoin.
     *
     * @param response the PKBResponse
     * @param synonyms the list of all synonyms from the query
     */
    void join(PKBResponse response, std::vector<std::string> synonyms);
};
}  // namespace qps::evaluator