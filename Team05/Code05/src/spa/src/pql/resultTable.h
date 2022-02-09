#pragma once

#include <string>
#include <vector>
#include <iterator>
#include <unordered_set>
#include <unordered_map>

#include "PKB/PKBResponse.h"

class ResultTable {
private:
    std::unordered_map<std::string, int> synSequenceMap;
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> table;
public:
    bool synExists(std::string);
    std::unordered_map<std::string, int> getSynMap();
    int getSynLocation(std::string);
    void insertSynLocationToLast(std::string);
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> getResult();
    //only do intsert when table is empty
    void insert(PKBResponse);
    void crossJoin(PKBResponse);
    void innerJoin(PKBResponse, bool, bool, std::vector<std::string>);
};
