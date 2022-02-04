#include "pql/evaluator.h"
#include "catch.hpp"

TEST_CASE("query noSuchthat noPattern stmtLo") {
    Query query = Query{};
    query.addDeclaration("s", DesignEntity::STMT);
    query.addVariable("s");
    REQUIRE(evaluate(query) == "1, 2, 3");
}

TEST_CASE("query noSuchthat noPattern variables") {
Query query = Query{};
query.addDeclaration("v", DesignEntity::VARIABLE);
query.addVariable("v");
REQUIRE(evaluate(query) == "a, present, x, y");
}