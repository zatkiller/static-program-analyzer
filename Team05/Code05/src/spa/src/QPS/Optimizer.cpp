#include "Optimizer.h"
namespace qps::optimizer {
    using query::RelRefType;
    using utils::hash_combine;

    std::unordered_set<query::RelRefType> higherPriorityClause {
        RelRefType::MODIFIESS, RelRefType::MODIFIESP, RelRefType::USESP, RelRefType::USESS, RelRefType::FOLLOWS,
        RelRefType::PARENT, RelRefType::CALLS, RelRefType::NEXT, RelRefType::AFFECTS
    };

    OrderedClause OrderedClause::ofWith(query::AttrCompare& with) {
        OrderedClause o;
        o.type = OrderedClauseType::WITH;
        o.with = std::move(with);

        query::AttrCompareRef lhs = o.with.getLhs();
        query::AttrCompareRef rhs = o.with.getRhs();

        if (lhs.isAttrRef()) {
            std::string synonym = lhs.getAttrRef().getDeclarationSynonym();
            o.synonyms.push_back(synonym);
        }

        if (rhs.isAttrRef()) {
            std::string synonym = rhs.getAttrRef().getDeclarationSynonym();
            o.synonyms.push_back(synonym);
        }

        return o;
    }

    OrderedClause OrderedClause::ofSuchThat(std::shared_ptr<query::RelRef>& relRef) {
        OrderedClause o;
        o.type = OrderedClauseType::SUCH_THAT;
        o.suchthat = std::move(relRef);

        std::vector<query::Declaration> declarations = o.suchthat.get()->getDecs();

        for (int i = 0; i < declarations.size(); i++) {
            o.synonyms.push_back(declarations[i].getSynonym());
        }

        return o;
    }

    OrderedClause OrderedClause::ofPattern(query::Pattern& pattern) {
        OrderedClause o;
        o.type = OrderedClauseType::PATTERN;
        o.pattern = std::move(pattern);

        query::EntRef lhs = o.pattern.getEntRef();
        o.synonyms.push_back(o.pattern.getSynonym());
        if (lhs.isDeclaration()) {
            o.synonyms.push_back(lhs.getDeclarationSynonym());
        }

        return o;
    }

    int OrderedClause::getPriority() {
        int priority;
        if (isWith()) {
            priority = 0;
        } else if (isPattern()) {
            query::Pattern p = getPattern();
            if (p.getSynonymType() == query::DesignEntity::IF || p.getSynonymType() == query::DesignEntity::WHILE) {
                priority = 1;
            } else if (p.getExpression().isFullMatch()) {
                priority = 2;
            } else if (p.getExpression().isPartialMatch()) {
                priority = 3;
            } else {
                priority = 4;
            }
        } else {
            std::shared_ptr<query::RelRef> s = getSuchThat();
            RelRefType type = s->getType();
            if (higherPriorityClause.find(type) != higherPriorityClause.end()) {
                priority = 5;
            } else {
                priority = 6;
            }
        }
        return priority;
    }

    bool OrderedClause::operator==(const OrderedClause &o) const {
        if (type != o.type) return false;
        if (type == OrderedClauseType::SUCH_THAT) {
            return *(suchthat) == *(o.suchthat);
        } else if (type == OrderedClauseType::WITH) {
            return with == o.with;
        } else {
            return pattern == o.pattern;
        }
    }

    size_t OrderedClause::getHash() const {
        size_t seed = 0;
        hash_combine(seed, type);
        if (type == OrderedClauseType::SUCH_THAT) hash_combine(seed, suchthat);
        if (type == OrderedClauseType::PATTERN) hash_combine(seed, pattern);
        if (type == OrderedClauseType::WITH) hash_combine(seed, with);
        return seed;
    }

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

    bool ClauseGroup::noSyn() {
        return syns.size() == 1 && syns.find("") != syns.end();
    }


    OrderedClause ClauseGroup::nextClause() {
        if (!bfs.initialized) {
            bfs.visitedSyn.insert(startingPoint);
            for (auto cl : subgroups[startingPoint]) {
                bfs.pq.push(cl);
                bfs.visitedCl.insert(cl);
            }
            bfs.initialized = true;
        }
        OrderedClause c = bfs.pq.top();
        bfs.pq.pop();
        for (auto s : c.getSynonyms()) {
            if (bfs.visitedSyn.find(s) == bfs.visitedSyn.end()) {
                insertToPQ(s);
            }
        }
        return c;
    }

    void ClauseGroup::insertToPQ(std::string s) {
        bfs.visitedSyn.insert(s);
        for (auto cl : subgroups[s]) {
            if (bfs.visitedCl.find(cl) == bfs.visitedCl.end()) {
                bfs.pq.push(cl);
                bfs.visitedCl.insert(cl);
            }
        }
    }

    bool ClauseGroup::hasNextClause() {
        return !bfs.pq.empty() || !bfs.initialized;
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
        for (auto g : groups) {
            pq.push(g);
        }
    }

    std::vector<ClauseGroup> Optimizer::getGroups() {
        return groups;
    }

    ClauseGroup Optimizer::nextGroup() {
        ClauseGroup group = pq.top();
        pq.pop();
        return group;
    }

    bool Optimizer::hasNextGroup() {
        return !pq.empty();
    }
}  // namespace qps::optimizer
