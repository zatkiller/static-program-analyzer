#include "PKB.h"
#include "PKB/PKBField.h"
#include "pql/evaluator.h"
#include "pql/query.h"
#include "catch.hpp"

TEST_CASE("Test get statements") {
    PKB pkb = PKB();
    PKB* ptr = &pkb;
    qps::evaluator::Evaluator evaluator = qps::evaluator::Evaluator(ptr);

    pkb.insertEntity(STMT_LO{ 2, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 5, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 8, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 3, StatementType::While });
    pkb.insertEntity(STMT_LO{ 4, StatementType::If });
    pkb.insertEntity(STMT_LO{ 6, StatementType::If });
    pkb.insertEntity(STMT_LO{ 7, StatementType::Print });

    PKBField a1 = PKBField::createConcrete(STMT_LO{ 2, StatementType::Assignment });
    PKBField a2 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });
    PKBField a3 = PKBField::createConcrete(STMT_LO{ 8, StatementType::Assignment });
    PKBField w1 = PKBField::createConcrete(STMT_LO{ 3, StatementType::While });
    PKBField i1 = PKBField::createConcrete(STMT_LO{ 4, StatementType::If });
    PKBField i2 = PKBField::createConcrete(STMT_LO{ 6, StatementType::If });
    PKBField p1 = PKBField::createConcrete(STMT_LO{ 7, StatementType::Print });

    qps::query::Query query1;
    qps::query::Declaration d1 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple1 { qps::query::Elem::ofDeclaration(d1) };
    qps::query::ResultCl r1 = qps::query::ResultCl::ofTuple(tuple1);
    query1.addResultCl(r1);

    query1.addDeclaration("s", qps::query::DesignEntity::STMT);
    std::list<std::string> resAll = evaluator.evaluate(query1);
    resAll.sort();
    REQUIRE(resAll == std::list<std::string>{"2", "3", "4", "5", "6", "7", "8"});

    qps::query::Query query2;
    query2.addDeclaration("a", qps::query::DesignEntity::ASSIGN);

    qps::query::Declaration d2 = qps::query::Declaration { "a", qps::query::DesignEntity::ASSIGN };
    std::vector<qps::query::Elem> tuple2 { qps::query::Elem::ofDeclaration(d2) };
    qps::query::ResultCl r2 = qps::query::ResultCl::ofTuple(tuple2);
    query2.addResultCl(r2);

    std::list<std::string> resAss = evaluator.evaluate(query2);
    resAss.sort();
    REQUIRE(resAss == std::list<std::string>{"2", "5", "8"});

    qps::query::Query query3;
    query3.addDeclaration("w", qps::query::DesignEntity::WHILE);

    qps::query::Declaration d3 = qps::query::Declaration { "w", qps::query::DesignEntity::WHILE };
    std::vector<qps::query::Elem> tuple3 { qps::query::Elem::ofDeclaration(d3) };
    qps::query::ResultCl r3 = qps::query::ResultCl::ofTuple(tuple3);
    query3.addResultCl(r3);

    std::list<std::string> resWhile = evaluator.evaluate(query3);
    resWhile.sort();
    REQUIRE(resWhile == std::list<std::string>{"3"});

    qps::query::Query query4;
    qps::query::Declaration d4 = qps::query::Declaration { "ifs", qps::query::DesignEntity::IF };
    std::vector<qps::query::Elem> tuple4 { qps::query::Elem::ofDeclaration(d4) };
    qps::query::ResultCl r4 = qps::query::ResultCl::ofTuple(tuple4);
    query4.addResultCl(r4);

    query4.addDeclaration("ifs", qps::query::DesignEntity::IF);
    std::list<std::string> resIf = evaluator.evaluate(query4);
    resIf.sort();
    REQUIRE(resIf == std::list<std::string>{"4", "6"});

    qps::query::Query query5;
    qps::query::Declaration d5 = qps::query::Declaration { "pr", qps::query::DesignEntity::PRINT };
    std::vector<qps::query::Elem> tuple5 { qps::query::Elem::ofDeclaration(d5) };
    qps::query::ResultCl r5 = qps::query::ResultCl::ofTuple(tuple5);
    query5.addResultCl(r5);

    query5.addDeclaration("pr", qps::query::DesignEntity::PRINT);
    std::list<std::string> resPrint = evaluator.evaluate(query5);
    resPrint.sort();
    REQUIRE(resPrint == std::list<std::string>{"7"});

    qps::query::Query query6;
    qps::query::Declaration d6 = qps::query::Declaration { "r", qps::query::DesignEntity::READ };
    std::vector<qps::query::Elem> tuple6 { qps::query::Elem::ofDeclaration(d6) };
    qps::query::ResultCl r6 = qps::query::ResultCl::ofTuple(tuple6);
    query6.addResultCl(r6);

    query6.addDeclaration("r", qps::query::DesignEntity::READ);
    std::list<std::string> resRead = evaluator.evaluate(query6);
    resRead.sort();
    REQUIRE(resRead.empty());
}


