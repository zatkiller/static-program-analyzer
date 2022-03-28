#include "pql/query.h"
#include "catch.hpp"

#include "exceptions.h"

using qps::query::Query;
using qps::query::Elem;
using qps::query::ResultCl;
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
using qps::query::Declaration;

TEST_CASE("AttrRef") {
    Declaration d("p", DesignEntity::PROCEDURE);
    AttrRef ar = AttrRef { AttrName::PROCNAME, Declaration {"p", DesignEntity::PROCEDURE} };
    REQUIRE(ar.getAttrName() == AttrName::PROCNAME);
    REQUIRE(ar.getDeclaration() == d);

    AttrRef ar2 = AttrRef { AttrName::VARNAME, Declaration { "v", DesignEntity::VARIABLE } };
    AttrRef ar3 = AttrRef { AttrName::STMTNUM, Declaration { "s", DesignEntity::STMT} };
    AttrRef ar4 = AttrRef { AttrName::VALUE, Declaration {"c", DesignEntity::CONSTANT} };

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

    Declaration d("p", DesignEntity::PROCEDURE);
    acr = AttrCompareRef::ofAttrRef(AttrRef { AttrName::PROCNAME, d });
    REQUIRE(acr.isAttrRef());
    AttrRef ar = acr.getAttrRef();
    REQUIRE(ar.getAttrName() == AttrName::PROCNAME);
    REQUIRE(ar.getDeclaration() == d);
}

