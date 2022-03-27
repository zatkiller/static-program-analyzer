#include "PKB.h"
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
    for (auto r : table.getTable()) {
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
    PKBField field11 = PKBField::createConcrete(PROC_NAME("proc1"));
    PKBField field12 = PKBField::createConcrete(PROC_NAME("proc2"));
    PKBField field13 = PKBField::createConcrete(PROC_NAME{"proc3"});
    pkb.insertEntity(PROC_NAME{ "proc1" });
    pkb.insertEntity(PROC_NAME{ "proc2" });
    pkb.insertEntity(PROC_NAME{ "proc3" });
    pkb.insertEntity(STMT_LO{ 5, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 2, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 4, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 1, StatementType::If });
    pkb.insertEntity(STMT_LO{ 3, StatementType::While });
    pkb.insertEntity(STMT_LO{ 6, StatementType::Print });
    pkb.insertEntity(VAR_NAME{ "variable" });
    pkb.insertEntity(VAR_NAME{ "current" });
    pkb.insertEntity(VAR_NAME{ "x" });
    pkb.insertEntity(VAR_NAME{ "y" });
    pkb.insertRelationship(PKBRelationship::MODIFIES, field1, field7);
    pkb.insertRelationship(PKBRelationship::MODIFIES, field2, field7);
    pkb.insertRelationship(PKBRelationship::MODIFIES, field3, field7);
    pkb.insertRelationship(PKBRelationship::MODIFIES, field4, field7);
    pkb.insertRelationship(PKBRelationship::MODIFIES, field3, field8);
    pkb.insertRelationship(PKBRelationship::MODIFIES, field11, field8);
    pkb.insertRelationship(PKBRelationship::MODIFIES, field11, field7);
    pkb.insertRelationship(PKBRelationship::MODIFIES, field11, field9);
    pkb.insertRelationship(PKBRelationship::MODIFIES, field12, field7);

    pkb.insertRelationship(PKBRelationship::USES, field1, field9);
    pkb.insertRelationship(PKBRelationship::USES, field6, field10);
    pkb.insertRelationship(PKBRelationship::USES, field5, field8);
    pkb.insertRelationship(PKBRelationship::USES, field11, field8);
    pkb.insertRelationship(PKBRelationship::USES, field11, field9);
    pkb.insertRelationship(PKBRelationship::USES, field12, field7);
    pkb.insertRelationship(PKBRelationship::USES, field12, field10);

    pkb.insertRelationship(PKBRelationship::FOLLOWS, field4, field1);
    pkb.insertRelationship(PKBRelationship::FOLLOWS, field1, field5);
    pkb.insertRelationship(PKBRelationship::FOLLOWS, field5, field2);
    pkb.insertRelationship(PKBRelationship::FOLLOWS, field2, field3);
    pkb.insertRelationship(PKBRelationship::FOLLOWS, field3, field6);

    pkb.insertRelationship(PKBRelationship::PARENT, field4, field1);
    pkb.insertRelationship(PKBRelationship::PARENT, field5, field2);
    pkb.insertRelationship(PKBRelationship::PARENT, field5, field3);
    pkb.insertRelationship(PKBRelationship::PARENT, field5, field6);

    pkb.insertRelationship(PKBRelationship::CALLS, field11, field12);
    pkb.insertRelationship(PKBRelationship::CALLS, field12, field13);

    pkb.insertRelationship(PKBRelationship::NEXT, field4, field1);
    pkb.insertRelationship(PKBRelationship::NEXT, field1, field5);
    pkb.insertRelationship(PKBRelationship::NEXT, field5, field2);
    pkb.insertRelationship(PKBRelationship::NEXT, field2, field3);
    pkb.insertRelationship(PKBRelationship::NEXT, field4, field6);
    return pkb;
}

