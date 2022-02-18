#include "PKB.h"
#include "PKB/StatementType.h"
#include "PKB/PKBDataTypes.h"
#include "PKB/PKBField.h"
#include "pql/evaluator.h"
#include "pql/query.h"
#include "catch.hpp"
#include "logging.h"

#define TEST_LOG Logger() << "TestEvaluateRelationships.cpp "

void printResult(std::list<std::string> result) {
    for (auto s : result) {
        TEST_LOG << s;
    }
}

void printTable(qps::evaluator::ResultTable table) {
    for (auto r : table.getResult()) {
        std::string record;
        for (auto f : r) {
            record = record + qps::evaluator::Evaluator::PKBFieldToString(f) + " ";
        }
        TEST_LOG << record;
    }
}

PKB createPKB() {
    PKB pkb = PKB();

    PKBField field1 = PKBField::createConcrete(STMT_LO{2, StatementType::Assignment});
    PKBField field2 = PKBField::createConcrete(STMT_LO{4, StatementType::Assignment});
    PKBField field3 = PKBField::createConcrete(STMT_LO{5, StatementType::Assignment});
    PKBField field4 = PKBField::createConcrete(STMT_LO{1, StatementType::If});
    PKBField field5 = PKBField::createConcrete(STMT_LO{3, StatementType::While});
    PKBField field6 = PKBField::createConcrete(STMT_LO{6, StatementType::Print});
    PKBField field7 = PKBField::createConcrete(VAR_NAME{"variable"});
    PKBField field8 = PKBField::createConcrete(VAR_NAME("current"));
    PKBField field9 = PKBField::createConcrete(VAR_NAME("x"));
    PKBField field10 = PKBField::createConcrete(VAR_NAME("y"));
    pkb.insertStatement(StatementType::Assignment, 5);
    pkb.insertStatement(StatementType::Assignment, 2);
    pkb.insertStatement(StatementType::Assignment, 4);
    pkb.insertStatement(StatementType::If, 1);
    pkb.insertStatement(StatementType::While, 3);
    pkb.insertStatement(StatementType::Print, 6);
    pkb.insertVariable("variable");
    pkb.insertVariable("current");
    pkb.insertVariable("x");
    pkb.insertVariable("y");
    pkb.insertRelationship(PKBRelationship::MODIFIES, field1, field7);
    pkb.insertRelationship(PKBRelationship::MODIFIES, field2, field7);
    pkb.insertRelationship(PKBRelationship::MODIFIES, field3, field7);
    pkb.insertRelationship(PKBRelationship::MODIFIES, field4, field7);
    pkb.insertRelationship(PKBRelationship::MODIFIES, field3, field8);
    pkb.insertRelationship(PKBRelationship::USES, field1, field9);
    pkb.insertRelationship(PKBRelationship::USES, field6, field10);
    pkb.insertRelationship(PKBRelationship::USES, field5, field8);
    pkb.insertRelationship(PKBRelationship::FOLLOWS, field4, field1);
    pkb.insertRelationship(PKBRelationship::FOLLOWS, field1, field5);
    pkb.insertRelationship(PKBRelationship::FOLLOWS, field5, field2);
    pkb.insertRelationship(PKBRelationship::FOLLOWS, field2, field3);
    pkb.insertRelationship(PKBRelationship::FOLLOWS, field3, field6);
    pkb.insertRelationship(PKBRelationship::PARENT, field4, field1);
    pkb.insertRelationship(PKBRelationship::PARENT, field5, field2);
    pkb.insertRelationship(PKBRelationship::PARENT, field5, field3);
    pkb.insertRelationship(PKBRelationship::PARENT, field5, field6);
    return pkb;
}