TEST_CASE("AttrCompare") {
    SECTION ("AttrCompare with compatible AttrCompareRef types") {
        Declaration d ("v", DesignEntity::VARIABLE);
        AttrCompare ac(AttrCompareRef::ofAttrRef( AttrRef { AttrName::VARNAME, d }),
                       AttrCompareRef::ofString("v"));

        REQUIRE_NOTHROW(ac.validateComparingTypes());
        AttrCompareRef lhs = ac.getLhs();
        AttrCompareRef rhs = ac.getRhs();

        REQUIRE(lhs.isAttrRef());
        AttrRef ar = lhs.getAttrRef();

        REQUIRE(ar.getDeclaration()  == d);
        REQUIRE(ar.getAttrName() == AttrName::VARNAME);

        REQUIRE(rhs.isString());
        REQUIRE(rhs.getString() == "v");
    }

    SECTION ("AttrCompare validateComparingTypes") {
        SECTION ("Incompatible type matches") {
            AttrCompare ac1(AttrCompareRef::ofAttrRef( AttrRef {AttrName::STMTNUM, Declaration {
                "rd", DesignEntity::READ } }), AttrCompareRef::ofString("v"));

            REQUIRE_THROWS_MATCHES(ac1.validateComparingTypes(), exceptions::PqlSemanticException,
                                   Catch::Message(messages::qps::parser::incompatibleComparisonMessage));

            AttrCompare ac2(AttrCompareRef::ofNumber(1), AttrCompareRef::ofAttrRef(
                    AttrRef {AttrName::VARNAME, Declaration { "v", DesignEntity::VARIABLE }}));

            REQUIRE_THROWS_MATCHES(ac2.validateComparingTypes(), exceptions::PqlSemanticException,
                                   Catch::Message(messages::qps::parser::incompatibleComparisonMessage));

            AttrCompare ac3(AttrCompareRef::ofString("v"), AttrCompareRef::ofNumber(1));

            REQUIRE_THROWS_MATCHES(ac3.validateComparingTypes(), exceptions::PqlSemanticException,
                                   Catch::Message(messages::qps::parser::incompatibleComparisonMessage));
        }

        SECTION ("Compatible type matches") {
            AttrCompare ac1(AttrCompareRef::ofAttrRef( AttrRef {AttrName::PROCNAME, Declaration {
                "p", DesignEntity::PROCEDURE }}), AttrCompareRef::ofString("v"));
            REQUIRE_NOTHROW(ac1.validateComparingTypes());

            AttrCompare ac2(AttrCompareRef::ofString("v"), AttrCompareRef::ofAttrRef(
                    AttrRef {AttrName::VARNAME, Declaration { "v", DesignEntity::VARIABLE, }}));
            REQUIRE_NOTHROW(ac2.validateComparingTypes());


            AttrCompare ac3(AttrCompareRef::ofAttrRef( AttrRef {AttrName::STMTNUM,  Declaration {
                "cl", DesignEntity::CALL}}), AttrCompareRef::ofNumber(200));
            REQUIRE_NOTHROW(ac3.validateComparingTypes());

            AttrCompare ac4(AttrCompareRef::ofNumber(99), AttrCompareRef::ofAttrRef(
                    AttrRef {AttrName::VALUE, Declaration { "c", DesignEntity::CONSTANT }}));
            REQUIRE_NOTHROW(ac4.validateComparingTypes());

            AttrCompare ac5(AttrCompareRef::ofAttrRef( AttrRef {AttrName::PROCNAME, Declaration {
                "p", DesignEntity::PROCEDURE }}), AttrCompareRef::ofAttrRef( AttrRef {
                    AttrName::VARNAME, Declaration { "v", DesignEntity::VARIABLE }}));

            REQUIRE_NOTHROW(ac5.validateComparingTypes());

            AttrCompare ac6(AttrCompareRef::ofAttrRef( AttrRef {AttrName::STMTNUM, Declaration {
                "cl", DesignEntity::CALL }}), AttrCompareRef::ofAttrRef(AttrRef {
                    AttrName::VALUE, Declaration { "c", DesignEntity::CONSTANT }}));

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

    StmtRef stmtRef3 = StmtRef::ofDeclaration(Declaration { "a", DesignEntity::ASSIGN });
    REQUIRE(stmtRef3.isDeclaration());
    REQUIRE(stmtRef3.getDeclaration() == Declaration { "a", DesignEntity::ASSIGN });
    REQUIRE(stmtRef3.getDeclarationSynonym() == "a");
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

    EntRef entRef3 = EntRef::ofDeclaration( Declaration { "a", DesignEntity::ASSIGN } );
    REQUIRE(entRef3.isDeclaration());
    REQUIRE(entRef3.getDeclaration() ==  Declaration { "a", DesignEntity::ASSIGN });
    REQUIRE(entRef3.getDeclarationSynonym() == "a");
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
    Pattern p = Pattern::ofAssignPattern("h", EntRef::ofWildcard(), ExpSpec::ofFullMatch("x"));
    REQUIRE(p.getSynonym() == "h");
    REQUIRE(p.getSynonymType() == DesignEntity::ASSIGN);
    REQUIRE(p.getEntRef().getType() == EntRefType::WILDCARD);
    REQUIRE(p.getExpression() == ExpSpec::ofFullMatch("x"));

    Pattern p2 = Pattern::ofAssignPattern("g", EntRef::ofWildcard(), ExpSpec::ofFullMatch("x"));
    REQUIRE(!(p == p2));
    REQUIRE(p2 == p2);

    Pattern p3 = Pattern::ofWhilePattern("x", EntRef::ofWildcard());
    REQUIRE(p3.getSynonym() == "x");
    REQUIRE(p3.getSynonymType() == DesignEntity::WHILE);
    REQUIRE(p3.getEntRef().getType() == EntRefType::WILDCARD);

    Pattern p4 = Pattern::ofIfPattern("y", EntRef::ofWildcard());
    REQUIRE(p4.getSynonym() == "y");
    REQUIRE(p4.getSynonymType() == DesignEntity::IF);
    REQUIRE(p4.getEntRef().getType() == EntRefType::WILDCARD);
}

TEST_CASE("Query") {
    Query query = Query{};
    query.addDeclaration("a", DesignEntity::STMT);
    REQUIRE(query.hasDeclaration("a"));

    std::vector<Elem> tuple;
    Elem e = Elem::ofDeclaration(Declaration { "a", DesignEntity::STMT });
    tuple.push_back(e);
    query.addResultCl(ResultCl::ofTuple(tuple));
    REQUIRE(query.hasSelectElem(e));

    std::shared_ptr<ModifiesS> ptr = std::make_shared<ModifiesS>();
    ptr->modifiesStmt = StmtRef::ofLineNo(4);
    ptr->modified = EntRef::ofDeclaration( Declaration { "v", DesignEntity::VARIABLE });
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

    Pattern p = Pattern::ofAssignPattern("a", EntRef::ofWildcard(), ExpSpec::ofFullMatch("x"));
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