TEST_CASE("Test get Modifies") {
    PKB pkb = createPKB();
    PKB* pkbPtr = &pkb;

    TEST_LOG << "select a such that Modifies(a, 'variable')";
    qps::query::Query query;
    std::shared_ptr<qps::query::ModifiesS> mPtr = std::make_shared<qps::query::ModifiesS>();
    mPtr->modifiesStmt = qps::query::StmtRef::ofDeclaration("a", qps::query::DesignEntity::ASSIGN);
    mPtr->modified = qps::query::EntRef::ofVarName("variable");

    query.addVariable("a");
    query.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query.addSuchthat(mPtr);
    qps::evaluator::Evaluator evaluator = qps::evaluator::Evaluator{pkbPtr};

    std::list<std::string> result = evaluator.evaluate(query);
    printResult(result);
    result.sort();
    REQUIRE(result == std::list<std::string>{"2", "4", "5"});

    TEST_LOG << "stmt s; assign a; select s such that Modifies(a, 'variable')";
    qps::query::Query query1;
    std::shared_ptr<qps::query::ModifiesS> mPtr1 = std::make_shared<qps::query::ModifiesS>();
    mPtr1->modifiesStmt = qps::query::StmtRef::ofDeclaration("a", qps::query::DesignEntity::ASSIGN);
    mPtr1->modified = qps::query::EntRef::ofVarName("variable");
    query1.addVariable("s");
    query1.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query1.addDeclaration("s", qps::query::DesignEntity::STMT);
    query1.addSuchthat(mPtr1);
    qps::evaluator::Evaluator evaluator1 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result1 = evaluator1.evaluate(query1);
    printResult(result1);
    result1.sort();
    REQUIRE(result1 == std::list<std::string>{"1", "2", "3", "4", "5", "6"});

    TEST_LOG << "select v such that Modifies(5, v)";
    qps::query::Query query2;
    query2.addVariable("v");
    query2.addDeclaration("v", qps::query::DesignEntity::VARIABLE);
    std::shared_ptr<qps::query::ModifiesS> mPtr2 = std::make_shared<qps::query::ModifiesS>();
    mPtr2->modifiesStmt = qps::query::StmtRef::ofLineNo(5);
    mPtr2->modified = qps::query::EntRef::ofDeclaration("v", qps::query::DesignEntity::VARIABLE);
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
    std::shared_ptr<qps::query::ModifiesS> mPtr3 = std::make_shared<qps::query::ModifiesS>();
    mPtr3->modifiesStmt = qps::query::StmtRef::ofDeclaration("s", qps::query::DesignEntity::STMT);
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
    std::shared_ptr<qps::query::ModifiesS> mPtr4 = std::make_shared<qps::query::ModifiesS>();
    mPtr4->modifiesStmt = qps::query::StmtRef::ofLineNo(3);
    mPtr4->modified = qps::query::EntRef::ofDeclaration("v", qps::query::DesignEntity::VARIABLE);
    query4.addSuchthat(mPtr4);
    qps::evaluator::Evaluator evaluator4 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result4 = evaluator4.evaluate(query4);
    printResult(result4);
    result4.sort();
    REQUIRE(result4.empty());

    TEST_LOG << "select p such that ModifiesP(p, 'variable')";
    qps::query::Query query5;
    std::shared_ptr<qps::query::ModifiesP> mPtr5 = std::make_shared<qps::query::ModifiesP>();
    mPtr5->modifiesProc = qps::query::EntRef::ofDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    mPtr5->modified = qps::query::EntRef::ofVarName("variable");

    query5.addVariable("p");
    query5.addDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    query5.addSuchthat(mPtr5);
    qps::evaluator::Evaluator evaluator5 = qps::evaluator::Evaluator{pkbPtr};

    std::list<std::string> result5 = evaluator5.evaluate(query5);
    printResult(result5);
    result5.sort();
    REQUIRE(result5 == std::list<std::string>{"proc1", "proc2"});

    TEST_LOG << "select v such that ModifiesP('proc1', v)";
    qps::query::Query query6;
    std::shared_ptr<qps::query::ModifiesP> mPtr6 = std::make_shared<qps::query::ModifiesP>();
    mPtr6->modifiesProc = qps::query::EntRef::ofVarName("proc1");
    mPtr6->modified = qps::query::EntRef::ofDeclaration("v", qps::query::DesignEntity::VARIABLE);

    query6.addVariable("v");
    query6.addDeclaration("v", qps::query::DesignEntity::VARIABLE);
    query6.addSuchthat(mPtr6);
    qps::evaluator::Evaluator evaluator6 = qps::evaluator::Evaluator{pkbPtr};

    std::list<std::string> result6 = evaluator6.evaluate(query6);
    printResult(result6);
    result6.sort();
    REQUIRE(result6 == std::list<std::string>{"current", "variable", "x"});
}

