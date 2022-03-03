#include "pql/evaluator.h"
#include "pql/query.h"
#include "catch.hpp"
#include "logging.h"


#define TEST_LOG Logger() << "evaluator_tests.cpp"

TEST_CASE("Process such that") {
    std::shared_ptr<qps::query::Parent> ptr1 = std::make_shared<qps::query::Parent>();
    ptr1.get()->parent = qps::query::StmtRef::ofLineNo(3);
    ptr1.get()->child = qps::query::StmtRef::ofLineNo(6);

    std::shared_ptr<qps::query::Modifies> ptr2 = std::make_shared<qps::query::Modifies>();
    ptr2.get()->modifiesStmt = qps::query::StmtRef::ofWildcard();
    ptr2.get()->modified = qps::query::EntRef::ofWildcard();

    std::shared_ptr<qps::query::Uses> ptr3 = std::make_shared<qps::query::Uses>();
    ptr3.get()->useStmt = qps::query::StmtRef::ofDeclaration("s1");
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

TEST_CASE("Get list of result") {
    PKB pkb = PKB();
    PKB* ptr = &pkb;
    pkb.insertStatement(StatementType::Assignment, 2);
    pkb.insertStatement(StatementType::Assignment, 5);
    pkb.insertStatement(StatementType::Assignment, 8);
    pkb.insertStatement(StatementType::While, 3);
    pkb.insertStatement(StatementType::If, 4);
    pkb.insertStatement(StatementType::If, 6);
    pkb.insertStatement(StatementType::Print, 7);

    qps::evaluator::Evaluator evaluator(ptr);
    qps::query::Query query{};
    TEST_LOG << "log";
    query.addDeclaration("if", qps::query::DesignEntity::IF);
    query.addVariable("if");
    REQUIRE(query.getSuchthat().empty());
    std::list<std::string> result = evaluator.evaluate(query);

    REQUIRE(2 == result.size());
    for (auto r : result) {
        TEST_LOG << r;
    }
}

TEST_CASE("PKBField to string") {
    PKB pkb = PKB();
    PKB* ptr = &pkb;
    qps::evaluator::Evaluator evaluator(ptr);
    PKBField field1 = PKBField::createConcrete(VAR_NAME{"main"});
    PKBField field2 = PKBField::createConcrete(VAR_NAME{"a"});
    PKBField field3 = PKBField::createConcrete(STMT_LO{5, StatementType::While});
    PKBField field4 = PKBField::createConcrete(STMT_LO{1, StatementType::Assignment});
    PKBField field5 = PKBField::createConcrete(CONST{87});
    PKBField field6 = PKBField::createConcrete(PROC_NAME{"maven"});
    REQUIRE("main" == evaluator.PKBFieldToString(field1));
    REQUIRE("a" == evaluator.PKBFieldToString(field2));
    REQUIRE("5" == evaluator.PKBFieldToString(field3));
    REQUIRE("1" == evaluator.PKBFieldToString(field4));
    REQUIRE("87" == evaluator.PKBFieldToString(field5));
    REQUIRE("maven" == evaluator.PKBFieldToString(field6));
}
