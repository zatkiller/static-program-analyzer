#pragma once

#include <utility>
#include <queue>
#include <climits>
#include "pql/query.h"
namespace qps::optimizer {

enum class OrderedClauseType {
    INVALID,
    SUCH_THAT,
    WITH,
    PATTERN
};

struct OrderedClause {

    static OrderedClause ofWith(query::AttrCompare &);
    static OrderedClause ofSuchThat(std::shared_ptr<query::RelRef> &);
    static OrderedClause ofPattern(query::Pattern &);

    std::shared_ptr<query::RelRef> getSuchThat() { return suchthat; }
    query::AttrCompare getWith() { return with; }
    query::Pattern getPattern() { return pattern; }

    std::vector<std::string> getSynonyms() { return synonyms; }
    int getPriority();
    bool isWith() { return type == OrderedClauseType::WITH; }
    bool isSuchThat() { return type == OrderedClauseType::SUCH_THAT; }
    bool isPattern() { return type == OrderedClauseType::PATTERN; }

    bool operator==(const OrderedClause& o) const;
    size_t getHash() const;

private:
    std::shared_ptr<query::RelRef> suchthat {};
    query::AttrCompare with {};
    query::Pattern pattern {};

    std::vector<std::string> synonyms;

    OrderedClauseType type = OrderedClauseType::INVALID;
};

struct ClausePriority {
public:
    bool operator()(OrderedClause& a,OrderedClause& b)
    {
        if (a.getSynonyms().size() < b.getSynonyms().size()) {
            return false;
        } else if (a.getSynonyms().size() > b.getSynonyms().size()) {
            return true;
        } else {
            return a.getPriority() > b.getPriority();
        }
    }
};

struct OrderedClauseHash {
public:
    size_t operator() (const OrderedClause& clause) const { return clause.getHash(); }
};


struct BFS {
    bool initialized = false;
    std::priority_queue<OrderedClause, std::vector<OrderedClause>, ClausePriority> pq;
    std::unordered_set<OrderedClause, OrderedClauseHash> visitedCl;
    std::unordered_set<std::string> visitedSyn;
};

struct ClauseGroup {
    int groupId;
    std::unordered_set<std::string> syns;
    std::vector<std::shared_ptr<query::RelRef>> suchthatGroup;
    std::vector<query::AttrCompare> withGroup;
    std::vector<query::Pattern> patternGroup;

    std::unordered_map<std::string, std::vector<OrderedClause>> subgroups;
    std::string startingPoint;
    int minClauseNo = INT_MAX;
    BFS bfs;

    static ClauseGroup ofNewGroup(int id);

    template<typename T>
    void addClause(T clause, std::vector<std::string> syns) {
        OrderedClause o;
        if constexpr(std::is_same_v<T, std::shared_ptr<query::RelRef>>) {
            suchthatGroup.push_back(clause);
            o = OrderedClause::ofSuchThat(clause);
        } else if constexpr(std::is_same_v<T, query::AttrCompare>) {
            withGroup.push_back(clause);
            o = OrderedClause::ofWith(clause);
        } else if constexpr(std::is_same_v<T, query::Pattern>) {
            patternGroup.push_back(clause);
            o = OrderedClause::ofPattern(clause);
        }
        addSyn(syns);
        for (auto s : syns) {
            if (subgroups.find(s) != subgroups.end()) subgroups[s].push_back(o);
            else
                subgroups[s] = std::vector<OrderedClause>{o};

            if (s == startingPoint && subgroups[s].size() < minClauseNo) minClauseNo += 1;
            else if (subgroups[s].size() < minClauseNo) {
                startingPoint = s;
                minClauseNo = subgroups[s].size();
            }
        }
    }

    bool noSyn();
    void addSyn(std::vector<std::string> s);
    void insertToPQ(std::string s);
    bool hasNextClause();
    OrderedClause nextClause();
};


struct GroupPriority {
public:
    bool operator()(ClauseGroup& a,ClauseGroup& b)
    {
        if (a.syns.find("") != a.syns.end()) {
            return false;
        } else if (b.syns.find("") != b.syns.end()) {
            return true;
        } else {
            return a.syns.size() > b.syns.size();
        }
    }
};

class Optimizer {
std::vector<ClauseGroup> groups;
std::unordered_map<std::string, int> synToGroup;
std::vector<std::shared_ptr<query::RelRef>>& suchthat;
std::vector<query::AttrCompare>& with;
std::vector<query::Pattern>& pattern;
std::priority_queue<ClauseGroup, std::vector<ClauseGroup>, GroupPriority> pq;

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
                groups[groupId].addClause(clause, syns);
            }
            else {
                ClauseGroup newGroup = ClauseGroup::ofNewGroup(groupId);
                newGroup.addClause(clause, syns);
                groups.push_back(newGroup);
            }
            addSynsToMap(syns, groupId);
        }
    }

    void optimize();

    std::vector<ClauseGroup> getGroups();

    bool hasNextGroup();

    ClauseGroup nextGroup();
};
}
