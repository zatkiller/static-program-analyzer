#include "pql/query.h"
#include "catch.hpp"

TEST_CASE("StmtRef") {
    StmtRef stmtRef;
    REQUIRE(stmtRef.getType() == StmtRefType::NOT_INITIALIZED);

    StmtRef stmtRef1;
    stmtRef1.setType(StmtRefType::LINE_NO);
    stmtRef1.setLineNo(1);
    REQUIRE(stmtRef1.isLineNo());
    REQUIRE(stmtRef1.getLineNo() == 1);

    StmtRef stmtRef2;
    stmtRef2.setType(StmtRefType::WILDCARD);
    REQUIRE(stmtRef2.isWildcard());

    StmtRef stmtRef3;
    stmtRef3.setType(StmtRefType::DECLARATION);
    stmtRef3.setDeclaration("a");
    REQUIRE(stmtRef3.isDeclaration());
    REQUIRE(stmtRef3.getDeclaration() == "a");
}

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