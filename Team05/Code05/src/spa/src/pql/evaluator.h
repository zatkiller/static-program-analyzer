#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <set>

#include <query.h>

std::string evaluate(Query);

std::set<int> processSuchthat(std::vector<RelRef>, DesignEntity);

std::set<int> processPattern(std::vector<Pattern>, DesignEntity);