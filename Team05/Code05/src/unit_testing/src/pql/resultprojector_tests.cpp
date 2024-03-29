#include "QPS/Evaluator.h"
#include "QPS/Query.h"
#include "catch.hpp"
#include "logging.h"

#define TEST_LOG Logger() << "resultprojector_tests.cpp"

using qps::query::Declaration;
TEST_CASE("Get list of result") {
PKB pkb = PKB();
PKB* ptr = &pkb;
pkb.insertEntity(STMT_LO{ 2, StatementType::Assignment });
pkb.insertEntity(STMT_LO{ 5, StatementType::Assignment });
pkb.insertEntity(STMT_LO{ 1, StatementType::Assignment });
pkb.insertEntity(STMT_LO{ 3, StatementType::While });
pkb.insertEntity(STMT_LO{ 4, StatementType::If });
pkb.insertEntity(STMT_LO{ 6, StatementType::If });
pkb.insertEntity(STMT_LO{ 7, StatementType::Print, "x"});
pkb.insertEntity(STMT_LO{ 8, StatementType::Print, "y"});
pkb.insertEntity(STMT_LO{ 9, StatementType::Read, "s"});
pkb.insertEntity(VAR_NAME{"x"});
pkb.insertEntity(VAR_NAME{"y"});
pkb.insertEntity(VAR_NAME{"s"});

TEST_LOG << "select if";
qps::evaluator::Evaluator evaluator(ptr);
qps::query::Query query{};
query.addDeclaration("if", qps::query::DesignEntity::IF);

qps::query::Declaration d = qps::query::Declaration { "if", qps::query::DesignEntity::IF };
std::vector<qps::query::Elem> tuple { qps::query::Elem::ofDeclaration(d) };
qps::query::ResultCl r = qps::query::ResultCl::ofTuple(tuple);
query.addResultCl(r);

REQUIRE(query.getSuchthat().empty());
std::list<std::string> result = evaluator.evaluate(query);

REQUIRE(2 == result.size());
for (auto r : result) {
TEST_LOG << r;
}

TEST_LOG << "select <x, pr, if>";
qps::evaluator::Evaluator evaluator2(ptr);
qps::query::Query query2{};
query2.addDeclaration("if", qps::query::DesignEntity::IF);
query2.addDeclaration("pr", qps::query::DesignEntity::PRINT);
query2.addDeclaration("w", qps::query::DesignEntity::WHILE);

qps::query::Declaration d21 = qps::query::Declaration { "if", qps::query::DesignEntity::IF };
qps::query::Declaration d22 = qps::query::Declaration { "pr", qps::query::DesignEntity::PRINT };
qps::query::Declaration d23 = qps::query::Declaration { "w", qps::query::DesignEntity::WHILE };
std::vector<qps::query::Elem> tuple2 { qps::query::Elem::ofDeclaration(d23),
                                       qps::query::Elem::ofDeclaration(d22),
                                       qps::query::Elem::ofDeclaration(d21) };
qps::query::ResultCl r2 = qps::query::ResultCl::ofTuple(tuple2);
query2.addResultCl(r2);
std::list<std::string> result2 = evaluator2.evaluate(query2);
result2.sort();
REQUIRE(result2 == std::list<std::string>{"3 7 4", "3 7 6", "3 8 4", "3 8 6"});
for (auto r : result2) {
TEST_LOG << r;
}

TEST_LOG << "select <p.varName, v.varName, s.stmt#> with v.varName = x";
qps::evaluator::Evaluator evaluator3(ptr);
qps::query::Query query3{};
query3.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
query3.addDeclaration("v", qps::query::DesignEntity::VARIABLE);
query3.addDeclaration("p", qps::query::DesignEntity::PRINT);

qps::query::AttrRef d31 = qps::query::AttrRef{qps::query::AttrName::STMTNUM,
                                              Declaration{ "a", qps::query::DesignEntity::ASSIGN}};
qps::query::AttrRef d32 = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                              Declaration{"v", qps::query::DesignEntity::VARIABLE}};
qps::query::AttrRef d33 = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                              Declaration{"p", qps::query::DesignEntity::PRINT}};
std::vector<qps::query::Elem> tuple3 { qps::query::Elem::ofAttrRef(d33),
                                       qps::query::Elem::ofAttrRef(d32),
                                       qps::query::Elem::ofAttrRef(d31) };
