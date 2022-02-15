#include "pql/evaluator.h"
#include "pql/query.h"
#include "catch.hpp"
#include "logging.h"



using namespace qps::evaluator;

#define TEST_LOG Logger() << "evaluator_tests.cpp"

TEST_CASE("Process such that") {
    std::shared_ptr<Parent> ptr1 = std::make_shared<Parent>();
    ptr1.get()->parent = StmtRef::ofLineNo(3);
    ptr1.get()->child = StmtRef::ofLineNo(6);

    std::shared_ptr<Modifies> ptr2 = std::make_shared<Modifies>();
    ptr2.get()->modifiesStmt = StmtRef::ofWildcard();
    ptr2.get()->modified = EntRef::ofWildcard();

    std::shared_ptr<Uses> ptr3 = std::make_shared<Uses>();
    ptr3.get()->useStmt = StmtRef::ofDeclaration("s1");
    ptr3.get()->used = EntRef::ofWildcard();

    std::vector<std::shared_ptr<RelRef>> suchThat1 = {ptr1};
    std::vector<std::shared_ptr<RelRef>> suchThat2 = {ptr2};
    std::vector<std::shared_ptr<RelRef>> suchThat3 = {ptr3};
    std::vector<std::shared_ptr<RelRef>> noSyn;
    std::vector<std::shared_ptr<RelRef>> hasSyn;

    PKB pkb = PKB();
    PKB* ptr = &pkb;
    Evaluator evaluator{ptr};
    evaluator.processSuchthat(suchThat1, noSyn, hasSyn);
    REQUIRE(noSyn.size() == 1);
    REQUIRE(hasSyn.empty());
    noSyn.clear();

    evaluator.processSuchthat(suchThat2, noSyn, hasSyn);
    REQUIRE(noSyn.size() == 1);
    REQUIRE(hasSyn.empty());
    REQUIRE(noSyn[0]->getType() == RelRefType::MODIFIESS);
    noSyn.clear();

    evaluator.processSuchthat(suchThat3, noSyn, hasSyn);
    REQUIRE(hasSyn.size() == 1);
    REQUIRE(noSyn.empty());
    REQUIRE(hasSyn[0]->getType() == RelRefType::USESS);
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

    Evaluator evaluator(ptr);
    Query query{};
    TEST_LOG << "log";
    query.addDeclaration("if", DesignEntity::IF);
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
    Evaluator evaluator(ptr);
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