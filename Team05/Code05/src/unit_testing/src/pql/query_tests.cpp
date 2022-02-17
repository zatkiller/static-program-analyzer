#include "pql/query.h"
#include "catch.hpp"


using qps::query::Query;
using qps::query::StmtRef;
using qps::query::EntRef;
using qps::query::RelRef;
using qps::query::RelRefType;
using qps::query::Uses;
using qps::query::Modifies;
using qps::query::Parent;
using qps::query::ParentT;
using qps::query::Follows;
using qps::query::FollowsT;
using qps::query::Pattern;
using qps::query::StmtRefType;
using qps::query::EntRefType;
using qps::query::DesignEntity;


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

    REQUIRE(!(stmtRef1 == stmtRef2));
    REQUIRE(stmtRef1 == stmtRef1);
}

TEST_CASE("EntRef") {
    EntRef entRef;
    REQUIRE(entRef.getType() == EntRefType::NOT_INITIALIZED);

    EntRef entRef1 = EntRef::ofVarName("a");
    REQUIRE(entRef1.isVarName());
    REQUIRE(entRef1.getVariableName() == "a");

    EntRef entRef2 = EntRef::ofWildcard();
    REQUIRE(entRef2.isWildcard());

    EntRef entRef3 = EntRef::ofDeclaration("a");
    REQUIRE(entRef3.isDeclaration());
    REQUIRE(entRef3.getDeclaration() == "a");

    REQUIRE(entRef1 == entRef1);
    REQUIRE(!(entRef1 == entRef2));
}

TEST_CASE("Modifies") {
    Modifies modifies;
    REQUIRE(modifies.getType() == RelRefType::MODIFIESS);
}

TEST_CASE("Uses") {
    Uses uses;
    REQUIRE(uses.getType() == RelRefType::USESS);
}

TEST_CASE("Follows") {
    Follows follows;
    REQUIRE(follows.getType() == RelRefType::FOLLOWS);
}

TEST_CASE("Follows*") {
    FollowsT follows;
    REQUIRE(follows.getType() == RelRefType::FOLLOWST);
}

TEST_CASE("Parent") {
    Parent p;
    REQUIRE(p.getType() == RelRefType::PARENT);
}

TEST_CASE("Parent*") {
    ParentT p;
    REQUIRE(p.getType() == RelRefType::PARENTT);
}

TEST_CASE("Pattern") {
    Pattern p = {"h", EntRef::ofWildcard(), "_x_"};
    REQUIRE(p.getSynonym() == "h");
    REQUIRE(p.getEntRef().getType() == EntRefType::WILDCARD);
    REQUIRE(p.getExpression() == "_x_");

    Pattern p2 = {"g", EntRef::ofWildcard(), "_x_"};
    REQUIRE(!(p == p2));
    REQUIRE(p2 == p2);
}

TEST_CASE("Query") {
    Query query = Query{};
    query.addDeclaration("a", DesignEntity::STMT);
    REQUIRE(query.hasDeclaration("a"));

    query.addVariable("a");
    REQUIRE(query.hasVariable("a"));

    std::shared_ptr<Modifies> ptr = std::make_shared<Modifies>();
    ptr.get()->modifiesStmt = StmtRef::ofLineNo(4);
    ptr.get()->modified = EntRef::ofDeclaration("v");
    query.addSuchthat(ptr);
    REQUIRE(!query.getSuchthat().empty());
    REQUIRE(query.getSuchthat()[0] == ptr);

    std::vector<std::shared_ptr<RelRef>> suchThat = query.getSuchthat();
    REQUIRE(suchThat[0]->getSyns() == std::vector<std::string>{"v"});

    std::vector<PKBField> fields = suchThat[0]->getField();
    REQUIRE(fields[0].entityType == PKBEntityType::STATEMENT);
    REQUIRE(fields[0].fieldType == PKBFieldType::CONCRETE);
    REQUIRE(fields[0].getContent<STMT_LO>()->statementNum == 4);
    REQUIRE(fields[1].entityType == PKBEntityType::VARIABLE);
    REQUIRE(fields[1].fieldType == PKBFieldType::DECLARATION);

    Pattern p = Pattern{"a", EntRef::ofWildcard(), "_x_"};
    query.addPattern(p);
    REQUIRE(!query.getPattern().empty());
    REQUIRE(query.getPattern()[0] == p);

    REQUIRE(query.getDeclarationDesignEntity("a") == DesignEntity::STMT);
}
