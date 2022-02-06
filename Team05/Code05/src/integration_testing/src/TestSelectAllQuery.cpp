#include "PKB.h"
#include "PKB/StatementType.h"
#include "pql/evaluator.h"
#include "catch.hpp"

TEST_CASE("Test get statements") {
    PKB pkb = PKB();
    pkb.insertStatement(StatementType::Assignment, 2);
    pkb.insertStatement(StatementType::Assignment, 5);
    pkb.insertStatement(StatementType::Assignment, 8);
    pkb.insertStatement(StatementType::While, 3);
    pkb.insertStatement(StatementType::If, 4);
    pkb.insertStatement(StatementType::If, 6);
    pkb.insertStatement(StatementType::Print, 7);


}