TEST_CASE("Test get Modifies") {
    PKB pkb = createPKB();
    PKB* pkbPtr = &pkb;

    TEST_LOG << "select a such that Modifies(a, 'variable')";
    qps::query::Query query;
    std::shared_ptr<qps::query::Modifies> mPtr = std::make_shared<qps::query::Modifies>();
    mPtr->modifiesStmt = qps::query::StmtRef::ofDeclaration("a");
    mPtr->modified = qps::query::EntRef::ofVarName("variable");

    query.addVariable("a");
    query.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query.addSuchthat(mPtr);
    qps::evaluator::Evaluator evaluator = qps::evaluator::Evaluator{pkbPtr};

    std::list<std::string> result = evaluator.evaluate(query);
    printResult(result);
    result.sort();
    REQUIRE(result == std::list<std::string>{"2", "4", "5"});

    TEST_LOG << "select v such that Modifies(5, v)";
    qps::query::Query query2;
    query2.addVariable("v");
    query2.addDeclaration("v", qps::query::DesignEntity::VARIABLE);
    std::shared_ptr<qps::query::Modifies> mPtr2 = std::make_shared<qps::query::Modifies>();
    mPtr2->modifiesStmt = qps::query::StmtRef::ofLineNo(5);
    mPtr2->modified = qps::query::EntRef::ofDeclaration("v");
    query2.addSuchthat(mPtr2);
    qps::evaluator::Evaluator evaluator2 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result2 = evaluator2.evaluate(query2);
    printResult(result2);
    result2.sort();
    REQUIRE(result2 == std::list<std::string>{"current", "variable"});

    TEST_LOG << "select s such that Modifies(s, _)";
    qps::query::Query query3;
    query3.addDeclaration("s", qps::query::DesignEntity::STMT);
    query3.addVariable("s");
    std::shared_ptr<qps::query::Modifies> mPtr3 = std::make_shared<qps::query::Modifies>();
    mPtr3->modifiesStmt = qps::query::StmtRef::ofDeclaration("s");
    mPtr3->modified = qps::query::EntRef::ofWildcard();
    query3.addSuchthat(mPtr3);
    qps::evaluator::Evaluator evaluator3 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result3 = evaluator3.evaluate(query3);
    printResult(result3);
    result3.sort();
    REQUIRE(result3 == std::list<std::string>{"1", "2", "4", "5"});

    TEST_LOG << "select v such that Modifies(3, v)";
    qps::query::Query query4;
    query4.addDeclaration("v", qps::query::DesignEntity::VARIABLE);
    query4.addVariable("v");
    std::shared_ptr<qps::query::Modifies> mPtr4 = std::make_shared<qps::query::Modifies>();
    mPtr4->modifiesStmt = qps::query::StmtRef::ofLineNo(3);
    mPtr4->modified = qps::query::EntRef::ofDeclaration("v");
    query4.addSuchthat(mPtr4);
    qps::evaluator::Evaluator evaluator4 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result4 = evaluator3.evaluate(query4);
    printResult(result4);
    result4.sort();
    REQUIRE(result4.empty());
}

TEST_CASE("Test get Uses") {
    PKB pkb = createPKB();
    PKB* pkbPtr = &pkb;

    TEST_LOG << "select s such that Uses(s, 'x')";
    qps::query::Query query1;
    query1.addDeclaration("s", qps::query::DesignEntity::STMT);
    query1.addVariable("s");
    std::shared_ptr<qps::query::Uses> mPtr1 = std::make_shared<qps::query::Uses>();
    mPtr1->useStmt = qps::query::StmtRef::ofDeclaration("s");
    mPtr1->used = qps::query::EntRef::ofVarName("x");
    query1.addSuchthat(mPtr1);
    qps::evaluator::Evaluator e1 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result1 = e1.evaluate(query1);
    result1.sort();
    printResult(result1);
    REQUIRE(result1 == std::list<std::string>{"2"});

    TEST_LOG << "select p such that Uses(p, _)";
    qps::query::Query query2;
    query2.addDeclaration("p", qps::query::DesignEntity::PRINT);
    query2.addVariable("p");
    std::shared_ptr<qps::query::Uses> mPtr2 = std::make_shared<qps::query::Uses>();
    mPtr2->useStmt = qps::query::StmtRef::ofDeclaration("p");
    mPtr2->used = qps::query::EntRef::ofWildcard();
    query2.addSuchthat(mPtr2);
    qps::evaluator::Evaluator e2 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result2 = e2.evaluate(query2);
    result2.sort();
    printResult(result2);
    REQUIRE(result2 == std::list<std::string>{"6"});

    TEST_LOG << "select v such that Uses(s, v)";
    qps::query::Query query3;
    query3.addDeclaration("v", qps::query::DesignEntity::VARIABLE);
    query3.addDeclaration("s", qps::query::DesignEntity::STMT);
    query3.addVariable("v");
    query3.addVariable("s");
    std::shared_ptr<qps::query::Uses> mPtr3 = std::make_shared<qps::query::Uses>();
    mPtr3->useStmt = qps::query::StmtRef::ofDeclaration("s");
    mPtr3->used = qps::query::EntRef::ofDeclaration("v");
    query3.addSuchthat(mPtr3);
    qps::evaluator::Evaluator e3 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result3 = e3.evaluate(query3);
    result3.sort();
    printResult(result3);
    REQUIRE(result3 == std::list<std::string>{"current", "x", "y"});
}

