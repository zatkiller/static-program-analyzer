#include "resulttable.h"

#include <utility>

namespace qps::evaluator {
    bool ResultTable::synExists(std::string name) {
        return synSequenceMap.find(name) != synSequenceMap.end();
    }

    std::unordered_map<std::string, int> ResultTable::getSynMap() {
        return this->synSequenceMap;
    }

    std::vector<std::string> ResultTable::getColumns() {
        return this->columns;
    }

    int ResultTable::getSynLocation(std::string synonym) {
        return getSynMap().find(synonym)->second;
    }

    void ResultTable::insertSynLocationToLast(std::string name) {
        int size = synSequenceMap.size();
        synSequenceMap.insert({name, size});
        columns.push_back(name);
    }
    
    Table ResultTable::getTable() {
        return this->table;
    }

    void ResultTable::setTable(Table table) {
        this->table = table;
    }

    bool ResultTable::isEmpty() {
        return table.empty() && synSequenceMap.empty();
    }

    bool ResultTable::hasResult() {
        return !table.empty() && !synSequenceMap.empty();
    }

    VectorResponse ResultTable::transToVectorResponse(SingleResponse response) {
        VectorResponse newVectorRes;
        for (const auto& r : response) {
            newVectorRes.insert(std::vector<PKBField>{r});
        }
        return newVectorRes;
    }

    ResultTable ResultTable::transToResultTable(PKBResponse response, std::vector<std::string> synonyms) {
        ResultTable resTable = ResultTable();
        for (auto & synonym : synonyms) {
            resTable.insertSynLocationToLast(synonym);
        }
        VectorResponse vResponse;
        if (auto *ptr = std::get_if<SingleResponse>(&response.res)) {
            vResponse = ResultTable::transToVectorResponse(*ptr);
        } else if (auto *ptr = std::get_if<VectorResponse>(&response.res)) {
            vResponse = *ptr;
        }
        Table t = Table{};
        for (auto r : vResponse) {
            t.insert(r);
        }
        resTable.setTable(t);
        return resTable;
    }

    void ResultTable::insert(PKBResponse r, std::vector<std::string> synonyms) {
        ResultTable resTable = ResultTable::transToResultTable(std::move(r), std::move(synonyms));
        join(resTable);
    }


    void ResultTable::crossJoin(ResultTable& other) {
        for (auto s : other.columns) {
            insertSynLocationToLast(s);
        }
        if (table.empty()) {
            return;
        }
        Table newTable;
        for (const auto& row : other.table) {
            for (const auto& thisRow : table) {
                auto newRow = thisRow;
                auto rowCopy = row;
                std::move(rowCopy.begin(), rowCopy.end(), std::back_inserter(newRow));
                newTable.emplace(std::move(newRow));
            }
        }
        this->table = std::move(newTable);
    }

    bool ResultTable::isJoinable(std::vector<PKBField> row1, std::vector<PKBField> row2,
                    std::vector<int> pos1, std::vector<int> pos2) {
        bool joinable = true;
        std::unordered_set<int> otherColsSet{};
        for (int i = 0; i < pos1.size(); i++) {
            PKBField thisFld = row1[pos1[i]];
            PKBField otherFld = row2[pos2[i]];
            if (!(thisFld == otherFld)) {
                joinable = false;
                break;
            }
        }
        return joinable;
    }

    void ResultTable::innerJoin(ResultTable& other) {
        if (table.empty()) {
            return;
        }
        std::vector<int> thisCols{};
        std::vector<int> otherCols{};
        for (auto syn : other.getColumns()) {
            if (synSequenceMap.find(syn) != synSequenceMap.end()) {
                thisCols.push_back(synSequenceMap.find(syn)->second);
                otherCols.push_back(other.getSynLocation(syn));
            } else {
                insertSynLocationToLast(syn);
            }
        }
        Table newTable;
        for (auto row : other.table) {
            for (auto record : table) {
                std::unordered_set<int> otherColsSet{otherCols.begin(), otherCols.end()};
                bool joinable = isJoinable(record, row, thisCols, otherCols);
                if (!joinable) continue;
                auto newRow = record;
                for (int i = 0; i < row.size(); i++) {
                    if(otherColsSet.find(i) == otherColsSet.end()) {
                        newRow.push_back(row[i]);
                    }
                }
                newTable.emplace(std::move(newRow));
            }
        }
        this->table = std::move(newTable);
    }

    void ResultTable::join(ResultTable& other) {
        bool hasSharedSyn = false;
        for (auto syn : other.getSynMap()) {
            if (synSequenceMap.find(syn.first) != synSequenceMap.end()) hasSharedSyn = true;
        }

        if (isEmpty()) {
            this->synSequenceMap = other.synSequenceMap;
            this->columns = other.columns;
            this->table = other.table;
        } else if (!hasSharedSyn) {
            crossJoin(other);
        } else {
            innerJoin(other);
        }
    }

    void ResultTable::filterColumns(std::vector<std::string> selectSyns) {
        std::vector<int> selectedColumn;
        std::unordered_map<std::string, int> map;
        int columnNo = 0;
        for (auto s : selectSyns) {
            if (synExists(s)) {
                selectedColumn.push_back(getSynLocation(s));
                map[s] = columnNo;
                columnNo++;
            }
        }
        this->synSequenceMap = map;
        Table newTable;
        for (auto row : table) {
            std::vector<PKBField> newRow;
            for (auto pos : selectedColumn) {
                newRow.push_back(row[pos]);
            }
            newTable.emplace(std::move(newRow));
        }
        this->table = std::move(newTable);
    }
}  // namespace qps::evaluator
