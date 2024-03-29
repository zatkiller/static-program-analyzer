#include "QPS/Evaluator.h"
#include "QPS/Query.h"
#include "catch.hpp"
#include "logging.h"

#define TEST_LOG Logger() << "TestWithClauses.cpp "

using qps::query::Declaration;

PKB createPKBForWith() {
    PKB pkb = PKB();
    pkb.insertEntity(PROC_NAME{ "proc1" });
    pkb.insertEntity(PROC_NAME{ "proc2" });
    pkb.insertEntity(PROC_NAME{ "proc3" });
    pkb.insertEntity(STMT_LO{ 5, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 2, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 4, StatementType::Assignment });
    pkb.insertEntity(STMT_LO{ 1, StatementType::If });
    pkb.insertEntity(STMT_LO{ 3, StatementType::While });
    pkb.insertEntity(STMT_LO{ 6, StatementType::Print, "x"});
    pkb.insertEntity(STMT_LO{ 7, StatementType::Call, "proc2"});
    pkb.insertEntity(STMT_LO{ 8, StatementType::Call, "proc2"});
    pkb.insertEntity(STMT_LO{ 9, StatementType::Print, "y"});
    pkb.insertEntity(STMT_LO{ 10, StatementType::Read, "variable"});

    pkb.insertEntity(VAR_NAME{ "variable" });
    pkb.insertEntity(VAR_NAME{ "current" });
    pkb.insertEntity(VAR_NAME{ "x" });
    pkb.insertEntity(VAR_NAME{ "y" });
    pkb.insertEntity(CONST{3});
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

    qps::query::Declaration d1 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple1 { qps::query::Elem::ofDeclaration(d1) };
    qps::query::ResultCl r1 = qps::query::ResultCl::ofTuple(tuple1);
    query.addResultCl(r1);

    qps::evaluator::Evaluator evaluator = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result = evaluator.evaluate(query);
    result.sort();
    REQUIRE(result == std::list<std::string>{"1", "10", "2", "3", "4", "5", "6", "7", "8", "9"});

    TEST_LOG << "select s with 5 = 1";
    qps::query::Query query2;
    qps::query::AttrCompareRef lhs2 = qps::query::AttrCompareRef::ofNumber(5);
    qps::query::AttrCompareRef rhs2 = qps::query::AttrCompareRef::ofNumber(1);
    qps::query::AttrCompare with2 = qps::query::AttrCompare{lhs2, rhs2};
    query2.addWith(with2);
    query2.addDeclaration("s", qps::query::DesignEntity::STMT);

    qps::query::Declaration d2 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple2 { qps::query::Elem::ofDeclaration(d2) };
    qps::query::ResultCl r2 = qps::query::ResultCl::ofTuple(tuple2);
    query2.addResultCl(r2);

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

    qps::query::Declaration d3 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple3 { qps::query::Elem::ofDeclaration(d3) };
    qps::query::ResultCl r3 = qps::query::ResultCl::ofTuple(tuple3);
    query3.addResultCl(r3);

    qps::evaluator::Evaluator evaluator3 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result3 = evaluator3.evaluate(query3);
    result3.sort();
    REQUIRE(result3 == std::list<std::string>{"1", "10", "2", "3", "4", "5", "6", "7", "8", "9"});

    TEST_LOG << "select s with x = world";
    qps::query::Query query4;
    qps::query::AttrCompareRef lhs4 = qps::query::AttrCompareRef::ofString("x");
    qps::query::AttrCompareRef rhs4 = qps::query::AttrCompareRef::ofString("world");
    qps::query::AttrCompare with4 = qps::query::AttrCompare{lhs4, rhs4};
    query4.addWith(with4);
    query4.addWith(with3);
    query4.addDeclaration("s", qps::query::DesignEntity::STMT);

    qps::query::Declaration d4 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple4 { qps::query::Elem::ofDeclaration(d4) };
    qps::query::ResultCl r4 = qps::query::ResultCl::ofTuple(tuple4);
    query4.addResultCl(r4);

    qps::evaluator::Evaluator evaluator4 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result4 = evaluator4.evaluate(query4);
    result4.sort();
    REQUIRE(result4.empty());
}

