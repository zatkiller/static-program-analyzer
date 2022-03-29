#include "optimizer.h"
namespace qps::optimizer {
    ClauseGroup ClauseGroup::ofNewGroup(int id) {
        ClauseGroup group = ClauseGroup();
        group.groupId = id;
        return group;
    }

    void ClauseGroup::addSyn(std::vector<std::string> s) {
        for (auto syn : s) {
            syns.insert(syn);
        }
    }

    bool ClauseGroup::hasSyn() {
        return syns.size() == 1 && syns.find("") != syns.end();
    }

    void Optimizer::addSynsToMap(std::vector<std::string> syns, int groupId) {
        for (auto syn : syns) {
            synToGroup[syn] = groupId;
        }
    }

    void Optimizer::optimize() {
        groupClauses<std::shared_ptr<query::RelRef>>(suchthat);
        groupClauses<query::AttrCompare>(with);
        groupClauses<query::Pattern>(pattern);
    }

    std::vector<ClauseGroup> Optimizer::getGroups() {
        return groups;
    }
} // namespace qps::optimizer