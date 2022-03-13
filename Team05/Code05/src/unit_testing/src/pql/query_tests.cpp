#include "pql/query.h"
#include "catch.hpp"

#include "exceptions.h"

using qps::query::Query;
using qps::query::StmtRef;
using qps::query::EntRef;
using qps::query::RelRef;
using qps::query::RelRefType;
using qps::query::UsesS;
using qps::query::ModifiesS;
using qps::query::Parent;
using qps::query::ParentT;
using qps::query::Follows;
using qps::query::FollowsT;
using qps::query::Pattern;
using qps::query::StmtRefType;
using qps::query::EntRefType;
using qps::query::DesignEntity;
using qps::query::ExpSpec;
using qps::query::AttrName;
using qps::query::AttrRef;
using qps::query::AttrCompare;
using qps::query::AttrCompareRef;
using qps::query::AttrCompareRefType;

TEST_CASE("AttrRef") {
    AttrRef ar = AttrRef { AttrName::PROCNAME, DesignEntity::PROCEDURE, "p" };
    REQUIRE(ar.attrName == AttrName::PROCNAME);
    REQUIRE(ar.declarationType == DesignEntity::PROCEDURE);
    REQUIRE(ar.declaration == "p");

    AttrRef ar2 = AttrRef { AttrName::VARNAME, DesignEntity::VARIABLE, "v" };
    AttrRef ar3 = AttrRef { AttrName::STMTNUM, DesignEntity::STMT, "s" };
    AttrRef ar4 = AttrRef { AttrName::VALUE, DesignEntity::CONSTANT, "c" };

    REQUIRE(ar.compatbileComparison(ar2));
    REQUIRE(ar3.compatbileComparison(ar4));

    REQUIRE(!ar.compatbileComparison(ar3));
    REQUIRE(!ar.compatbileComparison(ar4));

    REQUIRE(!ar2.compatbileComparison(ar3));
    REQUIRE(!ar2.compatbileComparison(ar4));
}

TEST_CASE("AttrCompareRef") {
    AttrCompareRef acr = AttrCompareRef::ofString("p");
    REQUIRE(acr.isString());
    REQUIRE(acr.getString() == "p");

    acr = AttrCompareRef::ofNumber(1);
    REQUIRE(acr.isNumber());
    REQUIRE(acr.getNumber() == 1);

    acr = AttrCompareRef::ofAttrRef(AttrRef { AttrName::PROCNAME, DesignEntity::PROCEDURE, "p" });
    REQUIRE(acr.isAttrRef());
    AttrRef ar = acr.getAttrRef();
    REQUIRE(ar.attrName == AttrName::PROCNAME);
    REQUIRE(ar.declarationType == DesignEntity::PROCEDURE);
    REQUIRE(ar.declaration == "p");
}

TEST_CASE("AttrCompare") {
    SECTION ("AttrCompare with compatible AttrCompareRef types") {
        AttrCompare ac(AttrCompareRef::ofAttrRef( AttrRef {AttrName::VARNAME, DesignEntity::VARIABLE, "v" }),
                       AttrCompareRef::ofString("v"));

        REQUIRE_NOTHROW(ac.validateComparingTypes());
        AttrCompareRef lhs = ac.getLhs();
        AttrCompareRef rhs = ac.getRhs();

        REQUIRE(lhs.isAttrRef());
        AttrRef ar = lhs.getAttrRef();

        REQUIRE(ar.declaration == "v");
        REQUIRE(ar.declarationType == DesignEntity::VARIABLE);
        REQUIRE(ar.attrName == AttrName::VARNAME);

        REQUIRE(rhs.isString());
        REQUIRE(rhs.getString() == "v");
    }

    SECTION ("AttrCompare validateComparingTypes") {
        SECTION ("Incompatible type matches") {
            AttrCompare ac1(AttrCompareRef::ofAttrRef( AttrRef {AttrName::STMTNUM, DesignEntity::READ, "rd" }),
                            AttrCompareRef::ofString("v"));

            REQUIRE_THROWS_MATCHES(ac1.validateComparingTypes(), exceptions::PqlSemanticException,
                                   Catch::Message(messages::qps::parser::incompatibleComparisonMessage));

            AttrCompare ac2(AttrCompareRef::ofNumber(1), AttrCompareRef::ofAttrRef(
                    AttrRef {AttrName::VARNAME, DesignEntity::VARIABLE, "v" }));

            REQUIRE_THROWS_MATCHES(ac2.validateComparingTypes(), exceptions::PqlSemanticException,
                                   Catch::Message(messages::qps::parser::incompatibleComparisonMessage));

            AttrCompare ac3(AttrCompareRef::ofString("v"), AttrCompareRef::ofNumber(1));

            REQUIRE_THROWS_MATCHES(ac3.validateComparingTypes(), exceptions::PqlSemanticException,
                                   Catch::Message(messages::qps::parser::incompatibleComparisonMessage));
        }

        SECTION ("Compatible type matches") {
            AttrCompare ac1(AttrCompareRef::ofAttrRef( AttrRef {AttrName::PROCNAME, DesignEntity::PROCEDURE, "p" }),
                            AttrCompareRef::ofString("v"));
            REQUIRE_NOTHROW(ac1.validateComparingTypes());

            AttrCompare ac2(AttrCompareRef::ofString("v"), AttrCompareRef::ofAttrRef(
                    AttrRef {AttrName::VARNAME, DesignEntity::VARIABLE, "v" }));
            REQUIRE_NOTHROW(ac2.validateComparingTypes());


            AttrCompare ac3(AttrCompareRef::ofAttrRef( AttrRef {AttrName::STMTNUM, DesignEntity::CALL, "cl" }),
                            AttrCompareRef::ofNumber(200));
            REQUIRE_NOTHROW(ac3.validateComparingTypes());

            AttrCompare ac4(AttrCompareRef::ofNumber(99), AttrCompareRef::ofAttrRef(
                    AttrRef {AttrName::VALUE, DesignEntity::CONSTANT, "c" }));
            REQUIRE_NOTHROW(ac4.validateComparingTypes());

            AttrCompare ac5(AttrCompareRef::ofAttrRef( AttrRef {AttrName::PROCNAME, DesignEntity::PROCEDURE, "p" }),
                            AttrCompareRef::ofAttrRef( AttrRef {AttrName::VARNAME, DesignEntity::VARIABLE, "v" }));
            REQUIRE_NOTHROW(ac5.validateComparingTypes());

            AttrCompare ac6(AttrCompareRef::ofAttrRef( AttrRef {AttrName::STMTNUM, DesignEntity::CALL, "cl" }),
                            AttrCompareRef::ofAttrRef(AttrRef {AttrName::VALUE, DesignEntity::CONSTANT, "c" }));
            REQUIRE_NOTHROW(ac6.validateComparingTypes());
        }
    }
}