TEST_CASE("Test get Uses") {
    PKB pkb = createPKB();
    PKB* pkbPtr = &pkb;

    TEST_LOG << "select s such that Uses(s, 'x')";
    qps::query::Query query1;
    query1.addDeclaration("s", qps::query::DesignEntity::STMT);
    query1.addVariable("s");
    std::shared_ptr<qps::query::UsesS> mPtr1 = std::make_shared<qps::query::UsesS>();
    mPtr1->useStmt = qps::query::StmtRef::ofDeclaration("s", qps::query::DesignEntity::STMT);
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
    std::shared_ptr<qps::query::UsesS> mPtr2 = std::make_shared<qps::query::UsesS>();
    mPtr2->useStmt = qps::query::StmtRef::ofDeclaration("p", qps::query::DesignEntity::PRINT);
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
    std::shared_ptr<qps::query::UsesS> mPtr3 = std::make_shared<qps::query::UsesS>();
    mPtr3->useStmt = qps::query::StmtRef::ofDeclaration("s", qps::query::DesignEntity::STMT);
    mPtr3->used = qps::query::EntRef::ofDeclaration("v", qps::query::DesignEntity::VARIABLE);
    query3.addSuchthat(mPtr3);
    qps::evaluator::Evaluator e3 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result3 = e3.evaluate(query3);
    result3.sort();
    printResult(result3);
    REQUIRE(result3 == std::list<std::string>{"current", "x", "y"});

    TEST_LOG << "select v such that UsesP('proc1', v)";
    qps::query::Query query4;
    query4.addDeclaration("v", qps::query::DesignEntity::VARIABLE);
    query4.addVariable("v");
    std::shared_ptr<qps::query::UsesP> mPtr4 = std::make_shared<qps::query::UsesP>();
    mPtr4->useProc = qps::query::EntRef::ofVarName("proc1");
    mPtr4->used = qps::query::EntRef::ofDeclaration("v", qps::query::DesignEntity::VARIABLE);
    query4.addSuchthat(mPtr4);
    qps::evaluator::Evaluator evaluator4 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result4 = evaluator4.evaluate(query4);
    printResult(result4);
    result4.sort();
    REQUIRE(result4 == std::list<std::string>{"current", "x"});

    TEST_LOG << "select p such that UsesP(p, 'variable')";
    qps::query::Query query5;
    std::shared_ptr<qps::query::UsesP> mPtr5 = std::make_shared<qps::query::UsesP>();
    mPtr5->useProc = qps::query::EntRef::ofDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    mPtr5->used = qps::query::EntRef::ofVarName("variable");

    query5.addVariable("p");
    query5.addDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    query5.addSuchthat(mPtr5);
    qps::evaluator::Evaluator evaluator5 = qps::evaluator::Evaluator{pkbPtr};

    std::list<std::string> result5 = evaluator5.evaluate(query5);
    printResult(result5);
    result5.sort();
    REQUIRE(result5 == std::list<std::string>{"proc2"});

    TEST_LOG << "select p such that ModifiesP(p, _)";
    qps::query::Query query6;
    std::shared_ptr<qps::query::UsesP> mPtr6 = std::make_shared<qps::query::UsesP>();
    mPtr6->useProc = qps::query::EntRef::ofDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    mPtr6->used = qps::query::EntRef::ofWildcard();

    query6.addVariable("p");
    query6.addDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    query6.addSuchthat(mPtr6);
    qps::evaluator::Evaluator evaluator6 = qps::evaluator::Evaluator{pkbPtr};

    std::list<std::string> result6 = evaluator6.evaluate(query6);
    printResult(result6);
    result6.sort();
    REQUIRE(result6 == std::list<std::string>{"proc1", "proc2"});
}

TEST_CASE("Test get Follows FollowsT") {
    PKB pkb = createPKB();
    PKB* pkbPtr = &pkb;

    TEST_LOG << "select s such that Follows(s, 3)";
    qps::query::Query query1;
    query1.addDeclaration("s", qps::query::DesignEntity::STMT);
    query1.addVariable("s");
    std::shared_ptr<qps::query::Follows> mPtr1 = std::make_shared<qps::query::Follows>();
    mPtr1->follower = qps::query::StmtRef::ofDeclaration("s", qps::query::DesignEntity::STMT);
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
    mPtr2->transitiveFollowed = qps::query::StmtRef::ofDeclaration("s", qps::query::DesignEntity::STMT);
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
    mPtr3->transitiveFollowed = qps::query::StmtRef::ofDeclaration("s", qps::query::DesignEntity::STMT);
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
    mPtr4->follower = qps::query::StmtRef::ofDeclaration("s", qps::query::DesignEntity::STMT);
    mPtr4->followed = qps::query::StmtRef::ofDeclaration("s", qps::query::DesignEntity::STMT);
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
    mPtr1->child = qps::query::StmtRef::ofDeclaration("a", qps::query::DesignEntity::ASSIGN);
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
    mPtr2->parent = qps::query::StmtRef::ofDeclaration("a", qps::query::DesignEntity::ASSIGN);
    mPtr2->child = qps::query::StmtRef::ofDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query2.addSuchthat(mPtr2);
    qps::evaluator::Evaluator e2 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result2 = e2.evaluate(query2);
    printResult(result2);
    REQUIRE(result2.empty());
}

