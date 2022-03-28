#include "pql/evaluator.h"
#include "SourceProcessor.h"
#include "PKB.h"
#include "catch.hpp"
#include "logging.h"

#define TEST_LOG Logger() << "TestPattern.cpp"

using qps::query::Declaration;

struct TestCode {
    std::string sourceCode = R"(
        procedure sumDigits {
            read number;
            sum = 0;

            while (number > 0) {
                digit = number % 10;
                sum = sum + digit;
                number = number / 10;
            }

            print sum;
        }
    )";

    std::string sourceCode2 = R"(
        procedure a {
            read x;
            x = x + 1;
            y = x * 2 - 3 % 1;
            print y;
        }
    )";
};

void printEvaluatorResult(std::list<std::string> result) {
    std::string s = " ";
    for (auto r : result) {
        s = s + r + " ";
    }
    TEST_LOG << s;
}

void printTable1(qps::evaluator::ResultTable table) {
    for (auto r : table.getTable()) {
        std::string record;
        for (auto f : r) {
            record = record + qps::evaluator::Evaluator::PKBFieldToString(f) + " ";
        }
        TEST_LOG << record;
    }
}

TEST_CASE("test simple source code") {
    PKB pkb;
    SourceProcessor sp;
    TestCode testcase{};
    sp.processSimple(testcase.sourceCode2, &pkb);

    TEST_LOG << "assign a; stmt s; Select s such that Parent(s, 10) pattern a('x', _)";
    qps::evaluator::Evaluator evaluator1 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query1{};
    query1.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query1.addDeclaration("s", qps::query::DesignEntity::STMT);

    qps::query::Declaration d1 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple1 { qps::query::Elem::ofDeclaration(d1) };
    qps::query::ResultCl r1 = qps::query::ResultCl::ofTuple(tuple1);
    query1.addResultCl(r1);

    std::shared_ptr<qps::query::Parent> ptr1 = std::make_shared<qps::query::Parent>();
    ptr1->parent = qps::query::StmtRef::ofDeclaration( Declaration { "s", qps::query::DesignEntity::STMT });
    ptr1->child = qps::query::StmtRef::ofLineNo(10);
    query1.addSuchthat(ptr1);
    query1.addPattern(qps::query::Pattern::ofAssignPattern("a",
                                          qps::query::EntRef::ofVarName("x"),
                                          qps::query::ExpSpec::ofWildcard()));

    std::list<std::string> result1 = evaluator1.evaluate(query1);
    result1.sort();
    printEvaluatorResult(result1);
    REQUIRE(result1.empty());

    TEST_LOG << "assign a; stmt s; variable v; Select s such that Uses(s,v) pattern a (v, _)";
    qps::evaluator::Evaluator evaluator2 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query2{};
    query2.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query2.addDeclaration("s", qps::query::DesignEntity::STMT);
    query2.addDeclaration("v", qps::query::DesignEntity::VARIABLE);

    qps::query::Declaration d2 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple2 { qps::query::Elem::ofDeclaration(d2) };
    qps::query::ResultCl r2 = qps::query::ResultCl::ofTuple(tuple2);
    query2.addResultCl(r2);

    std::shared_ptr<qps::query::UsesS> ptr2 = std::make_shared<qps::query::UsesS>();
    ptr2->useStmt = qps::query::StmtRef::ofDeclaration( Declaration { "s", qps::query::DesignEntity::STMT });
    ptr2->used = qps::query::EntRef::ofDeclaration( Declaration { "v", qps::query::DesignEntity::VARIABLE });
    query2.addSuchthat(ptr2);
    query2.addPattern(qps::query::Pattern::ofAssignPattern("a",
                                          qps::query::EntRef::ofDeclaration( Declaration {
                                              "v", qps::query::DesignEntity::VARIABLE }),
                                              qps::query::ExpSpec::ofWildcard()));
    std::list<std::string> result2 = evaluator2.evaluate(query2);
    result2.sort();
    printEvaluatorResult(result2);
    REQUIRE(result2 == std::list<std::string>{"2", "3", "4"});

    TEST_LOG << "assign a; Select a such that pattern a (v, 'x + 1')";
    qps::evaluator::Evaluator evaluator3 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query3{};
    query3.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query3.addDeclaration("v", qps::query::DesignEntity::VARIABLE);

    qps::query::Declaration d3 = qps::query::Declaration { "a", qps::query::DesignEntity::ASSIGN };
    std::vector<qps::query::Elem> tuple3 { qps::query::Elem::ofDeclaration(d3) };
    qps::query::ResultCl r3 = qps::query::ResultCl::ofTuple(tuple3);
    query3.addResultCl(r3);

    query3.addPattern(qps::query::Pattern::ofAssignPattern(
            "a",
            qps::query::EntRef::ofDeclaration( Declaration { "v", qps::query::DesignEntity::VARIABLE }),
            qps::query::ExpSpec::ofFullMatch("x + 1")));
    std::list<std::string> result3 = evaluator3.evaluate(query3);
    result3.sort();
    printEvaluatorResult(result3);
    REQUIRE(result3 == std::list<std::string>{"2"});
}

