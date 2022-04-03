#include "optimizer.h"
namespace qps::optimizer {

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