TEST_CASE("Test Next and NextT") {
    PKB pkb = createPKB();
    PKB* pkbPtr = &pkb;

    TEST_LOG << "select s such that Next(1, s)";
    qps::query::Query query1;
    query1.addDeclaration("s", qps::query::DesignEntity::STMT);
    query1.addVariable("s");
    std::shared_ptr<qps::query::Next> mPtr1 = std::make_shared<qps::query::Next>();
    mPtr1->before = qps::query::StmtRef::ofLineNo(1);
    mPtr1->after = qps::query::StmtRef::ofDeclaration("s", qps::query::DesignEntity::STMT);
    query1.addSuchthat(mPtr1);
    qps::evaluator::Evaluator e1 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result1 = e1.evaluate(query1);
    result1.sort();
    printResult(result1);
    REQUIRE(result1 == std::list<std::string>{"2", "6"});

    TEST_LOG << "select s such that NextT(2, s)";
    qps::query::Query query2;
    query2.addDeclaration("s", qps::query::DesignEntity::STMT);
    query2.addVariable("s");
    std::shared_ptr<qps::query::NextT> mPtr2 = std::make_shared<qps::query::NextT>();
    mPtr2->before = qps::query::StmtRef::ofLineNo(2);
    mPtr2->transitiveAfter = qps::query::StmtRef::ofDeclaration("s", qps::query::DesignEntity::STMT);
    query2.addSuchthat(mPtr2);
    qps::evaluator::Evaluator e2 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result2 = e2.evaluate(query2);
    printResult(result2);
    result2.sort();
    REQUIRE(result2 == std::list<std::string>{"3", "4", "5"});

    TEST_LOG << "select a such that NextT(a, _)";
    qps::query::Query query3;
    query3.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query3.addVariable("a");
    std::shared_ptr<qps::query::NextT> mPtr3 = std::make_shared<qps::query::NextT>();
    mPtr3->before = qps::query::StmtRef::ofDeclaration("a", qps::query::DesignEntity::ASSIGN);
    mPtr3->transitiveAfter = qps::query::StmtRef::ofWildcard();
    query3.addSuchthat(mPtr3);
    qps::evaluator::Evaluator e3 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result3 = e3.evaluate(query3);
    result3.sort();
    printResult(result3);
    REQUIRE(result3 == std::list<std::string>{"2", "4"});
}

TEST_CASE("Test get Calls and CallsT") {
    PKB pkb = createPKB();
    PKB* pkbPtr = &pkb;

    TEST_LOG << "select p such that Calls(p, 'proc2')";
    qps::query::Query query1;
    query1.addDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    query1.addVariable("p");
    std::shared_ptr<qps::query::Calls> mPtr1 = std::make_shared<qps::query::Calls>();
    mPtr1->caller = qps::query::EntRef::ofDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    mPtr1->callee = qps::query::EntRef::ofVarName("proc2");
    query1.addSuchthat(mPtr1);
    qps::evaluator::Evaluator e1 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result1 = e1.evaluate(query1);
    result1.sort();
    printResult(result1);
    REQUIRE(result1 == std::list<std::string>{"proc1"});

    TEST_LOG << "select p such that CallsT('proc1', p)";
    qps::query::Query query2;
    query2.addDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    query2.addVariable("p");
    std::shared_ptr<qps::query::CallsT> mPtr2 = std::make_shared<qps::query::CallsT>();
    mPtr2->caller = qps::query::EntRef::ofVarName("proc1");
    mPtr2->transitiveCallee = qps::query::EntRef::ofDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    query2.addSuchthat(mPtr2);
    qps::evaluator::Evaluator e2 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result2 = e2.evaluate(query2);
    printResult(result2);
    result2.sort();
    REQUIRE(result2 == std::list<std::string>{"proc2", "proc3"});

    TEST_LOG << "select p such that CallsT(p, _)";
    qps::query::Query query3;
    query3.addDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    query3.addVariable("p");
    std::shared_ptr<qps::query::CallsT> mPtr3 = std::make_shared<qps::query::CallsT>();
    mPtr3->caller = qps::query::EntRef::ofDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    mPtr3->transitiveCallee = qps::query::EntRef::ofWildcard();
    query3.addSuchthat(mPtr3);
    qps::evaluator::Evaluator e3 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result3 = e3.evaluate(query3);
    result3.sort();
    printResult(result3);
    REQUIRE(result3 == std::list<std::string>{"proc1", "proc2"});
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
    std::shared_ptr<qps::query::ModifiesS> mPtr3 = std::make_shared<qps::query::ModifiesS>();
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
    std::shared_ptr<qps::query::UsesS> mPtr4 = std::make_shared<qps::query::UsesS>();
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
    std::shared_ptr<qps::query::UsesS> mPtr5 = std::make_shared<qps::query::UsesS>();
    mPtr5->useStmt = qps::query::StmtRef::ofLineNo(5);
    mPtr5->used = qps::query::EntRef::ofWildcard();
    query5.addSuchthat(mPtr5);
    qps::evaluator::Evaluator e5 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result5 = e5.evaluate(query5);
    printResult(result5);
    REQUIRE(result5.empty());
}

