#pragma once

#include <utility>

#include "pql/query.h"
namespace qps::optimizer {

struct ClauseGroup {
    int groupId;
    std::unordered_set<std::string> syns;
    std::vector<std::shared_ptr<query::RelRef>> suchthatGroup;
    std::vector<query::AttrCompare> withGroup;
    std::vector<query::Pattern> patternGroup;

    static ClauseGroup ofNewGroup(int id);

    template<typename T>
    void addClause(T clause, std::vector<std::string> syns) {
        if constexpr(std::is_same_v<T, std::shared_ptr<query::RelRef>>) suchthatGroup.push_back(clause);
        else if constexpr(std::is_same_v<T, query::AttrCompare>) withGroup.push_back(clause);
        else if constexpr(std::is_same_v<T, query::Pattern>) patternGroup.push_back(clause);
        addSyn(syns);
    }

    bool hasSyn();
    void addSyn(std::vector<std::string> s);
};

class Optimizer {
std::vector<ClauseGroup> groups;
std::unordered_map<std::string, int> synToGroup;
std::vector<std::shared_ptr<query::RelRef>>& suchthat;
std::vector<query::AttrCompare>& with;
std::vector<query::Pattern>& pattern;

public:
    Optimizer(std::vector<std::shared_ptr<query::RelRef>>& suchthatcl, std::vector<query::AttrCompare>& withcl,
              std::vector<query::Pattern>& patterncl) :
              suchthat(suchthatcl), with(withcl), pattern(patterncl) {
    }

    template<typename T>
    std::vector<std::string> getSynonyms(T clause) {
        std::vector<std::string> res;
        if constexpr(std::is_same_v<T, std::shared_ptr<query::RelRef>>) {
            std::vector<query::Declaration> decs = clause->getDecs();
            if (decs.empty()) return std::vector<std::string>{""};
            for (auto d : decs) {
                res.push_back(d.getSynonym());
            }
        } else if constexpr(std::is_same_v<T, query::AttrCompare>) {
            if (!clause.getLhs().isAttrRef() && !clause.getRhs().isAttrRef()) return std::vector<std::string>{""};
            if (clause.getLhs().isAttrRef()) res.push_back(clause.getLhs().getAttrRef().getDeclarationSynonym());
            if (clause.getRhs().isAttrRef()) res.push_back(clause.getRhs().getAttrRef().getDeclarationSynonym());
        } else if constexpr(std::is_same_v<T, query::Pattern>) {
            res.push_back(clause.getSynonym());
            if (clause.getEntRef().isDeclaration()) res.push_back(clause.getEntRef().getDeclarationSynonym());
        }
        return res;
    }

    void addSynsToMap(std::vector<std::string> syns, int groupId);

    template<typename T>
    void groupClauses(std::vector<T>& clauses) {
        for (auto clause : clauses) {
            std::vector<std::string> syns = getSynonyms<T>(clause);
            int groupId = groups.size();
            bool belongsToExistGroup = false;
            for (auto s : syns) {
                if (synToGroup.find(s) != synToGroup.end()) {
                    belongsToExistGroup = true;
                    groupId = synToGroup.find(s)->second;
                }
            }
            if (belongsToExistGroup) {
                groups[groupId].template addClause(clause, syns);
            } else {
                ClauseGroup newGroup = ClauseGroup::ofNewGroup(groupId);
                newGroup.template addClause(clause, syns);
                groups.push_back(newGroup);
            }
            addSynsToMap(syns, groupId);
        }
    }

    void optimize();

    std::vector<ClauseGroup> getGroups();
};
}
