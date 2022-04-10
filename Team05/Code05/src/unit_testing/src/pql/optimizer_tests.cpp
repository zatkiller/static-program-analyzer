#include <memory>
#include "QPS/Optimizer.h"
#include "QPS/Query.h"
#include "catch.hpp"
#include "logging.h"

#define TEST_LOG Logger() << "optimizer_tests.cpp"

using qps::query::Declaration;

void printGroupSyns(std::vector<qps::optimizer::ClauseGroup> groups) {
    for (auto group : groups) {
        std::string s;
        for (auto syn : group.syns) {
            s += " " + syn;
        }
        TEST_LOG << s;
    }
}

TEST_CASE("OrderedClause") {
    qps::query::Parent p1;
    p1.parent = qps::query::StmtRef::ofDeclaration(qps::query::Declaration{ "s1", qps::query::DesignEntity::STMT });
    p1.child = qps::query::StmtRef::ofDeclaration(qps::query::Declaration{ "s2", qps::query::DesignEntity::STMT });

    std::shared_ptr<qps::query::RelRef> suchthat = std::make_shared<qps::query::Parent>(p1);

    qps::optimizer::OrderedClause o1 = qps::optimizer::OrderedClause::ofSuchThat(suchthat);
    REQUIRE(o1.isSuchThat());
    std::vector<std::string> syns1 = o1.getSynonyms();
    REQUIRE(syns1.size() == 2);
    REQUIRE(syns1[0] == "s1");
    REQUIRE(syns1[1] == "s2");

    std::shared_ptr<qps::query::RelRef> suchthatResult = o1.getSuchThat();
    REQUIRE((suchthatResult.get()->getType()) == qps::query::RelRefType::PARENT);

    std::shared_ptr<qps::query::Parent> p2 = std::dynamic_pointer_cast<qps::query::Parent>(suchthatResult);
    REQUIRE(p2.get()->parent == 
        qps::query::StmtRef::ofDeclaration(
            qps::query::Declaration{ "s1", qps::query::DesignEntity::STMT }
        )
    );
    REQUIRE(p2.get()->child ==
        qps::query::StmtRef::ofDeclaration(
            qps::query::Declaration{ "s2", qps::query::DesignEntity::STMT }
        )
    );

    qps::query::AttrRef ar = qps::query::AttrRef(
            qps::query::AttrName::STMTNUM,
            qps::query::Declaration{ "s3", qps::query::DesignEntity::STMT }
    );
    qps::query::AttrCompareRef lhs1 = qps::query::AttrCompareRef::ofAttrRef(ar);
    qps::query::AttrCompareRef rhs1 = qps::query::AttrCompareRef::ofString("x");
    qps::query::AttrCompare with1 = qps::query::AttrCompare{ lhs1, rhs1 };

    qps::optimizer::OrderedClause o2 = qps::optimizer::OrderedClause::ofWith(with1);
    REQUIRE(o2.isWith());
    std::vector<std::string> syns2 = o2.getSynonyms();
    REQUIRE(syns2.size() == 1);
    REQUIRE(syns2[0] == "s3");

    qps::query::Pattern pattern1 = qps::query::Pattern::ofAssignPattern("a",
        qps::query::EntRef::ofDeclaration(qps::query::Declaration{ "v", qps::query::DesignEntity::VARIABLE }),
        qps::query::ExpSpec::ofWildcard());

    qps::optimizer::OrderedClause o3 = qps::optimizer::OrderedClause::ofPattern(pattern1);
    REQUIRE(o3.isPattern());
    std::vector<std::string> syns3 = o3.getSynonyms();
    REQUIRE(syns3.size() == 2);
    REQUIRE(syns3[0] == "a");
    REQUIRE(syns3[1] == "v");
}