qps::query::ResultCl r3 = qps::query::ResultCl::ofTuple(tuple3);
query3.addResultCl(r3);
qps::query::AttrRef attr3 = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                                Declaration { "v", qps::query::DesignEntity::VARIABLE}};
qps::query::AttrCompareRef lhs3 = qps::query::AttrCompareRef::ofAttrRef(attr3);
qps::query::AttrCompareRef rhs3 = qps::query::AttrCompareRef::ofString("x");
qps::query::AttrCompare with3 = qps::query::AttrCompare{lhs3, rhs3};
query3.addWith(with3);
std::list<std::string> result3 = evaluator3.evaluate(query3);
result3.sort();
REQUIRE(result3 == std::list<std::string>{"x x 1", "x x 2", "x x 5", "y x 1", "y x 2", "y x 5"});
for (auto r : result3) {
TEST_LOG << r;
}

TEST_LOG << "select BOOLEAN";
qps::evaluator::Evaluator evaluator4(ptr);
qps::query::Query query4{};
qps::query::ResultCl r4 = qps::query::ResultCl::ofBoolean();
query4.addResultCl(r4);
std::list<std::string> result4 = evaluator4.evaluate(query4);
REQUIRE(result4 == std::list<std::string>{"TRUE"});

TEST_LOG << "select BOOLEAN with v.varName = sum";
qps::evaluator::Evaluator evaluator5(ptr);
qps::query::Query query5{};
query5.addDeclaration("v", qps::query::DesignEntity::VARIABLE);
qps::query::ResultCl r5 = qps::query::ResultCl::ofBoolean();
query5.addResultCl(r5);
qps::query::AttrRef attr5 = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                                Declaration { "v", qps::query::DesignEntity::VARIABLE }};
qps::query::AttrCompareRef lhs5 = qps::query::AttrCompareRef::ofAttrRef(attr5);
qps::query::AttrCompareRef rhs5 = qps::query::AttrCompareRef::ofString("sum");
qps::query::AttrCompare with5 = qps::query::AttrCompare{lhs5, rhs5};
query5.addWith(with5);
std::list<std::string> result5 = evaluator5.evaluate(query5);
REQUIRE(result5 == std::list<std::string>{"FALSE"});

TEST_LOG << "select <a, a>";
qps::evaluator::Evaluator evaluator6(ptr);
qps::query::Query query6{};
query6.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
qps::query::Declaration d61 = qps::query::Declaration { "a", qps::query::DesignEntity::ASSIGN };
qps::query::Declaration d62 = qps::query::Declaration { "a", qps::query::DesignEntity::ASSIGN };
std::vector<qps::query::Elem> tuple6 { qps::query::Elem::ofDeclaration(d61),
                                       qps::query::Elem::ofDeclaration(d62) };
qps::query::ResultCl r6 = qps::query::ResultCl::ofTuple(tuple6);
query6.addResultCl(r6);
std::list<std::string> result6 = evaluator6.evaluate(query6);
result6.sort();
REQUIRE(result6 == std::list<std::string>{"1 1", "2 2", "5 5"});
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
REQUIRE("main" == qps::evaluator::ResultProjector::PKBFieldToString(field1));
REQUIRE("a" == qps::evaluator::ResultProjector::PKBFieldToString(field2));
REQUIRE("5" == qps::evaluator::ResultProjector::PKBFieldToString(field3));
REQUIRE("1" == qps::evaluator::ResultProjector::PKBFieldToString(field4));
REQUIRE("87" == qps::evaluator::ResultProjector::PKBFieldToString(field5));
REQUIRE("maven" == qps::evaluator::ResultProjector::PKBFieldToString(field6));
}
