#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <set>
#include <algorithm>
#include <iterator>

#include <query.h>

std::set<int> processSuchthat(std::vector<RelRef>, DesignEntity);

std::set<int> processPattern(std::vector<Pattern>, DesignEntity);

std::string processResult(std::set<int>);

std::string evaluate(Query);