TEST_CASE("test group clauses") {
    std::vector<std::shared_ptr<qps::query::RelRef>> suchthat;
    std::vector<qps::query::AttrCompare> with;
    std::vector<qps::query::Pattern> pattern;

    std::shared_ptr<qps::query::Parent> ptr1 = std::make_shared<qps::query::Parent>();
    ptr1.get()->parent = qps::query::StmtRef::ofLineNo(3);
    ptr1.get()->child = qps::query::StmtRef::ofLineNo(6);
    suchthat.push_back(ptr1);

    std::shared_ptr<qps::query::ModifiesS> ptr2 = std::make_shared<qps::query::ModifiesS>();
    ptr2.get()->modifiesStmt = qps::query::StmtRef::ofWildcard();
    ptr2.get()->modified = qps::query::EntRef::ofWildcard();
    suchthat.push_back(ptr2);

    std::shared_ptr<qps::query::UsesS> ptr3 = std::make_shared<qps::query::UsesS>();
    ptr3.get()->useStmt = qps::query::StmtRef::ofDeclaration( 
        Declaration { "s1", qps::query::DesignEntity::STMT }
    );
    ptr3.get()->used = qps::query::EntRef::ofWildcard();
    suchthat.push_back(ptr3);

    std::shared_ptr<qps::query::NextT> ptr4 = std::make_shared<qps::query::NextT>();
    ptr4.get()->before = qps::query::StmtRef::ofDeclaration(
        Declaration { "s", qps::query::DesignEntity::STMT }
    );
    ptr4.get()->transitiveAfter = qps::query::StmtRef::ofDeclaration(
        Declaration { "a1", qps::query::DesignEntity::ASSIGN }
    );
    suchthat.push_back(ptr4);

    qps::query::AttrCompareRef lhs1 = qps::query::AttrCompareRef::ofString("x");
    qps::query::AttrCompareRef rhs1 = qps::query::AttrCompareRef::ofString("x");
    qps::query::AttrCompare with1 = qps::query::AttrCompare{lhs1, rhs1};
    with.push_back(with1);

    qps::query::AttrRef attr2 = qps::query::AttrRef{
        qps::query::AttrName::VARNAME,
        Declaration { "r", qps::query::DesignEntity::READ }
    };
    qps::query::AttrCompareRef lhs2 = qps::query::AttrCompareRef::ofAttrRef(attr2);
    qps::query::AttrCompareRef rhs2 = qps::query::AttrCompareRef::ofString("variable");
    qps::query::AttrCompare with2 = qps::query::AttrCompare{lhs2, rhs2};
    with.push_back(with2);

    qps::query::AttrRef attrl3 = qps::query::AttrRef{
        qps::query::AttrName::VARNAME,
        Declaration { "r", qps::query::DesignEntity::READ }
    };
    qps::query::AttrRef attrr3 = qps::query::AttrRef{
        qps::query::AttrName::VARNAME,
        Declaration { "pr", qps::query::DesignEntity::PRINT }
    };
    qps::query::AttrCompareRef lhs3 = qps::query::AttrCompareRef::ofAttrRef(attrl3);
    qps::query::AttrCompareRef rhs3 = qps::query::AttrCompareRef::ofAttrRef(attrr3);
    qps::query::AttrCompare with3 = qps::query::AttrCompare{lhs3, rhs3};
    with.push_back(with3);

    qps::query::AttrRef attrl4 = qps::query::AttrRef{
        qps::query::AttrName::STMTNUM, 
        Declaration { "a", qps::query::DesignEntity::ASSIGN }
    };
    qps::query::AttrRef attrr4 = qps::query::AttrRef{
        qps::query::AttrName::VALUE,
        Declaration { "c", qps::query::DesignEntity::CONSTANT }
    };
    qps::query::AttrCompareRef lhs4 = qps::query::AttrCompareRef::ofAttrRef(attrl4);
    qps::query::AttrCompareRef rhs4 = qps::query::AttrCompareRef::ofAttrRef(attrr4);
    qps::query::AttrCompare with4 = qps::query::AttrCompare{lhs4, rhs4};
    with.push_back(with4);

    qps::query::Pattern pattern1 = qps::query::Pattern::ofAssignPattern(
        "a",
        qps::query::EntRef::ofVarName("sum"),
        qps::query::ExpSpec::ofWildcard()
    );
    pattern.push_back(pattern1);

    qps::query::Pattern pattern2 = qps::query::Pattern::ofAssignPattern(
        "a1",
        qps::query::EntRef::ofDeclaration(Declaration{"v", qps::query::DesignEntity::VARIABLE}),
        qps::query::ExpSpec::ofWildcard()
    );
    pattern.push_back(pattern2);

    qps::optimizer::Optimizer opt = qps::optimizer::Optimizer(suchthat, with, pattern);
    opt.optimize();
    REQUIRE(opt.getGroups().size() == 5);

    TEST_LOG << "Test group order";
    std::vector<qps::optimizer::ClauseGroup> orderedGroups;
    while (opt.hasNextGroup()) {
        orderedGroups.push_back(opt.nextGroup());
    }
    for (int i = 0; i < orderedGroups.size() - 1; i++) {
        REQUIRE(orderedGroups[i].syns.size() <= orderedGroups[i + 1].syns.size());
    }
    REQUIRE(orderedGroups[0].syns.find("") != orderedGroups[0].syns.end());
    printGroupSyns(orderedGroups);

    TEST_LOG << "Test clause order";
    for (auto group : orderedGroups) {
        std::vector<qps::optimizer::OrderedClause> orderedClause;
        TEST_LOG << " group id: " + std::to_string(group.groupId);
        while (group.hasNextClause()) {
            qps::optimizer::OrderedClause clause = group.nextClause();
            orderedClause.push_back(clause);
            int priority = clause.getPriority();
            TEST_LOG << " " + std::to_string(priority);
        }
        TEST_LOG << "\n";
        REQUIRE(
            orderedClause.size() == 
                group.suchthatGroup.size() 
                + group.withGroup.size() 
                + group.patternGroup.size()
        );
        for (int i = 0; i < orderedClause.size() - 1; i++) {
            REQUIRE(orderedClause[i].getSynonyms().size() <= orderedClause[i + 1].getSynonyms().size());
        }
    }
}
