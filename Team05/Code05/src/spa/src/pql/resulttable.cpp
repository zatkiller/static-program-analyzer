#include "resulttable.h"

namespace qps::evaluator {
    bool ResultTable::synExists(std::string name) {
        return synSequenceMap.find(name) != synSequenceMap.end();
    }

    std::unordered_map<std::string, int> ResultTable::getSynMap() {
        return this->synSequenceMap;
    }

    int ResultTable::getSynLocation(std::string synonym) {
        return getSynMap().find(synonym)->second;
    }

    void ResultTable::insertSynLocationToLast(std::string name) {
        int size = synSequenceMap.size();
        synSequenceMap.insert({name, size});
    }

    Table ResultTable::getResult() {
        return this->table;
    }

    bool ResultTable::isEmpty() {
        return table.empty() && synSequenceMap.empty();

    }

    VectorResponse ResultTable::transToVectorResponse(SingleResonse response) {
        VectorResponse newVectorRes;
        for (auto r : response) {
            newVectorRes.insert(std::vector<PKBField>{r});
        }
        return newVectorRes;
    }

    void ResultTable::insert(PKBResponse r) {
        VectorResponse response;
        if (auto *ptr = std::get_if<SingleResonse>(&r.res)) {
            response = transToVectorResponse(*ptr);
        } else if (auto *ptr = std::get_if<VectorResponse>(&r.res)) {
            response = *ptr;
        }
        for (auto r : response) {
            table.insert(r);
        }
    }


    void ResultTable::crossJoin(PKBResponse r) {
        if (table.empty()) {
            return;
        }
        Table newTable;
        VectorResponse response;
        if (auto *ptr = std::get_if<SingleResonse>(&r.res)) {
            response = transToVectorResponse(*ptr);
        } else if (auto *ptr = std::get_if<VectorResponse>(&r.res)) {
            response = *ptr;
        }

        for (auto r : response) {
            for (auto record : table) {
                std::vector<PKBField> newRecord = record;
                auto rCopy = r;
                std::move(rCopy.begin(), rCopy.end(), std::back_inserter(newRecord));
                newTable.insert(newRecord);
            }
        }
        this->table = newTable;
    }

    void ResultTable::oneSynInnerJoin(InnerJoinParam params, Table& newTable) {
        int pos = getSynLocation(params.syns[0]);
        for (auto r : params.vectorResponse) {
            for (auto record : table) {
                if (r[0] == record[pos]) {
                    std::vector<PKBField> newRecord = record;
                    newTable.insert(newRecord);
                }
            }
        }
    }

    void ResultTable::twoSynInnerJoinTwo(InnerJoinParam params, Table& newTable) {
        int firstMatch = getSynLocation(params.syns[0]);
        int secondMatch = getSynLocation(params.syns[1]);
        for (auto r : params.vectorResponse) {
            for (auto record : table) {
                if (r[0] == record[firstMatch] && r[1] == record[secondMatch]) {
                    std::vector<PKBField> newRecord = record;
                    newTable.insert(newRecord);
                }
            }
        }
    }

    void ResultTable::twoSynInnerJoinOne(InnerJoinParam params, Table& newTable) {
        std::string matched = params.isFirst ? params.syns[0] : params.syns[1];
        std::string another = params.isFirst ? params.syns[1]  : params.syns[0];
        int mPos = params.isFirst ? 0 : 1;
        int aPos = params.isFirst ? 1 : 0;
        int matchedPos = getSynLocation(matched);
        for (auto r : params.vectorResponse) {
            for (auto record : table) {
                if ((r[mPos] == record[matchedPos])) {
                    std::vector<PKBField> newRecord = record;
                    newRecord.push_back(r[aPos]);
                    newTable.insert(newRecord);
                }
            }
        }
    }

    void ResultTable::innerJoin(InnerJoinParam params) {
        if (table.empty()) {
            return;
        }
        Table newTable;
        if (auto *ptr = std::get_if<SingleResonse>(&params.response.res)) {
            params.vectorResponse = transToVectorResponse(*ptr);
        } else if (auto *ptr = std::get_if<VectorResponse>(&params.response.res)) {
            params.vectorResponse = *ptr;
        }
        if (params.syns.size() == 1) {  // Only one synonym in the clause
            oneSynInnerJoin(params, newTable);
        } else if (params.isFirst && params.isSecond) {  // Two synonyms all in table already
            twoSynInnerJoinTwo(params, newTable);
        } else {  // Two synonyms in the clause, one is already in the table
            twoSynInnerJoinOne(params, newTable);
        }
        this->table = newTable;
    }

    void ResultTable::join(PKBResponse response, std::vector<std::string> synonyms) {
        if (synonyms.size() == 2) {
            std::string first = synonyms[0];
            std::string second = synonyms[1];
            if (synExists(first) && synExists(second)) {
                innerJoin(InnerJoinParam{synonyms, true, true, response});
            } else if (!synExists(first) && !synExists(second)) {
                if (isEmpty()) {
                    insertSynLocationToLast(first);
                    insertSynLocationToLast(second);
                    insert(response);
                    return;
                }
                insertSynLocationToLast(first);
                insertSynLocationToLast(second);
                crossJoin(response);
            } else if (synExists(first)) {
                insertSynLocationToLast(second);
                innerJoin(InnerJoinParam{synonyms, true, false, response});
            } else if (synExists(second)) {
                insertSynLocationToLast(first);
                innerJoin(InnerJoinParam{synonyms, false, true, response});
            }
        } else {
            std::string syn = synonyms[0];
            if (synExists(syn)) {
                innerJoin(InnerJoinParam{synonyms, true, false, response});
            } else {
                if (isEmpty()) {
                    insertSynLocationToLast(syn);
                    insert(response);
                    return;
                }
                insertSynLocationToLast(syn);
                crossJoin(response);
            }
        }
    }
}  // namespace qps::evaluator