TEST_CASE("Test get Follows FollowsT") {
    PKB pkb = createPKB();
    PKB* pkbPtr = &pkb;

    TEST_LOG << "select s such that Follows(s, 3)";
    qps::query::Query query1;
    query1.addDeclaration("s", qps::query::DesignEntity::STMT);
    query1.addVariable("s");
    std::shared_ptr<qps::query::Follows> mPtr1 = std::make_shared<qps::query::Follows>();
    mPtr1->follower = qps::query::StmtRef::ofDeclaration("s");
    mPtr1->followed = qps::query::StmtRef::ofLineNo(3);
    query1.addSuchthat(mPtr1);
    qps::evaluator::Evaluator e1 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result1 = e1.evaluate(query1);
    result1.sort();
    printResult(result1);
    REQUIRE(result1 == std::list<std::string>{"2"});

    TEST_LOG << "select s such that FollowsT(3, s)";
    qps::query::Query query2;
    query2.addDeclaration("s", qps::query::DesignEntity::STMT);
    query2.addVariable("s");
    std::shared_ptr<qps::query::FollowsT> mPtr2 = std::make_shared<qps::query::FollowsT>();
    mPtr2->follower = qps::query::StmtRef::ofLineNo(3);
    mPtr2->transitiveFollowed = qps::query::StmtRef::ofDeclaration("s");
    query2.addSuchthat(mPtr2);
    qps::evaluator::Evaluator e2 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result2 = e2.evaluate(query2);
    result2.sort();
    printResult(result2);
    REQUIRE(result2 == std::list<std::string>{"4", "5", "6"});

    TEST_LOG << "select s such that FollowsT(_, s)";
    qps::query::Query query3;
    query3.addDeclaration("s", qps::query::DesignEntity::STMT);
    query3.addVariable("s");
    std::shared_ptr<qps::query::FollowsT> mPtr3 = std::make_shared<qps::query::FollowsT>();
    mPtr3->follower = qps::query::StmtRef::ofWildcard();
    mPtr3->transitiveFollowed = qps::query::StmtRef::ofDeclaration("s");
    query3.addSuchthat(mPtr3);
    qps::evaluator::Evaluator e3 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result3 = e3.evaluate(query3);
    result3.sort();
    printResult(result3);
    REQUIRE(result3 == std::list<std::string>{"2", "3", "4", "5", "6"});

    TEST_LOG << "select s such that Follows(s, s)";
    qps::query::Query query4;
    query4.addDeclaration("s", qps::query::DesignEntity::STMT);
    query4.addVariable("s");
    std::shared_ptr<qps::query::Follows> mPtr4 = std::make_shared<qps::query::Follows>();
    mPtr4->follower = qps::query::StmtRef::ofDeclaration("s");
    mPtr4->followed = qps::query::StmtRef::ofDeclaration("s");
    query4.addSuchthat(mPtr4);
    qps::evaluator::Evaluator e4 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result4 = e3.evaluate(query4);
    result4.sort();
    printResult(result4);
    REQUIRE(result4.empty());
}

TEST_CASE("Test Parent") {
    PKB pkb = createPKB();
    PKB* pkbPtr = &pkb;

    TEST_LOG << "select a such that Parent(3, a)";
    qps::query::Query query1;
    query1.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query1.addVariable("a");
    std::shared_ptr<qps::query::Parent> mPtr1 = std::make_shared<qps::query::Parent>();
    mPtr1->parent = qps::query::StmtRef::ofLineNo(3);
    mPtr1->child = qps::query::StmtRef::ofDeclaration("a");
    query1.addSuchthat(mPtr1);
    qps::evaluator::Evaluator e1 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result1 = e1.evaluate(query1);
    result1.sort();
    printResult(result1);
    REQUIRE(result1 == std::list<std::string>{"4", "5"});

    TEST_LOG << "select a such that Parent(a, a)";
    qps::query::Query query2;
    query2.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query2.addVariable("a");
    std::shared_ptr<qps::query::Parent> mPtr2 = std::make_shared<qps::query::Parent>();
    mPtr2->parent = qps::query::StmtRef::ofDeclaration("a");
    mPtr2->child = qps::query::StmtRef::ofDeclaration("a");
    query2.addSuchthat(mPtr2);
    qps::evaluator::Evaluator e2 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result2 = e2.evaluate(query2);
    printResult(result2);
    REQUIRE(result2.empty());
}

