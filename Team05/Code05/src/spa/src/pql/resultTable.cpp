#include "resultTable.h"

bool ResultTable::synExists(std::string name) {
    return synSequenceMap.find(name) != synSequenceMap.end();
}

std::unordered_map<std::string, int> ResultTable::getSynMap() {
    return this->synSequenceMap;
}

int ResultTable::getSynLocation(std::string synonym) {
    return this->synSequenceMap.find(synonym)->second;
}

void ResultTable::insertSynLocationToLast(std::string name) {
    int size = synSequenceMap.size();
    synSequenceMap.insert({name, size});
}

std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> ResultTable::getResult() {
    return this->table;
}

void ResultTable::insert(PKBResponse r) {
    if (std::unordered_set<PKBField, PKBFieldHash>* ptr = std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&r.res)) {
        for (auto r : *ptr) {
            std::vector<PKBField> newRecord;
            newRecord.push_back(r);
            table.insert(newRecord);
        }
    } else if (std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>* ptr = std::get_if<std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>>(&r.res)) {
        for (auto r : *ptr) {
            table.insert(r);
        }
    }
}


void ResultTable::crossJoin(PKBResponse r) {
    if (table.empty()) {
        insert(r);
        return;
    }
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> newTable;
    if (std::unordered_set<PKBField, PKBFieldHash>* ptr = std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&r.res)) {
        std::unordered_set<PKBField, PKBFieldHash> queryRes = *ptr;
        for (auto r : queryRes) {
            for (auto record : table) {
                std::vector<PKBField> newRecord = record;
                newRecord.push_back(r);
                newTable.insert(newRecord);
            }
        }
    } else if (std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>* ptr = std::get_if<std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>>(&r.res)) {
        std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> queryRes = *ptr;
        for (auto r : queryRes) {
            for (auto record : table) {
                std::vector<PKBField> newRecord = record;
                newRecord.insert(record.end(),
                              std::make_move_iterator(r.begin()),
                              std::make_move_iterator(r.end()));
                newTable.insert(newRecord);
            }
        }
    } else {
        return;
    }
    this->table = newTable;
}

void ResultTable::innerJoin(PKBResponse r, bool isFirst, bool isSecond, std::vector<std::string> allSyn) {
    if (table.empty()) {
        insert(r);
        return;
    }
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> newTable;
    if (std::unordered_set<PKBField, PKBFieldHash> *ptr = std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(
            &r.res)) {
        int pos = getSynLocation(allSyn[0]);
        std::unordered_set<PKBField, PKBFieldHash> queryRes = *ptr;
        for (auto r: queryRes) {
            for (auto record: table) {
                if (r == record[pos]) {
                    std::vector<PKBField> newRecord = record;
                    newTable.insert(newRecord);
                }
            }
        }
    } else if (std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> *ptr = std::get_if<std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>>(
            &r.res)) {
        std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> queryRes = *ptr;
        // two synonyms all in table already
        if (isFirst && isSecond) {
            int firstMatch = getSynLocation(allSyn[0]);
            int secondMatch = getSynLocation(allSyn[1]);
            for (auto r: queryRes) {
                for (auto record: table) {
                    if (r[0] == record[firstMatch] && r[1] == record[secondMatch]) {
                        std::vector<PKBField> newRecord = record;
                        newTable.insert(newRecord);
                    }
                }
            }
        } else { //one synonym in the tabel
            std::string matched = isFirst ? allSyn[0] : allSyn[1];
            std::string another = isFirst ? allSyn[1] : allSyn[2];
            int m = isFirst ? 0 : 1;
            int a = isFirst ? 1 : 0;
            int matchedPos = getSynLocation(matched);
            for (auto r: queryRes) {
                for (auto record: table) {
                    if ((r[m] == record[matchedPos])) {
                        std::vector<PKBField> newRecord = record;
                        newRecord.push_back(record[a]);  //ensure new field is added at last, otherwise use insert()
                        newTable.insert(newRecord);
                    }
                }
            }
        }
    }
    this->table = newTable;
}
