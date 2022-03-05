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

    void ResultTable::oneSynInnerJoin(InnerJoinParam param, Table& newTable) {
        int pos = getSynLocation(param.syns[0]);
        for (auto r : param.vectorResponse) {
            for (auto record : table) {
                if (r[0] == record[pos]) {
                    std::vector<PKBField> newRecord = record;
                    newTable.insert(newRecord);
                }
            }
        }
    }

    void ResultTable::twoSynInnerJoinTwo(InnerJoinParam param, Table& newTable) {
        int firstMatch = getSynLocation(param.syns[0]);
        int secondMatch = getSynLocation(param.syns[1]);
        for (auto r : param.vectorResponse) {
            for (auto record : table) {
                if (r[0] == record[firstMatch] && r[1] == record[secondMatch]) {
                    std::vector<PKBField> newRecord = record;
                    newTable.insert(newRecord);
                }
            }
        }
    }

    void ResultTable::twoSynInnerJoinOne(InnerJoinParam param, Table& newTable) {
        std::string matched = param.isFirst ? param.syns[0] : param.syns[1];
        std::string another = param.isFirst ? param.syns[1]  : param.syns[0];
        int mPos = param.isFirst ? 0 : 1;
        int aPos = param.isFirst ? 1 : 0;
        int matchedPos = getSynLocation(matched);
        for (auto r : param.vectorResponse) {
            for (auto record : table) {
                if ((r[mPos] == record[matchedPos])) {
                    std::vector<PKBField> newRecord = record;
                    newRecord.push_back(r[aPos]);
                    newTable.insert(newRecord);
                }
            }
        }
    }

    void ResultTable::innerJoin(InnerJoinParam param) {
        if (table.empty()) {
            return;
        }
        Table newTable;
        if (auto *ptr = std::get_if<SingleResonse>(&param.response.res)) {
            param.vectorResponse = transToVectorResponse(*ptr);
        } else if (auto *ptr = std::get_if<VectorResponse>(&param.response.res)) {
            param.vectorResponse = *ptr;
        }
        if (param.syns.size() == 1) {  // Only one synonym in the clause
            oneSynInnerJoin(param, newTable);
        } else if (param.isFirst && param.isSecond) {  // Two synonyms all in table already
            twoSynInnerJoinTwo(param, newTable);
        } else {  // Two synonyms in the clause, one is already in the table
            twoSynInnerJoinOne(param, newTable);
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