TEST_CASE("StmtRef") {
    StmtRef stmtRef;
    REQUIRE(stmtRef.getType() == StmtRefType::NOT_INITIALIZED);

    StmtRef stmtRef1 = StmtRef::ofLineNo(1);
    REQUIRE(stmtRef1.isLineNo());
    REQUIRE(stmtRef1.getLineNo() == 1);

    StmtRef stmtRef2 = StmtRef::ofWildcard();
    REQUIRE(stmtRef2.isWildcard());

    StmtRef stmtRef3 = StmtRef::ofDeclaration("a", DesignEntity::ASSIGN);
    REQUIRE(stmtRef3.isDeclaration());
    REQUIRE(stmtRef3.getDeclaration() == "a");
    REQUIRE(stmtRef3.getDeclarationType() == DesignEntity::ASSIGN);


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

    EntRef entRef3 = EntRef::ofDeclaration("a", DesignEntity::ASSIGN);
    REQUIRE(entRef3.isDeclaration());
    REQUIRE(entRef3.getDeclaration() == "a");
    REQUIRE(entRef3.getDeclarationType() == DesignEntity::ASSIGN);

    REQUIRE(entRef1 == entRef1);
    REQUIRE(!(entRef1 == entRef2));
}

TEST_CASE("Modifies") {
    ModifiesS modifies;
    REQUIRE(modifies.getType() == RelRefType::MODIFIESS);
}

TEST_CASE("Uses") {
    UsesS uses;
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
    Pattern p = {"h", EntRef::ofWildcard(), ExpSpec::ofFullMatch("x")};
    REQUIRE(p.getSynonym() == "h");
    REQUIRE(p.getEntRef().getType() == EntRefType::WILDCARD);
    REQUIRE(p.getExpression() == ExpSpec::ofFullMatch("x"));

    Pattern p2 = {"g", EntRef::ofWildcard(), ExpSpec::ofFullMatch("x")};
    REQUIRE(!(p == p2));
    REQUIRE(p2 == p2);
}

TEST_CASE("Query") {
    Query query = Query{};
    query.addDeclaration("a", DesignEntity::STMT);
    REQUIRE(query.hasDeclaration("a"));

    query.addVariable("a");
    REQUIRE(query.hasVariable("a"));

    std::shared_ptr<ModifiesS> ptr = std::make_shared<ModifiesS>();
    ptr.get()->modifiesStmt = StmtRef::ofLineNo(4);
    ptr.get()->modified = EntRef::ofDeclaration("v", DesignEntity::VARIABLE);
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

    Pattern p = Pattern{"a", EntRef::ofWildcard(), ExpSpec::ofFullMatch("x")};
    query.addPattern(p);
    REQUIRE(!query.getPattern().empty());
    REQUIRE(query.getPattern()[0] == p);

    REQUIRE(query.getDeclarationDesignEntity("a") == DesignEntity::STMT);

    query.addWith(AttrCompare { AttrCompareRef::ofNumber(1), AttrCompareRef::ofString("v") });
    REQUIRE(!query.getWith().empty());
    AttrCompare ac2 = query.getWith()[0];
    AttrCompareRef lhs = ac2.getLhs();
    AttrCompareRef rhs = ac2.getRhs();

    REQUIRE(lhs.isNumber());
    REQUIRE(lhs.getNumber() == 1);
    REQUIRE(rhs.isString());
    REQUIRE(rhs.getString() == "v");
}

TEST_CASE("Query getDeclarationDesignEntity") {
    Query query {};
    query.addDeclaration("a", DesignEntity::ASSIGN);

    REQUIRE(query.getDeclarationDesignEntity("a") == DesignEntity::ASSIGN);
    REQUIRE_THROWS_MATCHES(query.getDeclarationDesignEntity("b") ,
                           exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::declarationDoesNotExistMessage));
}
