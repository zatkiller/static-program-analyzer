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

    std::unordered_set<PKBField, PKBFieldHash> expectedSelectAll{a1, a2, a3, w1, i1, i2, p1};
    std::unordered_set<PKBField, PKBFieldHash> expectedSelectAssign {a1, a2, a3};
    std::unordered_set<PKBField, PKBFieldHash> expectedSelectWhile{w1};
    std::unordered_set<PKBField, PKBFieldHash> expectedSelectIf{i1, i2};
    std::unordered_set<PKBField, PKBFieldHash> expectedSelectPrint{p1};
    std::unordered_set<PKBField, PKBFieldHash> expectedSelectRead{};

    PKBResponse resAll = evaluator.getAll(qps::query::DesignEntity::STMT);
    PKBResponse resAss = evaluator.getAll(qps::query::DesignEntity::ASSIGN);
    PKBResponse resWhile = evaluator.getAll(qps::query::DesignEntity::WHILE);
    PKBResponse resIf = evaluator.getAll(qps::query::DesignEntity::IF);
    PKBResponse resPrint = evaluator.getAll(qps::query::DesignEntity::PRINT);
    PKBResponse resRead = evaluator.getAll(qps::query::DesignEntity::READ);

    std::unordered_set<PKBField, PKBFieldHash> setAll =
        *(std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&resAll.res));
    std::unordered_set<PKBField, PKBFieldHash> setAss =
        *(std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&resAss.res));
    std::unordered_set<PKBField, PKBFieldHash> setWhile =
        *(std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&resWhile.res));
    std::unordered_set<PKBField, PKBFieldHash> setIf =
        *(std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&resIf.res));
    std::unordered_set<PKBField, PKBFieldHash> setPrint =
        *(std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&resPrint.res));
    std::unordered_set<PKBField, PKBFieldHash> setRead =
        *(std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&resRead.res));

    int countAssign = setAss.size();
    REQUIRE(countAssign == 3);

    REQUIRE(setAll == expectedSelectAll);
    REQUIRE(setAss == expectedSelectAssign);
    REQUIRE(setWhile == expectedSelectWhile);
    REQUIRE(setIf == expectedSelectIf);
    REQUIRE(setPrint == expectedSelectPrint);
    REQUIRE(resRead.hasResult == false);
    REQUIRE(setRead == expectedSelectRead);
}

TEST_CASE("Test get variables") {
    PKB pkb = PKB();
    PKB* ptr = &pkb;
    qps::evaluator::Evaluator evaluator = qps::evaluator::Evaluator(ptr);

    REQUIRE(evaluator.getAll(qps::query::DesignEntity::VARIABLE).hasResult == false);

    pkb.insertEntity(VAR_NAME{ "x" });
    pkb.insertEntity(VAR_NAME{ "y" });
    pkb.insertEntity(VAR_NAME{ "cur" });

    PKBField v1 = PKBField::createConcrete(VAR_NAME{ "x" });
    PKBField v2 = PKBField::createConcrete(VAR_NAME{ "y" });
    PKBField v3 = PKBField::createConcrete(VAR_NAME{ "cur" });

    std::unordered_set<PKBField, PKBFieldHash> expectedSelectVar{v1, v2, v3};

    PKBResponse resVar = evaluator.getAll(qps::query::DesignEntity::VARIABLE);
    std::unordered_set<PKBField, PKBFieldHash> setVar =
        *(std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&resVar.res));
    REQUIRE(setVar == expectedSelectVar);
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
