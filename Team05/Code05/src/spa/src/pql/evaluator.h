#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <set>
#include <algorithm>
#include <iterator>
#include <memory>
#include <variant>
#include <map>
#include <list>

#include "pql/query.h"
#include "clausehandler.h"
#include "PKB/PKBField.h"
#include "PKB/PKBDataTypes.h"
#include "PKB/PKBEntityType.h"
#include "PKB/StatementType.h"
#include "PKB/PKBResponse.h"
#include "PKB.h"
#include "PKB/PKBRelationship.h"

namespace qps::evaluator {
    using namespace qps::query;
    using namespace qps::evaluator;

    /**
     * A class to evaluate a query object. Get the corresponding result from PKB and merge results accordingly.
     */
    class Evaluator {
        PKB *pkb;
        ResultTable resultTable;

    public:
        /** Constructor for the evaluator. */
        Evaluator(PKB *pkb) {
            this->pkb = pkb;
            this->resultTable = ResultTable();
        }

        /**
         * Retrieves all the results of a certain design entity from PKB database.
         *
         * @param type the design entity type to retrieve
         * @return PKBResponse contains all results of the design entity type
         */
        PKBResponse getAll(DesignEntity type);

        /**
         * Wraps a certain PKBFiend into a string representation.
         *
         * @param pkbField the PKBField to wrap
         * @return string representation of the PKBField
         */
        static std::string PKBFieldToString(PKBField pkbField);

        /**
         * Classifies the clauses in such that into clauses with synonyms and clauses without synonyms.
         *
         * @param clauses all clauses in such that
         * @param noSyn group of clauses without synonyms
         * @param hasSyn group of clauses with synonyms
         */
        void processSuchthat(std::vector<std::shared_ptr<RelRef>> clauses, std::vector<std::shared_ptr<RelRef>> &noSyn,
                             std::vector<std::shared_ptr<RelRef>> &hasSyn);

        /**
         * Classifies one clause into noSynonym group and withSynonym group.
         *
         * @param r the shared pointer of the relRef
         * @param f1 the memory address of the first RelRef variable
         * @param f2 the memory address of the second RelRef variable
         * @param noSyn group of clauses without synonyms
         * @param hasSyn group of clasues with synonyms
         */
        template<typename F1, typename F2>
        void processSuchthatRelRef(std::shared_ptr<RelRef> r, F1 f1, F2 f2, std::vector<std::shared_ptr<RelRef>> &noSyn,
                                   std::vector<std::shared_ptr<RelRef>> &hasSyn) {
            if (!f1.isDeclaration() && !f2.isDeclaration()) {
                noSyn.push_back(r);
            } else {
                hasSyn.push_back(r);
            }
        }

        /**
         * Retrieves the final result from the result table.
         *
         * @param table the reference the result table
         * @param variable the synonym in select part of the query
         * @return the list of string representation of the query result
         */
        std::list<std::string> getListOfResult(ResultTable &table, std::string variable);

        /**
         * Evaluates the query object.
         *
         * @param query the query object to evaluate
         * @return the list of string representation of the query result
         */
        std::list<std::string> evaluate(Query query);
    };
}

