#include "pql/query.h"
#include "catch.hpp"

TEST_CASE("StmtRef") {
    StmtRef stmtRef;
    REQUIRE(stmtRef.getType() == StmtRefType::NOT_INITIALIZED);

    StmtRef stmtRef1 = StmtRef::ofLineNo(1);
    REQUIRE(stmtRef1.isLineNo());
    REQUIRE(stmtRef1.getLineNo() == 1);

    StmtRef stmtRef2 = StmtRef::ofWildcard();
    REQUIRE(stmtRef2.isWildcard());

    StmtRef stmtRef3 = StmtRef::ofDeclaration("a");
    REQUIRE(stmtRef3.isDeclaration());
    REQUIRE(stmtRef3.getDeclaration() == "a");
}

TEST_CASE("EntRef") {
    EntRef entRef;
    REQUIRE(entRef.getType() == EntRefType::NOT_INITIALIZED);

    EntRef entRef1 = EntRef::ofVarName("a");
    REQUIRE(entRef1.isVarName());
    REQUIRE(entRef1.getVariableName() == "a");

    EntRef entRef2  = EntRef::ofWildcard();
    REQUIRE(entRef2.isWildcard());

    EntRef entRef3 = EntRef::ofDeclaration("a");
    REQUIRE(entRef3.isDeclaration());
    REQUIRE(entRef3.getDeclaration() == "a");
}

TEST_CASE("Modifies") {
    Modifies modifies;
    REQUIRE(modifies.getType() == RelRefType::MODIFIESS);
}

TEST_CASE("Uses") {
    Uses uses;
    REQUIRE(uses.getType() == RelRefType::USESS);
}

TEST_CASE("Pattern") {
    Pattern p = { "h", EntRef::ofWildcard(), "_x_"};
    REQUIRE(p.getSynonym() == "h");
    REQUIRE(p.getEntRef().getType() == EntRefType::WILDCARD);
    REQUIRE(p.getExpression() == "_x_");
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