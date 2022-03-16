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
    query1.addVariable("s");
    query1.addDeclaration("s", qps::query::DesignEntity::STMT);
    std::list<std::string> resAll = evaluator.evaluate(query1);
    resAll.sort();
    REQUIRE(resAll == std::list<std::string>{"2", "3", "4", "5", "6", "7", "8"});

    qps::query::Query query2;
    query2.addVariable("a");
    query2.addDeclaration("a", qps::query::DesignEntity::ASSIGN);
    std::list<std::string> resAss = evaluator.evaluate(query2);
    resAss.sort();
    REQUIRE(resAss == std::list<std::string>{"2", "5","8"});

    qps::query::Query query3;
    query3.addVariable("w");
    query3.addDeclaration("w", qps::query::DesignEntity::WHILE);
    std::list<std::string> resWhile = evaluator.evaluate(query3);
    resWhile.sort();
    REQUIRE(resWhile == std::list<std::string>{"3"});

    qps::query::Query query4;
    query4.addVariable("ifs");
    query4.addDeclaration("ifs", qps::query::DesignEntity::IF);
    std::list<std::string> resIf = evaluator.evaluate(query4);
    resIf.sort();
    REQUIRE(resIf == std::list<std::string>{"4", "6"});

    qps::query::Query query5;
    query5.addVariable("pr");
    query5.addDeclaration("pr", qps::query::DesignEntity::PRINT);
    std::list<std::string> resPrint = evaluator.evaluate(query5);
    resPrint.sort();
    REQUIRE(resPrint == std::list<std::string>{"7"});

    qps::query::Query query6;
    query6.addVariable("r");
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
    query1.addVariable("v");
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

    q.addVariable("s");

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

    q.addVariable("c");

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
    q.addVariable("a");

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
    q.addVariable("v");

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
