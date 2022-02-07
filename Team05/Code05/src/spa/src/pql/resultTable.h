#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>


class resultTable {
    std::unordered_map<std::string, int> synSequenceMap;
    std::unordered_set<std::vector<std::string>> result;
public:
    std::unordered_map<std::string, int> getSynMap();
    int getSycLocation(std::string);
    std::unordered_set<std::vector<std::string>> getResult();

};