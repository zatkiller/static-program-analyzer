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

    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> ResultTable::getResult() {
        return this->table;
    }

    void ResultTable::insert(PKBResponse r) {
        if (std::unordered_set<PKBField, PKBFieldHash> *ptr = std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(
                &r.res)) {
            for (auto r : *ptr) {
                std::vector<PKBField> newRecord;
                newRecord.push_back(r);
                table.insert(newRecord);
            }
        } else if (std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> *ptr =
                std::get_if<std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>>(&r.res)) {
            for (auto r : *ptr) {
                table.insert(r);
            }
        }
    }


    void ResultTable::crossJoin(PKBResponse r) {
        if (table.empty()) {
            return;
        }
        std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> newTable;
        if (auto *ptr = std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&r.res)) {
            auto queryRes = *ptr;
            for (auto field : queryRes) {
                for (auto record : table) {
                    std::vector<PKBField> newRecord = record;
                    newRecord.push_back(field);
                    newTable.insert(newRecord);
                }
            }
        } else if (auto *ptr =
                std::get_if<std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>>(&r.res)) {
            auto queryRes = *ptr;
            for (auto r : queryRes) {
                for (auto record : table) {
                    std::vector<PKBField> newRecord = record;
                    for (auto field : r) {
                        newRecord.push_back(field);
                    }
                    newTable.insert(newRecord);
                }
            }
        }
        this->table = newTable;
    }

    void ResultTable::oneSynInnerJoin(std::string synName,
                                      std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> queryRes,
                                      std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>& newTable) {
        int pos = getSynLocation(synName);
        for (auto r : queryRes) {
            for (auto record : table) {
                if (r[0] == record[pos]) {
                    std::vector<PKBField> newRecord = record;
                    newTable.insert(newRecord);
                }
            }
        }
    }

    void ResultTable::oneSynInnerJoin(std::string synName,
                                      std::unordered_set<PKBField, PKBFieldHash> queryRes,
                                      std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>& newTable) {
        int pos = getSynLocation(synName);
        for (auto r : queryRes) {
            for (auto record : table) {
                if (r == record[pos]) {
                    std::vector<PKBField> newRecord = record;
                    newTable.insert(newRecord);
                }
            }
        }
    }

    void ResultTable::twoSynInnerJoinTwo(std::string syn1, std::string syn2,
                                         std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> queryRes,
                                         std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>& newTable) {
        int firstMatch = getSynLocation(syn1);
        int secondMatch = getSynLocation(syn2);
        for (auto r : queryRes) {
            for (auto record : table) {
                if (r[0] == record[firstMatch] && r[1] == record[secondMatch]) {
                    std::vector<PKBField> newRecord = record;
                    newTable.insert(newRecord);
                }
            }
        }
    }

    void ResultTable::twoSynInnerJoinOne(std::string syn1, std::string syn2, bool isFirst,
                                         std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> queryRes,
                                         std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>& newTable) {
        std::string matched = isFirst ? syn1 : syn2;
        std::string another = isFirst ? syn2 : syn1;
        int mPos = isFirst ? 0 : 1;
        int aPos = isFirst ? 1 : 0;
        int matchedPos = getSynLocation(matched);
        for (auto r : queryRes) {
            for (auto record : table) {
                if ((r[mPos] == record[matchedPos])) {
                    std::vector<PKBField> newRecord = record;
                    newRecord.push_back(r[aPos]);
                    newTable.insert(newRecord);
                }
            }
        }
    }

    void ResultTable::innerJoin(PKBResponse r, bool isFirst, bool isSecond, std::vector<std::string> allSyn) {
        if (table.empty()) {
            return;
        }
        std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> newTable;
        if (auto *ptr = std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&r.res)) {
            auto queryRes = *ptr;
            oneSynInnerJoin(allSyn[0], queryRes, newTable);
        } else if (auto *ptr =
                std::get_if<std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>>(&r.res)) {
            auto queryRes = *ptr;
            if (allSyn.size() == 1) {  // Only one synonym in the clause
                oneSynInnerJoin(allSyn[0], queryRes, newTable);
            } else if (isFirst && isSecond) {  // Two synonyms all in table already
                twoSynInnerJoinTwo(allSyn[0], allSyn[1], queryRes, newTable);
            } else {  // Two synonyms in the clause, one is already in the table
                twoSynInnerJoinOne(allSyn[0], allSyn[1], isFirst, queryRes, newTable);
            }
        }
        this->table = newTable;
    }

    void ResultTable::join(PKBResponse response, std::vector<std::string> synonyms) {
        if (synonyms.size() == 2) {
            std::string first = synonyms[0];
            std::string second = synonyms[1];
            if (synExists(first) && synExists(second)) {
                innerJoin(response, true, true, synonyms);
            } else if (!synExists(first) && !synExists(second)) {
                if (table.empty() && synSequenceMap.empty()) {
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
                innerJoin(response, true, false, synonyms);
            } else if (synExists(second)) {
                insertSynLocationToLast(first);
                innerJoin(response, false, true, synonyms);
            }
        } else {
            std::string syn = synonyms[0];
            if (synExists(syn)) {
                innerJoin(response, true, false, synonyms);
            } else {
                if (table.empty() && synSequenceMap.empty()) {
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
