#include "pql/query.h"
#include "catch.hpp"

TEST_CASE("Query hasDeclaration") {
    Query query = Query{};
    REQUIRE(!query.hasDeclaration("a"));

    query.addDeclaration("a", DesignEntity::STMT);
    REQUIRE(query.hasDeclaration("a"));
}

TEST_CASE("Query hasVariable") {
    Query query = Query{};
    REQUIRE(!query.hasVariable("a"));

    query.addVariable("a");
    REQUIRE(query.hasVariable("a"));
}