TEST_CASE("Test get variables") {
    PKB pkb = PKB();
    PKB* ptr = &pkb;
    qps::evaluator::Evaluator evaluator = qps::evaluator::Evaluator(ptr);

    qps::query::Query query1;
    qps::query::Declaration d1 = qps::query::Declaration { "v", qps::query::DesignEntity::VARIABLE };
    std::vector<qps::query::Elem> tuple1 { qps::query::Elem::ofDeclaration(d1) };
    qps::query::ResultCl r1 = qps::query::ResultCl::ofTuple(tuple1);
    query1.addResultCl(r1);

    query1.addDeclaration("v", qps::query::DesignEntity::VARIABLE);
    REQUIRE(evaluator.evaluate(query1).empty());

    pkb.insertEntity(VAR_NAME{ "x" });
    pkb.insertEntity(VAR_NAME{ "y" });
    pkb.insertEntity(VAR_NAME{ "cur" });

    PKBField v1 = PKBField::createConcrete(VAR_NAME{ "x" });
    PKBField v2 = PKBField::createConcrete(VAR_NAME{ "y" });
    PKBField v3 = PKBField::createConcrete(VAR_NAME{ "cur" });

    qps::evaluator::Evaluator evaluator2 = qps::evaluator::Evaluator(ptr);
    std::list<std::string> resVar = evaluator2.evaluate(query1);
    resVar.sort();
    REQUIRE(resVar == std::list<std::string>{"cur", "x", "y"});
}

TEST_CASE("Test evaluate select s") {
    qps::query::Query q = qps::query::Query();
    q.addDeclaration("s", qps::query::DesignEntity::STMT);
    q.addDeclaration("a", qps::query::DesignEntity::STMT);
    q.addDeclaration("v", qps::query::DesignEntity::VARIABLE);
    q.addDeclaration("w", qps::query::DesignEntity::WHILE);

    qps::query::Declaration d1 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple1 { qps::query::Elem::ofDeclaration(d1) };
    qps::query::ResultCl r1 = qps::query::ResultCl::ofTuple(tuple1);
    q.addResultCl(r1);

    PKB pkb = PKB();
    PKB* ptr = &pkb;
    qps::evaluator::Evaluator evaluator = qps::evaluator::Evaluator(ptr);
    pkb.insertEntity(STMT_LO{ 2, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 5, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 8, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 3, StatementType::While });
    pkb.insertEntity(STMT_LO{ 4, StatementType::If });
    pkb.insertEntity(STMT_LO{ 6, StatementType::If });
    pkb.insertEntity(STMT_LO{ 7, StatementType::Print });

    std::list<std::string> result = evaluator.evaluate(q);
    result.sort();
    std::list<std::string> expected{"7", "6", "4", "3", "8", "5", "2"};
    expected.sort();
    REQUIRE(result == expected);
}

TEST_CASE("Test evaluate select c") {
    qps::query::Query q = qps::query::Query();
    q.addDeclaration("c", qps::query::DesignEntity::CONSTANT);

    qps::query::Declaration d1 = qps::query::Declaration { "c", qps::query::DesignEntity::CONSTANT };
    std::vector<qps::query::Elem> tuple1 { qps::query::Elem::ofDeclaration(d1) };
    qps::query::ResultCl r1 = qps::query::ResultCl::ofTuple(tuple1);
    q.addResultCl(r1);

    PKB pkb = PKB();
    PKB* ptr = &pkb;
    pkb.insertEntity(4);
    pkb.insertEntity(56);
    qps::evaluator::Evaluator evaluator = qps::evaluator::Evaluator(ptr);

    std::list<std::string> result = evaluator.evaluate(q);
    result.sort();
    std::list<std::string> expected{"4", "56"};
    expected.sort();
    REQUIRE(result == expected);
}

TEST_CASE("Test evaluate select a") {
    qps::query::Query q = qps::query::Query();
    q.addDeclaration("a", qps::query::DesignEntity::ASSIGN);

    qps::query::Declaration d1 = qps::query::Declaration { "a", qps::query::DesignEntity::ASSIGN };
    std::vector<qps::query::Elem> tuple1 { qps::query::Elem::ofDeclaration(d1) };
    qps::query::ResultCl r1 = qps::query::ResultCl::ofTuple(tuple1);
    q.addResultCl(r1);

    PKB pkb = PKB();
    PKB* ptr = &pkb;
    qps::evaluator::Evaluator evaluator = qps::evaluator::Evaluator(ptr);
    pkb.insertEntity(STMT_LO{ 2, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 5, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 8, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 3, StatementType::While });
    pkb.insertEntity(STMT_LO{ 4, StatementType::If });
    pkb.insertEntity(STMT_LO{ 6, StatementType::If });
    pkb.insertEntity(STMT_LO{ 7, StatementType::Print });

    std::list<std::string> result = evaluator.evaluate(q);
    result.sort();
    std::list<std::string> expected{"8", "5", "2"};
    expected.sort();
    REQUIRE(result == expected);
}

TEST_CASE("Test evaluate select v") {
    qps::query::Query q = qps::query::Query();
    q.addDeclaration("v", qps::query::DesignEntity::VARIABLE);

    qps::query::Declaration d1 = qps::query::Declaration { "v", qps::query::DesignEntity::VARIABLE };
    std::vector<qps::query::Elem> tuple1 { qps::query::Elem::ofDeclaration(d1) };
    qps::query::ResultCl r1 = qps::query::ResultCl::ofTuple(tuple1);
    q.addResultCl(r1);

    PKB pkb = PKB();
    PKB* ptr = &pkb;
    qps::evaluator::Evaluator evaluator = qps::evaluator::Evaluator(ptr);

    pkb.insertEntity(VAR_NAME{ "x" });
    pkb.insertEntity(VAR_NAME{ "y" });
    pkb.insertEntity(VAR_NAME{ "cur" });

    std::list<std::string> result = evaluator.evaluate(q);
    result.sort();
    std::list<std::string> expected{"cur", "y", "x"};
    expected.sort();
    REQUIRE(result == expected);
}