TEST_CASE("Test with clause with one attrRef") {
    PKB pkb = createPKBForWith();

    TEST_LOG << "select c with c.procName = 'proc2'";
    qps::query::Query query;
    qps::query::AttrRef attr = qps::query::AttrRef{qps::query::AttrName::PROCNAME,
                                                   Declaration { "c", qps::query::DesignEntity::CALL }};
    qps::query::AttrCompareRef lhs = qps::query::AttrCompareRef::ofAttrRef(attr);
    qps::query::AttrCompareRef rhs = qps::query::AttrCompareRef::ofString("proc2");
    qps::query::AttrCompare with = qps::query::AttrCompare{lhs, rhs};
    query.addWith(with);
    query.addDeclaration("c", qps::query::DesignEntity::CALL);

    qps::query::Declaration d1 = qps::query::Declaration { "c", qps::query::DesignEntity::CALL };
    std::vector<qps::query::Elem> tuple1 { qps::query::Elem::ofDeclaration(d1) };
    qps::query::ResultCl r1 = qps::query::ResultCl::ofTuple(tuple1);
    query.addResultCl(r1);

    qps::evaluator::Evaluator evaluator = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result = evaluator.evaluate(query);
    result.sort();
    REQUIRE(result == std::list<std::string>{"7", "8"});

    TEST_LOG << "select pr with pr.varName = 'y'";
    qps::query::Query query2;
    qps::query::AttrRef attr2 = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                                    Declaration { "pr", qps::query::DesignEntity::PRINT }};
    qps::query::AttrCompareRef lhs2 = qps::query::AttrCompareRef::ofAttrRef(attr2);
    qps::query::AttrCompareRef rhs2 = qps::query::AttrCompareRef::ofString("y");
    qps::query::AttrCompare with2 = qps::query::AttrCompare{lhs2, rhs2};
    query2.addWith(with2);
    query2.addDeclaration("pr", qps::query::DesignEntity::PRINT);

    qps::query::Declaration d2 = qps::query::Declaration { "pr", qps::query::DesignEntity::PRINT };
    std::vector<qps::query::Elem> tuple2 { qps::query::Elem::ofDeclaration(d2) };
    qps::query::ResultCl r2 = qps::query::ResultCl::ofTuple(tuple2);
    query2.addResultCl(r2);

    qps::evaluator::Evaluator evaluator2 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result2 = evaluator2.evaluate(query2);
    result2.sort();
    REQUIRE(result2 == std::list<std::string>{"9"});

    TEST_LOG << "select r with r.varName = 'variable'";
    qps::query::Query query3;
    qps::query::AttrRef attr3 = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                                    Declaration { "r", qps::query::DesignEntity::READ }};
    qps::query::AttrCompareRef lhs3 = qps::query::AttrCompareRef::ofAttrRef(attr3);
    qps::query::AttrCompareRef rhs3 = qps::query::AttrCompareRef::ofString("variable");
    qps::query::AttrCompare with3 = qps::query::AttrCompare{lhs3, rhs3};
    query3.addWith(with3);
    query3.addDeclaration("r", qps::query::DesignEntity::READ);

    qps::query::Declaration d3 = qps::query::Declaration { "r", qps::query::DesignEntity::READ };
    std::vector<qps::query::Elem> tuple3 { qps::query::Elem::ofDeclaration(d3) };
    qps::query::ResultCl r3 = qps::query::ResultCl::ofTuple(tuple3);
    query3.addResultCl(r3);

    qps::evaluator::Evaluator evaluator3 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result3 = evaluator3.evaluate(query3);
    result3.sort();
    REQUIRE(result3 == std::list<std::string>{"10"});

    TEST_LOG << "select p with p.procName = 'proc2'";
    qps::query::Query query4;
    qps::query::AttrRef attr4 = qps::query::AttrRef{qps::query::AttrName::PROCNAME,
                                                    Declaration { "p", qps::query::DesignEntity::PROCEDURE }};
    qps::query::AttrCompareRef lhs4 = qps::query::AttrCompareRef::ofAttrRef(attr4);
    qps::query::AttrCompareRef rhs4 = qps::query::AttrCompareRef::ofString("proc2");
    qps::query::AttrCompare with4 = qps::query::AttrCompare{lhs4, rhs4};
    query4.addWith(with4);
    query4.addDeclaration("p", qps::query::DesignEntity::PROCEDURE);

    qps::query::Declaration d4 = qps::query::Declaration { "p", qps::query::DesignEntity::PROCEDURE };
    std::vector<qps::query::Elem> tuple4 { qps::query::Elem::ofDeclaration(d4) };
    qps::query::ResultCl r4 = qps::query::ResultCl::ofTuple(tuple4);
    query4.addResultCl(r4);

    qps::evaluator::Evaluator evaluator4 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result4 = evaluator4.evaluate(query4);
    result4.sort();
    REQUIRE(result4 == std::list<std::string>{"proc2"});

    TEST_LOG << "select a with a.Stmt# = 4";
    qps::query::Query query5;
    qps::query::AttrRef attr5 = qps::query::AttrRef{qps::query::AttrName::STMTNUM,
                                                    Declaration { "a", qps::query::DesignEntity::ASSIGN }};
    qps::query::AttrCompareRef lhs5 = qps::query::AttrCompareRef::ofAttrRef(attr5);
    qps::query::AttrCompareRef rhs5 = qps::query::AttrCompareRef::ofNumber(4);
    qps::query::AttrCompare with5 = qps::query::AttrCompare{lhs5, rhs5};
    query5.addWith(with5);
    query5.addDeclaration("a", qps::query::DesignEntity::ASSIGN);

    qps::query::Declaration d5 = qps::query::Declaration { "a", qps::query::DesignEntity::ASSIGN };
    std::vector<qps::query::Elem> tuple5 { qps::query::Elem::ofDeclaration(d5) };
    qps::query::ResultCl r5 = qps::query::ResultCl::ofTuple(tuple5);
    query5.addResultCl(r5);

    qps::evaluator::Evaluator evaluator5 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result5 = evaluator5.evaluate(query5);
    result5.sort();
    REQUIRE(result5 == std::list<std::string>{"4"});

    TEST_LOG << "select w with w.Stmt# = 4";
    qps::query::Query query6;
    qps::query::AttrRef attr6 = qps::query::AttrRef{qps::query::AttrName::STMTNUM,
                                                    Declaration { "w", qps::query::DesignEntity::WHILE }};
    qps::query::AttrCompareRef lhs6 = qps::query::AttrCompareRef::ofAttrRef(attr6);
    qps::query::AttrCompareRef rhs6 = qps::query::AttrCompareRef::ofNumber(4);
    qps::query::AttrCompare with6 = qps::query::AttrCompare{rhs6, lhs6};
    query6.addWith(with6);
    query6.addDeclaration("w", qps::query::DesignEntity::WHILE);

    qps::query::Declaration d6 = qps::query::Declaration { "w", qps::query::DesignEntity::WHILE };
    std::vector<qps::query::Elem> tuple6 { qps::query::Elem::ofDeclaration(d6) };
    qps::query::ResultCl r6 = qps::query::ResultCl::ofTuple(tuple6);
    query6.addResultCl(r6);

    qps::evaluator::Evaluator evaluator6 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result6 = evaluator6.evaluate(query6);
    result6.sort();
    REQUIRE(result6.empty());
}