TEST_CASE("Test isRelationshipPresent") {
    PKB pkb = createPKB();
    PKB* pkbPtr = &pkb;

    TEST_LOG << "select a such that Parent(3, 2)";
    qps::query::Query query1;
    query1.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query1.addVariable("a");
    std::shared_ptr<qps::query::Parent> mPtr1 = std::make_shared<qps::query::Parent>();
    mPtr1->parent = qps::query::StmtRef::ofLineNo(3);
    mPtr1->child = qps::query::StmtRef::ofLineNo(2);
    query1.addSuchthat(mPtr1);
    qps::evaluator::Evaluator e1 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result1 = e1.evaluate(query1);
    result1.sort();
    printResult(result1);
    REQUIRE(result1.empty());

    TEST_LOG << "select s such that FollowsT(3, 6)";
    qps::query::Query query2;
    query2.addDeclaration("s", qps::query::DesignEntity::STMT);
    query2.addVariable("s");
    std::shared_ptr<qps::query::FollowsT> mPtr2 = std::make_shared<qps::query::FollowsT>();
    mPtr2->follower = qps::query::StmtRef::ofLineNo(3);
    mPtr2->transitiveFollowed = qps::query::StmtRef::ofLineNo(6);
    query2.addSuchthat(mPtr2);
    qps::evaluator::Evaluator e2 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result2 = e2.evaluate(query2);
    result2.sort();
    printResult(result2);
    REQUIRE(result2 == std::list<std::string>{"1", "2", "3", "4", "5", "6"});

    TEST_LOG << "select v such that Modifies(2, 'variable')";
    qps::query::Query query3;
    query3.addDeclaration("v", qps::query::DesignEntity::VARIABLE);
    query3.addVariable("v");
    std::shared_ptr<qps::query::Modifies> mPtr3 = std::make_shared<qps::query::Modifies>();
    mPtr3->modifiesStmt = qps::query::StmtRef::ofLineNo(2);
    mPtr3->modified = qps::query::EntRef::ofVarName("variable");
    query3.addSuchthat(mPtr3);
    qps::evaluator::Evaluator e3 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result3 = e3.evaluate(query3);
    printResult(result3);
    result3.sort();
    REQUIRE(result3 == std::list<std::string>{"current", "variable", "x", "y"});

    TEST_LOG << "select s such that Uses(6, _)";
    qps::query::Query query4;
    query4.addDeclaration("s", qps::query::DesignEntity::STMT);
    query4.addVariable("s");
    std::shared_ptr<qps::query::Uses> mPtr4 = std::make_shared<qps::query::Uses>();
    mPtr4->useStmt = qps::query::StmtRef::ofLineNo(6);
    mPtr4->used = qps::query::EntRef::ofWildcard();
    query4.addSuchthat(mPtr4);
    qps::evaluator::Evaluator e4 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result4 = e4.evaluate(query4);
    printResult(result4);
    result4.sort();
    REQUIRE(result4 == std::list<std::string>{"1", "2", "3", "4", "5", "6"});

    TEST_LOG << "select s such that Uses(5, _)";
    qps::query::Query query5;
    query5.addDeclaration("s", qps::query::DesignEntity::STMT);
    query5.addVariable("s");
    std::shared_ptr<qps::query::Uses> mPtr5 = std::make_shared<qps::query::Uses>();
    mPtr5->useStmt = qps::query::StmtRef::ofLineNo(5);
    mPtr5->used = qps::query::EntRef::ofWildcard();
    query5.addSuchthat(mPtr5);
    qps::evaluator::Evaluator e5 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result5 = e5.evaluate(query5);
    printResult(result5);
    REQUIRE(result5.empty());
}
