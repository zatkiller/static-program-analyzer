#include "resultTable.h"

std::unordered_map<std::string, int> resultTable::getSynMap() {
    return this->synSequenceMap;
}

int resultTable::getSycLocation(std::string synonym) {
    return this->synSequenceMap.find(synonym)->second;
}

std::unordered_set<std::vector<std::string>> resultTable::getResult() {
    return this->result;
}