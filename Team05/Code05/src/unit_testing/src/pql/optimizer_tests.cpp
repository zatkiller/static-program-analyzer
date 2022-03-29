#include "pql/optimizer.h"
#include "pql/query.h"
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
    ptr3.get()->useStmt = qps::query::StmtRef::ofDeclaration( Declaration { "s1", qps::query::DesignEntity::STMT });
    ptr3.get()->used = qps::query::EntRef::ofWildcard();
    suchthat.push_back(ptr3);

    std::shared_ptr<qps::query::NextT> ptr4 = std::make_shared<qps::query::NextT>();
    ptr4.get()->before = qps::query::StmtRef::ofDeclaration( Declaration { "s", qps::query::DesignEntity::STMT });
    ptr4.get()->transitiveAfter = qps::query::StmtRef::ofDeclaration(Declaration { "a1", qps::query::DesignEntity::ASSIGN });
    suchthat.push_back(ptr4);

    qps::query::AttrCompareRef lhs1 = qps::query::AttrCompareRef::ofString("x");
    qps::query::AttrCompareRef rhs1 = qps::query::AttrCompareRef::ofString("x");
    qps::query::AttrCompare with1 = qps::query::AttrCompare{lhs1, rhs1};
    with.push_back(with1);

    qps::query::AttrRef attr2 = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                                    Declaration { "r", qps::query::DesignEntity::READ }};
    qps::query::AttrCompareRef lhs2 = qps::query::AttrCompareRef::ofAttrRef(attr2);
    qps::query::AttrCompareRef rhs2 = qps::query::AttrCompareRef::ofString("variable");
    qps::query::AttrCompare with2 = qps::query::AttrCompare{lhs2, rhs2};
    with.push_back(with2);

    qps::query::AttrRef attrl3 = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                                     Declaration { "r", qps::query::DesignEntity::READ }};
    qps::query::AttrRef attrr3 = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                                     Declaration { "pr", qps::query::DesignEntity::PRINT }};
    qps::query::AttrCompareRef lhs3 = qps::query::AttrCompareRef::ofAttrRef(attrl3);
    qps::query::AttrCompareRef rhs3 = qps::query::AttrCompareRef::ofAttrRef(attrr3);
    qps::query::AttrCompare with3 = qps::query::AttrCompare{lhs3, rhs3};
    with.push_back(with3);

    qps::query::AttrRef attrl4 = qps::query::AttrRef{qps::query::AttrName::STMTNUM,
                                                     Declaration { "a", qps::query::DesignEntity::ASSIGN }};
    qps::query::AttrRef attrr4 = qps::query::AttrRef{qps::query::AttrName::VALUE,
                                                     Declaration { "c", qps::query::DesignEntity::CONSTANT }};
    qps::query::AttrCompareRef lhs4 = qps::query::AttrCompareRef::ofAttrRef(attrl4);
    qps::query::AttrCompareRef rhs4 = qps::query::AttrCompareRef::ofAttrRef(attrr4);
    qps::query::AttrCompare with4 = qps::query::AttrCompare{lhs4, rhs4};
    with.push_back(with4);

    qps::query::Pattern pattern1 = qps::query::Pattern::ofAssignPattern("a",
                                                                        qps::query::EntRef::ofVarName("sum"),
                                                                        qps::query::ExpSpec::ofWildcard());
    pattern.push_back(pattern1);

    qps::query::Pattern pattern2 =qps::query::Pattern::ofAssignPattern(
            "a1",
            qps::query::EntRef::ofDeclaration(Declaration{"v", qps::query::DesignEntity::VARIABLE}),
            qps::query::ExpSpec::ofWildcard());
    pattern.push_back(pattern2);

    qps::optimizer::Optimizer opt = qps::optimizer::Optimizer(suchthat, with, pattern);
//    opt.groupClauses<std::shared_ptr<qps::query::RelRef>>(suchthat);
    opt.optimize();
    REQUIRE(opt.getGroups().size() == 5);
    printGroupSyns(opt.getGroups());
}