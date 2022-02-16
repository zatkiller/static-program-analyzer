#include "PKB/StatementRow.h"
#include "logging.h"
#include <iostream>
#include "catch.hpp"

TEST_CASE("StatementRow getStmt") {
    StatementRow row1{ StatementType::Assignment, 1 };

    REQUIRE(row1.getStmt() == STMT_LO{ 1, StatementType::Assignment });
}

TEST_CASE("StatementRow ==") {
    StatementRow row1{ StatementType::Assignment, 1 };
    StatementRow row2{ StatementType::Assignment, 1 };
    StatementRow row3{ StatementType::Assignment, 2 };
    StatementRow row4{ StatementType::While, 1 };
    StatementRow row5{ StatementType::While, 2 };

    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);

    REQUIRE_FALSE(row1 == row3);
    REQUIRE_FALSE(row1 == row4);
    REQUIRE_FALSE(row1 == row5);
}
