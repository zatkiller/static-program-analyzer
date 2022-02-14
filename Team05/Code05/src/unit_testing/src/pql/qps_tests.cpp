#include <list>

#include "catch.hpp"

#include "PKB.h"
#include "pql/qps.h"

TEST_CASE("QPS") {
    SECTION("QPS- evaluate invalid query") {
        std::list<std::string> results;
        qps::QPS qps;
        PKB pkb;
        qps.evaluate("Nani", results, &pkb);

        REQUIRE(results.size() == 1);
        REQUIRE(results.front() == "Invalid query!");
    }
}
