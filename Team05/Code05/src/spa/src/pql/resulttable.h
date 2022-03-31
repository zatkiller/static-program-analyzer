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
    std::vector<std::string> columns;
    Table table;

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
     * Retrieves the columns template of the result table.
     *
     * @return the column names
     */
    std::vector<std::string> getColumns();
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
    Table getTable();

    /**
     * Sets the table in the resultTable.
     *
     * @param table
     */
    void setTable(Table table);

    /**
     * @return whether the result table is empty.
     */
    bool isEmpty();

    /**
     * @return whether the result table contains valid results;
     */
    bool hasResult();

    /**
     * Transfers the PKBResponse in a type of set<PKBField> into set<vector<PKBField>>.
     *
     * @param response the PKBResponse in set<PKBField> format
     * @return the transferred PKBResponse in set<vector<PKBField>> format
     */
    static VectorResponse transToVectorResponse(SingleResponse response);

    /**
     * Transforms the PKBResponse into a resultTable
     *
     * @param response the PKBResponse
     * @param synonyms the list of all synonyms from the query
     * @return a new resultTable with the content in PKBResponse.
     */
    static ResultTable transToResultTable(PKBResponse response, std::vector<std::string> synonyms);

    /**
     * Inserts the PKBResponse to the result table and join the response to the table.
     *
     * @param r the PKBResponse from PKB side
     * @param synonyms the list of all synonyms from the query
     */
    void insert(PKBResponse r, std::vector<std::string> synonyms);

    /**
     * CrossJoins the result table to the current response table when the synonyms of the result are different from
     * all synonyms in the table.
     *
     * @param other a resultTable
     */
    void crossJoin(ResultTable& other);

    /**
     * Checks whether two rows in two resultTable are able to inner join together.
     *
     * @param row1 the row in result table 1
     * @param row2 the row in result table2
     * @param pos1 column numbers of shared synonyms in result table 1
     * @param pos2 column numbers of shared synonyms in result table 2
     * @return a bool values indicates whether two rows are able to join
     */
    bool isJoinable(std::vector<PKBField> row1, std::vector<PKBField> row2,
                                 std::vector<int> pos1, std::vector<int> pos2);

    /**
     * InnerJoins the result table to the current response table if table already contains the synonyms in the response.
     *
     * @param other a resultTable
     */
    void innerJoin(ResultTable& other);

    /**
     * Joins the result table to the result table using either crossJoin or innerJoin.
     *
     * @param other a resultTable
     */
    void join(ResultTable& other);
};
}  // namespace qps::evaluator
