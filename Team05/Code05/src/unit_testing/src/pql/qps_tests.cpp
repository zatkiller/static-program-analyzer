#include <list>

#include "catch.hpp"

#include "PKB.h"
#include "pql/qps.h"

TEST_CASE("QPS") {
    SECTION("QPS- evaluate invalid syntax query") {
        std::list<std::string> results;
        qps::QPS qps;
        PKB pkb;
        qps.evaluate("stmt s; Select v", results, &pkb);

        REQUIRE(results.size() == 0);
    }

    SECTION("QPS- evaluate invalid semantics query") {
        std::list<std::string> results;
        qps::QPS qps;
        PKB pkb;
        qps.evaluate("stmt s; variable v, v1; Select s such that Parent(v, v1)", results, &pkb);

        REQUIRE(results.size() == 0);
    }
}
