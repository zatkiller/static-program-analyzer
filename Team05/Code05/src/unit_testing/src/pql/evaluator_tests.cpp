#include "pql/evaluator.h"
#include "pql/query.h"
#include "catch.hpp"
#include "logging.h"


#define TEST_LOG Logger() << "evaluator_tests.cpp"

using qps::query::Declaration;

TEST_CASE("Process such that") {
    std::shared_ptr<qps::query::Parent> ptr1 = std::make_shared<qps::query::Parent>();
    ptr1.get()->parent = qps::query::StmtRef::ofLineNo(3);
    ptr1.get()->child = qps::query::StmtRef::ofLineNo(6);

    std::shared_ptr<qps::query::ModifiesS> ptr2 = std::make_shared<qps::query::ModifiesS>();
    ptr2.get()->modifiesStmt = qps::query::StmtRef::ofWildcard();
    ptr2.get()->modified = qps::query::EntRef::ofWildcard();

    std::shared_ptr<qps::query::UsesS> ptr3 = std::make_shared<qps::query::UsesS>();
    ptr3.get()->useStmt = qps::query::StmtRef::ofDeclaration( Declaration { "s1", qps::query::DesignEntity::STMT });
    ptr3.get()->used = qps::query::EntRef::ofWildcard();

    std::vector<std::shared_ptr<qps::query::RelRef>> suchThat1 = {ptr1};
    std::vector<std::shared_ptr<qps::query::RelRef>> suchThat2 = {ptr2};
    std::vector<std::shared_ptr<qps::query::RelRef>> suchThat3 = {ptr3};
    std::vector<std::shared_ptr<qps::query::RelRef>> noSyn;
    std::vector<std::shared_ptr<qps::query::RelRef>> hasSyn;

    PKB pkb = PKB();
    PKB* ptr = &pkb;
    qps::evaluator::Evaluator evaluator{ptr};
    evaluator.processSuchthat(suchThat1, noSyn, hasSyn);
    REQUIRE(noSyn.size() == 1);
    REQUIRE(hasSyn.empty());
    noSyn.clear();

    evaluator.processSuchthat(suchThat2, noSyn, hasSyn);
    REQUIRE(noSyn.size() == 1);
    REQUIRE(hasSyn.empty());
    REQUIRE(noSyn[0]->getType() == qps::query::RelRefType::MODIFIESS);
    noSyn.clear();

    evaluator.processSuchthat(suchThat3, noSyn, hasSyn);
    REQUIRE(hasSyn.size() == 1);
    REQUIRE(noSyn.empty());
    REQUIRE(hasSyn[0]->getType() == qps::query::RelRefType::USESS);
}
