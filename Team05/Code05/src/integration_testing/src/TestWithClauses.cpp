#include "pql/evaluator.h"
#include "pql/query.h"
#include "catch.hpp"
#include "logging.h"

#define TEST_LOG Logger() << "TestWithClauses.cpp "

PKB createPKBForWith() {
    PKB pkb = PKB();
    pkb.insertProcedure("proc1");
    pkb.insertProcedure("proc2");
    pkb.insertProcedure("proc3");
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
    return pkb;
}

TEST_CASE("test concrete with clause") {
    PKB pkb = createPKBForWith();

    TEST_LOG << "select s with 5 = 5";
    qps::query::Query query;
    qps::query::AttrCompareRef lhs = qps::query::AttrCompareRef::ofNumber(5);
    qps::query::AttrCompareRef rhs = qps::query::AttrCompareRef::ofNumber(5);
    qps::query::AttrCompare with = qps::query::AttrCompare{lhs, rhs};
    query.addWith(with);
    query.addDeclaration("s", qps::query::DesignEntity::STMT);
    query.addVariable("s");
    qps::evaluator::Evaluator evaluator = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result = evaluator.evaluate(query);
    result.sort();
    REQUIRE(result == std::list<std::string>{"1", "2", "3", "4", "5", "6"});

    TEST_LOG << "select s with 5 = 1";
    qps::query::Query query2;
    qps::query::AttrCompareRef lhs2 = qps::query::AttrCompareRef::ofNumber(5);
    qps::query::AttrCompareRef rhs2 = qps::query::AttrCompareRef::ofNumber(1);
    qps::query::AttrCompare with2 = qps::query::AttrCompare{lhs2, rhs2};
    query2.addWith(with2);
    query2.addDeclaration("s", qps::query::DesignEntity::STMT);
    query2.addVariable("s");
    qps::evaluator::Evaluator evaluator2 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result2 = evaluator2.evaluate(query2);
    result2.sort();
    REQUIRE(result2.empty());

    TEST_LOG << "select s with x = x";
    qps::query::Query query3;
    qps::query::AttrCompareRef lhs3 = qps::query::AttrCompareRef::ofString("x");
    qps::query::AttrCompareRef rhs3 = qps::query::AttrCompareRef::ofString("x");
    qps::query::AttrCompare with3 = qps::query::AttrCompare{lhs3, rhs3};
    query3.addWith(with3);
    query3.addDeclaration("s", qps::query::DesignEntity::STMT);
    query3.addVariable("s");
    qps::evaluator::Evaluator evaluator3 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result3 = evaluator3.evaluate(query3);
    result3.sort();
    REQUIRE(result3 == std::list<std::string>{"1", "2", "3", "4", "5", "6"});

    TEST_LOG << "select s with x = world";
    qps::query::Query query4;
    qps::query::AttrCompareRef lhs4 = qps::query::AttrCompareRef::ofString("x");
    qps::query::AttrCompareRef rhs4 = qps::query::AttrCompareRef::ofString("world");
    qps::query::AttrCompare with4 = qps::query::AttrCompare{lhs4, rhs4};
    query4.addWith(with4);
    query4.addWith(with3);
    query4.addDeclaration("s", qps::query::DesignEntity::STMT);
    query4.addVariable("s");
    qps::evaluator::Evaluator evaluator4 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result4 = evaluator4.evaluate(query4);
    result4.sort();
    REQUIRE(result4.empty());
}