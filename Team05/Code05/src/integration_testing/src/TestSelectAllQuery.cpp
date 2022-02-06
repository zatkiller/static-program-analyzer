#include "PKB.h"
#include "PKB/StatementType.h"
#include "PKB/PKBDataTypes.h"
#include "PKB/PKBType.h"
#include "PKB/PKBField.h"
#include "pql/evaluator.h"
#include "catch.hpp"

TEST_CASE("Test get statements") {
    PKB pkb = PKB();
    PKB* pkbptr = &pkb;
    Evaluator evaluator = Evaluator(pkbptr);
    pkb.insertStatement(StatementType::Assignment, 2);
    pkb.insertStatement(StatementType::Assignment, 5);
    pkb.insertStatement(StatementType::Assignment, 8);
    pkb.insertStatement(StatementType::While, 3);
    pkb.insertStatement(StatementType::If, 4);
    pkb.insertStatement(StatementType::If, 6);
    pkb.insertStatement(StatementType::Print, 7);

    PKBField a1{PKBType::STATEMENT, true, Content{STMT_LO{2, StatementType::Assignment}}};
    PKBField a2{PKBType::STATEMENT, true, Content{STMT_LO{5, StatementType::Assignment}}};
    PKBField a3{PKBType::STATEMENT, true, Content{STMT_LO{8, StatementType::Assignment}}};
    PKBField w1{PKBType::STATEMENT, true, Content{STMT_LO{3, StatementType::While}}};
    PKBField i1{PKBType::STATEMENT, true, Content{STMT_LO{4, StatementType::If}}};
    PKBField i2{PKBType::STATEMENT, true, Content{STMT_LO{6, StatementType::If}}};
    PKBField p1{PKBType::STATEMENT, true, Content{STMT_LO{7, StatementType::Print}}};

    std::unordered_set<PKBField, PKBFieldHash> expectedSelectAll;
    expectedSelectAll.insert(a1);
    expectedSelectAll.insert(a2);
    expectedSelectAll.insert(a3);
    expectedSelectAll.insert(w1);
    expectedSelectAll.insert(i1);
    expectedSelectAll.insert(i2);
    expectedSelectAll.insert(p1);
    std::unordered_set<PKBField, PKBFieldHash> expectedSelectAssign;
    expectedSelectAll.insert(a1);
    expectedSelectAll.insert(a2);
    expectedSelectAll.insert(a3);
    std::unordered_set<PKBField, PKBFieldHash> expectedSelectWhile;
    expectedSelectAll.insert(w1);
    std::unordered_set<PKBField, PKBFieldHash> expectedSelectIf;
    expectedSelectAll.insert(i1);
    expectedSelectAll.insert(i2);
    std::unordered_set<PKBField, PKBFieldHash> expectedSelectCall;

    REQUIRE(std::get<std::unordered_set<PKBField, PKBFieldHash>>(evaluator.getAll(DesignEntity::STMT).res) == expectedSelectAll);
    REQUIRE(std::get<std::unordered_set<PKBField, PKBFieldHash>>(evaluator.getAll(DesignEntity::ASSIGN).res) == expectedSelectAssign);
    REQUIRE(std::get<std::unordered_set<PKBField, PKBFieldHash>>(evaluator.getAll(DesignEntity::WHILE).res) == expectedSelectWhile);
    REQUIRE(std::get<std::unordered_set<PKBField, PKBFieldHash>>(evaluator.getAll(DesignEntity::IF).res) == expectedSelectIf);
    REQUIRE(evaluator.getAll(DesignEntity::ASSIGN).hasResult == false);

}