TEST_CASE("test evaluate pattern") {
    PKB pkb;
    SourceProcessor sp;
    TestCode testcase{};
    sp.processSimple(testcase.sourceCode, &pkb);

    TEST_LOG << "assign a; stmt s; Select s pattern a('sum', _)";
    qps::evaluator::Evaluator evaluator1 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query1{};
    query1.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query1.addDeclaration("s", qps::query::DesignEntity::STMT);

    qps::query::Declaration d1 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple1 { qps::query::Elem::ofDeclaration(d1) };
    qps::query::ResultCl r1 = qps::query::ResultCl::ofTuple(tuple1);
    query1.addResultCl(r1);

    query1.addPattern(qps::query::Pattern::ofAssignPattern("a",
                                          qps::query::EntRef::ofVarName("sum"),
                                          qps::query::ExpSpec::ofWildcard()));
    std::list<std::string> result1 = evaluator1.evaluate(query1);
    result1.sort();
    printEvaluatorResult(result1);
    REQUIRE(result1 == std::list<std::string>{"1", "2", "3", "4", "5", "6", "7"});

    TEST_LOG << "assign a; stmt s; variable v; Select s pattern a(v, _)";
    qps::evaluator::Evaluator evaluator2 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query2{};
    query2.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query2.addDeclaration("s", qps::query::DesignEntity::STMT);
    query2.addDeclaration("v", qps::query::DesignEntity::VARIABLE);

    qps::query::Declaration d2 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple2 { qps::query::Elem::ofDeclaration(d2) };
    qps::query::ResultCl r2 = qps::query::ResultCl::ofTuple(tuple2);
    query2.addResultCl(r2);

    query2.addPattern(qps::query::Pattern::ofAssignPattern("a",
                                          qps::query::EntRef::ofDeclaration( Declaration {
                                              "v", qps::query::DesignEntity::VARIABLE }),
                                              qps::query::ExpSpec::ofWildcard()));
    std::list<std::string> result2 = evaluator2.evaluate(query2);
    result2.sort();
    printEvaluatorResult(result2);
    REQUIRE(result2 == std::list<std::string>{"1", "2", "3", "4", "5", "6", "7"});

    TEST_LOG << "assign a; Select a pattern a(_, _)";
    qps::evaluator::Evaluator evaluator3 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query3{};
    query3.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query3.addDeclaration("s", qps::query::DesignEntity::STMT);

    qps::query::Declaration d3 = qps::query::Declaration { "a", qps::query::DesignEntity::ASSIGN };
    std::vector<qps::query::Elem> tuple3 { qps::query::Elem::ofDeclaration(d3) };
    qps::query::ResultCl r3 = qps::query::ResultCl::ofTuple(tuple3);
    query3.addResultCl(r3);

    query3.addPattern(qps::query::Pattern::ofAssignPattern("a",
                                          qps::query::EntRef::ofWildcard(),
                                          qps::query::ExpSpec::ofWildcard()));
    std::list<std::string> result3 = evaluator3.evaluate(query3);
    result3.sort();
    printEvaluatorResult(result3);
    REQUIRE(result3 == std::list<std::string>{ "2", "4", "5", "6"});

    TEST_LOG << "assign a; Select a pattern a(_, _'number'_)";
    qps::evaluator::Evaluator evaluator4 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query4{};
    query4.addDeclaration("a", qps::query::DesignEntity::ASSIGN);

    qps::query::Declaration d4 = qps::query::Declaration { "a", qps::query::DesignEntity::ASSIGN };
    std::vector<qps::query::Elem> tuple4 { qps::query::Elem::ofDeclaration(d4) };
    qps::query::ResultCl r4 = qps::query::ResultCl::ofTuple(tuple4);
    query4.addResultCl(r4);

    query4.addPattern(qps::query::Pattern::ofAssignPattern("a",
                                          qps::query::EntRef::ofWildcard(),
                                          qps::query::ExpSpec::ofPartialMatch("number")));
    std::list<std::string> result4 = evaluator4.evaluate(query4);
    result4.sort();
    printEvaluatorResult(result4);
    REQUIRE(result4 == std::list<std::string>{ "4", "6"});

    TEST_LOG << "assign a; Select a pattern a('x', _'number'_)  -- no result";
    qps::evaluator::Evaluator evaluator5 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query5{};
    query5.addDeclaration("a", qps::query::DesignEntity::ASSIGN);

    qps::query::Declaration d5 = qps::query::Declaration { "a", qps::query::DesignEntity::ASSIGN };
    std::vector<qps::query::Elem> tuple5 { qps::query::Elem::ofDeclaration(d5) };
    qps::query::ResultCl r5 = qps::query::ResultCl::ofTuple(tuple5);
    query5.addResultCl(r5);

    query5.addPattern(qps::query::Pattern::ofAssignPattern("a",
                                          qps::query::EntRef::ofVarName("x"),
                                          qps::query::ExpSpec::ofPartialMatch("number")));
    std::list<std::string> result5 = evaluator5.evaluate(query5);
    result5.sort();
    printEvaluatorResult(result5);
    REQUIRE(result5.empty());

    TEST_LOG << "assign a; variable v; Select a pattern a('digit', _'number'_)  -- return all variable";
    qps::evaluator::Evaluator evaluator6 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query6{};
    query6.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query6.addDeclaration("v", qps::query::DesignEntity::VARIABLE);

    qps::query::Declaration d6 = qps::query::Declaration { "v", qps::query::DesignEntity::VARIABLE };
    std::vector<qps::query::Elem> tuple6 { qps::query::Elem::ofDeclaration(d6) };
    qps::query::ResultCl r6 = qps::query::ResultCl::ofTuple(tuple6);
    query6.addResultCl(r6);

    query6.addPattern(qps::query::Pattern::ofAssignPattern("a",
                                          qps::query::EntRef::ofVarName("digit"),
                                          qps::query::ExpSpec::ofPartialMatch("number")));
    std::list<std::string> result6 = evaluator6.evaluate(query6);
    result6.sort();
    printEvaluatorResult(result6);
    REQUIRE(result6 == std::list<std::string>{"digit", "number", "sum"});

    TEST_LOG << "assign a; stmt s; Select s such that Follows*(1, s) pattern a('sum', _) -- no shared synonyms";
    qps::evaluator::Evaluator evaluator7 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query7{};
    query7.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query7.addDeclaration("s", qps::query::DesignEntity::STMT);

    qps::query::Declaration d7 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple7 { qps::query::Elem::ofDeclaration(d7) };
    qps::query::ResultCl r7 = qps::query::ResultCl::ofTuple(tuple7);
    query7.addResultCl(r7);

    std::shared_ptr<qps::query::FollowsT> ptr7 = std::make_shared<qps::query::FollowsT>();
    ptr7->follower = qps::query::StmtRef::ofLineNo(1);
    ptr7->transitiveFollowed = qps::query::StmtRef::ofDeclaration( Declaration { "s", qps::query::DesignEntity::STMT });
    query7.addSuchthat(ptr7);
    query7.addPattern(qps::query::Pattern::ofAssignPattern("a",
                                          qps::query::EntRef::ofVarName("sum"),
                                          qps::query::ExpSpec::ofWildcard()));
    std::list<std::string> result7 = evaluator7.evaluate(query7);
    result7.sort();
    printEvaluatorResult(result7);
    REQUIRE(result7 == std::list<std::string>{"2", "3", "7"});


    TEST_LOG << "assign a; stmt s; Select s such that Parent(s, a) pattern a('digit', _)  --shared synonyms";
    qps::evaluator::Evaluator evaluator8 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query8{};
    query8.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query8.addDeclaration("s", qps::query::DesignEntity::STMT);

    qps::query::Declaration d8 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple8 { qps::query::Elem::ofDeclaration(d8) };
    qps::query::ResultCl r8 = qps::query::ResultCl::ofTuple(tuple8);
    query8.addResultCl(r8);

    std::shared_ptr<qps::query::Parent> ptr8 = std::make_shared<qps::query::Parent>();
    ptr8->parent = qps::query::StmtRef::ofDeclaration( Declaration { "s", qps::query::DesignEntity::STMT });
    ptr8->child = qps::query::StmtRef::ofDeclaration( Declaration { "a", qps::query::DesignEntity::ASSIGN });
    query8.addSuchthat(ptr8);
    query8.addPattern(qps::query::Pattern::ofAssignPattern("a",
                                          qps::query::EntRef::ofVarName("digit"),
                                          qps::query::ExpSpec::ofWildcard()));
    std::list<std::string> result8 = evaluator8.evaluate(query8);
    result8.sort();
    printEvaluatorResult(result8);
    REQUIRE(result8 == std::list<std::string>{"3"});

    TEST_LOG << "stmt a; Select a pattern a(_, _) and a1('number', _) such that Follows(a, a1)";
    qps::evaluator::Evaluator evaluator9 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query9{};
    query9.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query9.addDeclaration("a1", qps::query::DesignEntity::ASSIGN);

    qps::query::Declaration d9 = qps::query::Declaration { "a", qps::query::DesignEntity::ASSIGN };
    std::vector<qps::query::Elem> tuple9 { qps::query::Elem::ofDeclaration(d9) };
    qps::query::ResultCl r9 = qps::query::ResultCl::ofTuple(tuple9);
    query9.addResultCl(r9);

    query9.addPattern(qps::query::Pattern::ofAssignPattern("a",
                                                           qps::query::EntRef::ofWildcard(),
                                                           qps::query::ExpSpec::ofWildcard()));
    query9.addPattern(qps::query::Pattern::ofAssignPattern("a1",
                                                           qps::query::EntRef::ofVarName("number"),
                                                           qps::query::ExpSpec::ofWildcard()));
    std::shared_ptr<qps::query::FollowsT> ptr9 = std::make_shared<qps::query::FollowsT>();
    ptr9->follower = qps::query::StmtRef::ofDeclaration( Declaration { "a", qps::query::DesignEntity::ASSIGN });
    ptr9->transitiveFollowed = qps::query::StmtRef::ofDeclaration( Declaration {
        "a1", qps::query::DesignEntity::ASSIGN });

    query9.addSuchthat(ptr9);
    std::list<std::string> result9 = evaluator9.evaluate(query9);
    result9.sort();
    printEvaluatorResult(result9);
    REQUIRE(result9 == std::list<std::string>{"4", "5"});

    TEST_LOG << "stmt a; print pr; Select a pattern a(v, _) with pr.varName = v.varName";
    qps::evaluator::Evaluator evaluator10 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query10{};
    query10.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query10.addDeclaration("pr", qps::query::DesignEntity::PRINT);
    query10.addDeclaration("v", qps::query::DesignEntity::VARIABLE);

    qps::query::Declaration d10 = qps::query::Declaration { "a", qps::query::DesignEntity::ASSIGN };
    std::vector<qps::query::Elem> tuple10 { qps::query::Elem::ofDeclaration(d10) };
    qps::query::ResultCl r10 = qps::query::ResultCl::ofTuple(tuple10);
    query10.addResultCl(r10);

    query10.addPattern(qps::query::Pattern::ofAssignPattern(
            "a",
            qps::query::EntRef::ofDeclaration( Declaration { "v", qps::query::DesignEntity::VARIABLE }),
            qps::query::ExpSpec::ofWildcard()));
    qps::query::AttrRef attrl = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                                    Declaration { "pr", qps::query::DesignEntity::PRINT }};
    qps::query::AttrRef attrr = qps::query::AttrRef{qps::query::AttrName::VARNAME,
                                                    Declaration { "v", qps::query::DesignEntity::VARIABLE }};
    qps::query::AttrCompareRef lhs = qps::query::AttrCompareRef::ofAttrRef(attrl);
    qps::query::AttrCompareRef rhs = qps::query::AttrCompareRef::ofAttrRef(attrr);
    qps::query::AttrCompare with = qps::query::AttrCompare{lhs, rhs};
    query10.addWith(with);
    std::list<std::string> result10 = evaluator10.evaluate(query10);
    result10.sort();
    printEvaluatorResult(result10);
    REQUIRE(result10 == std::list<std::string>{"2", "5"});

    TEST_LOG << "assign a; stmt s; Select s pattern a(_, 'number % 10') and a('digit',_) such that Next*(a, s)";
    qps::evaluator::Evaluator evaluator11 = qps::evaluator::Evaluator(&pkb);
    qps::query::Query query11{};
    query11.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    query11.addDeclaration("s", qps::query::DesignEntity::STMT);

    qps::query::Declaration d11 = qps::query::Declaration { "s", qps::query::DesignEntity::STMT };
    std::vector<qps::query::Elem> tuple11 { qps::query::Elem::ofDeclaration(d11) };
    qps::query::ResultCl r11 = qps::query::ResultCl::ofTuple(tuple11);
    query11.addResultCl(r11);

    query11.addPattern(qps::query::Pattern::ofAssignPattern("a", qps::query::EntRef::ofWildcard(),
                                                            qps::query::ExpSpec::ofFullMatch("number % 10")));
    query11.addPattern(qps::query::Pattern::ofAssignPattern("a",
                                                            qps::query::EntRef::ofVarName("digit"),
                                                            qps::query::ExpSpec::ofWildcard()));
    std::shared_ptr<qps::query::NextT> ptr11 = std::make_shared<qps::query::NextT>();
    ptr11->before = qps::query::StmtRef::ofDeclaration( Declaration { "a", qps::query::DesignEntity::ASSIGN });
    ptr11->transitiveAfter = qps::query::StmtRef::ofDeclaration( Declaration { "s", qps::query::DesignEntity::STMT });
    query11.addSuchthat(ptr11);
    std::list<std::string> result11 = evaluator11.evaluate(query11);
    result11.sort();
    printEvaluatorResult(result11);
    REQUIRE(result11 == std::list<std::string>{"3", "4", "5", "6", "7"});
}