TEST_CASE("test with clauses with two attrRefs") {
    PKB pkb = createPKBForWith();

    TEST_LOG << "select c with c.procName = p.procName";
    qps::query::Query query;
    qps::query::AttrRef attrl = qps::query::AttrRef{qps::query::AttrName::PROCNAME,
                                                    Declaration { "c", qps::query::DesignEntity::CALL }};
    qps::query::AttrRef attrr = qps::query::AttrRef{ qps::query::AttrName::PROCNAME,
                                                     Declaration { "p", qps::query::DesignEntity::PROCEDURE }};
    qps::query::AttrCompareRef lhs = qps::query::AttrCompareRef::ofAttrRef(attrl);
    qps::query::AttrCompareRef rhs = qps::query::AttrCompareRef::ofAttrRef(attrr);
    qps::query::AttrCompare with = qps::query::AttrCompare{lhs, rhs};
    query.addWith(with);
    query.addDeclaration("c", qps::query::DesignEntity::CALL);
    query.addDeclaration("p", qps::query::DesignEntity::PROCEDURE);

    qps::query::Declaration d = qps::query::Declaration { "c", qps::query::DesignEntity::CALL };
    std::vector<qps::query::Elem> tuple { qps::query::Elem::ofDeclaration(d) };
    qps::query::ResultCl r = qps::query::ResultCl::ofTuple(tuple);
    query.addResultCl(r);

    qps::evaluator::Evaluator evaluator = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result = evaluator.evaluate(query);
    result.sort();
    REQUIRE(result == std::list<std::string>{"7", "8"});

    TEST_LOG << "select s with c.procName = c.procName";
    qps::query::Query query1;
    qps::query::AttrRef attrl1 = qps::query::AttrRef{qps::query::AttrName::PROCNAME,
                                                     Declaration { "c", qps::query::DesignEntity::CALL }};
    qps::query::AttrRef attrr1 = qps::query::AttrRef{qps::query::AttrName::PROCNAME,
                                                     Declaration { "c", qps::query::DesignEntity::CALL }};
    qps::query::AttrCompareRef lhs1 = qps::query::AttrCompareRef::ofAttrRef(attrl1);
    qps::query::AttrCompareRef rhs1 = qps::query::AttrCompareRef::ofAttrRef(attrr1);
    qps::query::AttrCompare with1 = qps::query::AttrCompare{lhs1, rhs1};
    query1.addWith(with1);
    query1.addDeclaration("c", qps::query::DesignEntity::CALL);
    query1.addDeclaration("s", qps::query::DesignEntity::STMT);

    qps::query::Declaration d1 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple1 { qps::query::Elem::ofDeclaration(d1) };
    qps::query::ResultCl r1 = qps::query::ResultCl::ofTuple(tuple1);
    query1.addResultCl(r1);

    qps::evaluator::Evaluator evaluator1 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result1 = evaluator1.evaluate(query1);
    result1.sort();
    REQUIRE(result1 == std::list<std::string>{"1", "10", "2", "3", "4", "5", "6", "7", "8", "9"});

    TEST_LOG << "select v with pr.varName = v.varName";
    qps::query::Query query2;
    qps::query::AttrRef attrl2 = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                                     Declaration { "pr", qps::query::DesignEntity::PRINT }};
    qps::query::AttrRef attrr2 = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                                     Declaration { "v", qps::query::DesignEntity::VARIABLE }};
    qps::query::AttrCompareRef lhs2 = qps::query::AttrCompareRef::ofAttrRef(attrl2);
    qps::query::AttrCompareRef rhs2 = qps::query::AttrCompareRef::ofAttrRef(attrr2);
    qps::query::AttrCompare with2 = qps::query::AttrCompare{lhs2, rhs2};
    query2.addWith(with2);
    query2.addDeclaration("pr", qps::query::DesignEntity::PRINT);
    query2.addDeclaration("v", qps::query::DesignEntity::VARIABLE);

    qps::query::Declaration d2 = qps::query::Declaration { "v", qps::query::DesignEntity::VARIABLE };
    std::vector<qps::query::Elem> tuple2 { qps::query::Elem::ofDeclaration(d2) };
    qps::query::ResultCl r2 = qps::query::ResultCl::ofTuple(tuple2);
    query2.addResultCl(r2);

    qps::evaluator::Evaluator evaluator2 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result2 = evaluator2.evaluate(query2);
    result2.sort();
    REQUIRE(result2 == std::list<std::string>{"x", "y"});

    TEST_LOG << "select r with r.varName = pr.varName";
    qps::query::Query query3;
    qps::query::AttrRef attrl3 = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                                     Declaration { "r", qps::query::DesignEntity::READ }};
    qps::query::AttrRef attrr3 = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                                     Declaration { "pr", qps::query::DesignEntity::PRINT }};
    qps::query::AttrCompareRef lhs3 = qps::query::AttrCompareRef::ofAttrRef(attrl3);
    qps::query::AttrCompareRef rhs3 = qps::query::AttrCompareRef::ofAttrRef(attrr3);
    qps::query::AttrCompare with3 = qps::query::AttrCompare{lhs3, rhs3};
    query3.addWith(with3);
    query3.addDeclaration("r", qps::query::DesignEntity::READ);
    query3.addDeclaration("pr", qps::query::DesignEntity::PRINT);

    qps::query::Declaration d3 = qps::query::Declaration { "r", qps::query::DesignEntity::READ };
    std::vector<qps::query::Elem> tuple3 { qps::query::Elem::ofDeclaration(d3) };
    qps::query::ResultCl r3 = qps::query::ResultCl::ofTuple(tuple3);
    query3.addResultCl(r3);

    qps::evaluator::Evaluator evaluator3 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result3 = evaluator3.evaluate(query3);
    result3.sort();
    REQUIRE(result3.empty());

    TEST_LOG << "select c with a.stmt# = c.value";
    qps::query::Query query4;
    qps::query::AttrRef attrl4 = qps::query::AttrRef{qps::query::AttrName::STMTNUM,
                                                     Declaration { "a", qps::query::DesignEntity::ASSIGN }};
    qps::query::AttrRef attrr4 = qps::query::AttrRef{qps::query::AttrName::VALUE,
                                                     Declaration { "c", qps::query::DesignEntity::CONSTANT }};
    qps::query::AttrCompareRef lhs4 = qps::query::AttrCompareRef::ofAttrRef(attrl4);
    qps::query::AttrCompareRef rhs4 = qps::query::AttrCompareRef::ofAttrRef(attrr4);
    qps::query::AttrCompare with4 = qps::query::AttrCompare{lhs4, rhs4};
    query4.addWith(with4);
    query4.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query4.addDeclaration("c", qps::query::DesignEntity::CONSTANT);

    qps::query::Declaration d4 = qps::query::Declaration { "c", qps::query::DesignEntity::CONSTANT };
    std::vector<qps::query::Elem> tuple4 { qps::query::Elem::ofDeclaration(d4) };
    qps::query::ResultCl r4 = qps::query::ResultCl::ofTuple(tuple4);
    query4.addResultCl(r4);

    qps::evaluator::Evaluator evaluator4 = qps::evaluator::Evaluator{&pkb};
    std::list<std::string> result4 = evaluator4.evaluate(query4);
    result4.sort();
    REQUIRE(result4.empty());
}