TEST_CASE("multiple relationship clauses") {
    PKB pkb = createPKB();
    PKB* pkbPtr = &pkb;

    TEST_LOG << "select v such that Parent(3, a) and Modifies(a, v)";
    qps::query::Query query1;
    query1.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query1.addDeclaration("v", qps::query::DesignEntity::VARIABLE);
    query1.addVariable("v");
    std::shared_ptr<qps::query::Parent> mPtr11 = std::make_shared<qps::query::Parent>();
    mPtr11->parent = qps::query::StmtRef::ofLineNo(3);
    mPtr11->child = qps::query::StmtRef::ofDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query1.addSuchthat(mPtr11);
    std::shared_ptr<qps::query::ModifiesS> mPtr12 = std::make_shared<qps::query::ModifiesS>();
    mPtr12->modifiesStmt = qps::query::StmtRef::ofDeclaration("a", qps::query::DesignEntity::ASSIGN);
    mPtr12->modified = qps::query::EntRef::ofDeclaration("v", qps::query::DesignEntity::VARIABLE);
    query1.addSuchthat(mPtr12);
    qps::evaluator::Evaluator e1 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result1 = e1.evaluate(query1);
    result1.sort();
    printResult(result1);
    REQUIRE(result1 == std::list<std::string>{"current", "variable"});

    TEST_LOG << "select s such that CallsT('proc1', p) UsesP(p, v) Uses(s, v)";
    qps::query::Query query2;
    query2.addDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    query2.addDeclaration("v", qps::query::DesignEntity::VARIABLE);
    query2.addDeclaration("s", qps::query::DesignEntity::STMT);
    query2.addVariable("s");
    std::shared_ptr<qps::query::CallsT> mPtr21 = std::make_shared<qps::query::CallsT>();
    mPtr21->caller = qps::query::EntRef::ofVarName("proc1");
    mPtr21->transitiveCallee = qps::query::EntRef::ofDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    query2.addSuchthat(mPtr21);
    std::shared_ptr<qps::query::UsesP> mPtr22 = std::make_shared<qps::query::UsesP>();
    mPtr22->useProc = qps::query::EntRef::ofDeclaration("p", qps::query::DesignEntity::PROCEDURE);
    mPtr22->used = qps::query::EntRef::ofDeclaration("v", qps::query::DesignEntity::VARIABLE);
    query2.addSuchthat(mPtr22);
    std::shared_ptr<qps::query::UsesS> mPtr23 = std::make_shared<qps::query::UsesS>();
    mPtr23->useStmt = qps::query::StmtRef::ofDeclaration("s", qps::query::DesignEntity::VARIABLE);
    mPtr23->used = qps::query::EntRef::ofDeclaration("v", qps::query::DesignEntity::VARIABLE);
    query2.addSuchthat(mPtr23);
    qps::evaluator::Evaluator e2 = qps::evaluator::Evaluator{pkbPtr};
    std::list<std::string> result2 = e2.evaluate(query2);
    printResult(result2);
    result2.sort();
    REQUIRE(result2 == std::list<std::string>{"6"});
}
