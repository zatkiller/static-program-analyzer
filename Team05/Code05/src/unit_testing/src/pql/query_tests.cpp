#include "pql/query.h"
#include "catch.hpp"

#include "exceptions.h"

#include <unordered_set>
#include <memory>

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
    SECTION("AttrRef correctness") {
        Declaration d("p", DesignEntity::PROCEDURE);
        AttrRef ar = AttrRef{ AttrName::PROCNAME, Declaration {"p", DesignEntity::PROCEDURE} };
        REQUIRE(ar.getAttrName() == AttrName::PROCNAME);
        REQUIRE(ar.getDeclaration() == d);

        AttrRef ar2 = AttrRef{ AttrName::VARNAME, Declaration { "v", DesignEntity::VARIABLE } };
        AttrRef ar3 = AttrRef{ AttrName::STMTNUM, Declaration { "s", DesignEntity::STMT} };
        AttrRef ar4 = AttrRef{ AttrName::VALUE, Declaration {"c", DesignEntity::CONSTANT} };

    REQUIRE(ar.canBeCompared(ar2));
    REQUIRE(ar3.canBeCompared(ar4));

    REQUIRE(!ar.canBeCompared(ar3));
    REQUIRE(!ar.canBeCompared(ar4));

    REQUIRE(!ar2.canBeCompared(ar3));
    REQUIRE(!ar2.canBeCompared(ar4));
}

    SECTION("AttrRef Hash") {
        AttrRef ar1 = AttrRef{ AttrName::PROCNAME, Declaration {"p", DesignEntity::PROCEDURE} };
        AttrRef ar2 = AttrRef{ AttrName::PROCNAME, Declaration {"p", DesignEntity::PROCEDURE} };
        std::unordered_set<AttrRef> aSet;
        aSet.insert(ar1);
        aSet.insert(ar2);
        REQUIRE(aSet.size() == 1);
    }
}

TEST_CASE("AttrCompareRef") {
    SECTION("AttrCompareRef correctness") {
        AttrCompareRef acr = AttrCompareRef::ofString("p");
        REQUIRE(acr.isString());
        REQUIRE(acr.getString() == "p");

        acr = AttrCompareRef::ofNumber(1);
        REQUIRE(acr.isNumber());
        REQUIRE(acr.getNumber() == 1);

        Declaration d("p", DesignEntity::PROCEDURE);
        acr = AttrCompareRef::ofAttrRef(AttrRef{ AttrName::PROCNAME, d });
        REQUIRE(acr.isAttrRef());
        AttrRef ar = acr.getAttrRef();
        REQUIRE(ar.getAttrName() == AttrName::PROCNAME);
        REQUIRE(ar.getDeclaration() == d);
    }

    SECTION("AttrCompareRef Hash") {
        std::unordered_set<AttrCompareRef> aSet;
        AttrCompareRef acr1 = AttrCompareRef::ofString("p");
        AttrCompareRef acr2 = AttrCompareRef::ofString("p");

        aSet.insert(acr1);
        aSet.insert(acr2);

        REQUIRE(aSet.size() == 1);

        AttrCompareRef acr3 = AttrCompareRef::ofNumber(1);
        AttrCompareRef acr4 = AttrCompareRef::ofNumber(1);

        aSet.insert(acr3);
        aSet.insert(acr4);

        REQUIRE(aSet.size() == 2);

        Declaration d("p", DesignEntity::PROCEDURE);
        AttrCompareRef acr5 = AttrCompareRef::ofAttrRef(AttrRef{ AttrName::PROCNAME, d });
        AttrCompareRef acr6 = AttrCompareRef::ofAttrRef(AttrRef{ AttrName::PROCNAME, d });

        aSet.insert(acr5);
        aSet.insert(acr6);

        REQUIRE(aSet.size() == 3);
    }
}

TEST_CASE("AttrCompare") {
    SECTION ("AttrCompare with compatible AttrCompareRef types") {
        Declaration d ("v", DesignEntity::VARIABLE);
        AttrCompare ac(AttrCompareRef::ofAttrRef( AttrRef { AttrName::VARNAME, d }),
                       AttrCompareRef::ofString("v"));

        AttrCompareRef lhs = ac.getLhs();
        AttrCompareRef rhs = ac.getRhs();

        REQUIRE(lhs.isAttrRef());
        AttrRef ar = lhs.getAttrRef();

        REQUIRE(ar.getDeclaration()  == d);
        REQUIRE(ar.getAttrName() == AttrName::VARNAME);

        REQUIRE(rhs.isString());
        REQUIRE(rhs.getString() == "v");
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

TEST_CASE("RelRef Hash") {
    std::unordered_set<std::shared_ptr<RelRef>> rSet;

    Follows f1;
    f1.follower = StmtRef::ofLineNo(1);
    f1.followed = StmtRef::ofLineNo(2);

    std::shared_ptr<RelRef> fPtr1 = std::make_shared<Follows>(f1);

    Follows f2;
    f2.follower = StmtRef::ofLineNo(1);
    f2.followed = StmtRef::ofLineNo(2);

    std::shared_ptr<RelRef> fPtr2 = std::make_shared<Follows>(f2);

    rSet.insert(fPtr1);
    REQUIRE(rSet.size() == 1);
    rSet.insert(fPtr2);
    REQUIRE(rSet.size() == 1);

    Follows f3;
    f3.follower = StmtRef::ofLineNo(1);
    f3.followed = StmtRef::ofWildcard();

    std::shared_ptr<RelRef> fPtr3 = std::make_shared<Follows>(f3);

    rSet.insert(fPtr3);
    REQUIRE(rSet.size() == 2);

    FollowsT f4;
    f4.follower = StmtRef::ofLineNo(1);
    f4.transitiveFollowed = StmtRef::ofWildcard();

    std::shared_ptr<RelRef> fPtr4 = std::make_shared<FollowsT>(f4);

    rSet.insert(fPtr4);
    REQUIRE(rSet.size() == 3);
}

TEST_CASE("ExpSpec") {
    ExpSpec e1 = ExpSpec::ofFullMatch("x");
    REQUIRE(e1.isFullMatch());
    REQUIRE(!e1.isPartialMatch());
    REQUIRE(!e1.isWildcard());

    ExpSpec e2 = ExpSpec::ofPartialMatch("x");
    REQUIRE(!e2.isFullMatch());
    REQUIRE(e2.isPartialMatch());
    REQUIRE(!e2.isWildcard());

    ExpSpec e3 = ExpSpec::ofWildcard();
    REQUIRE(!e3.isFullMatch());
    REQUIRE(!e3.isPartialMatch());
    REQUIRE(e3.isWildcard());
}

TEST_CASE("Pattern") {
    SECTION("Pattern correctness") {
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

    SECTION("Pattern Hash") {
        std::unordered_set<Pattern> pSet;
        Pattern p1 = Pattern::ofAssignPattern("h", EntRef::ofWildcard(), ExpSpec::ofFullMatch("x"));
        Pattern p2 = Pattern::ofAssignPattern("h", EntRef::ofWildcard(), ExpSpec::ofFullMatch("x"));

        pSet.insert(p1);
        REQUIRE(pSet.size() == 1);

        pSet.insert(p2);
        REQUIRE(pSet.size() == 1);

        Pattern p3 = Pattern::ofAssignPattern("h", EntRef::ofWildcard(), ExpSpec::ofFullMatch("y"));
        pSet.insert(p3);

        REQUIRE(pSet.size() == 2);

        Pattern p4 = Pattern::ofWhilePattern("x", EntRef::ofWildcard());
        Pattern p5 = Pattern::ofWhilePattern("x", EntRef::ofWildcard());
        Pattern p6 = Pattern::ofWhilePattern("y", EntRef::ofWildcard());

        pSet.insert(p4);
        REQUIRE(pSet.size() == 3);

        pSet.insert(p5);
        REQUIRE(pSet.size() == 3);

        pSet.insert(p6);
        REQUIRE(pSet.size() == 4);

        Pattern p7 = Pattern::ofIfPattern("y", EntRef::ofWildcard());
        Pattern p8 = Pattern::ofIfPattern("y", EntRef::ofWildcard());
        Pattern p9 = Pattern::ofIfPattern("z", EntRef::ofWildcard());

        pSet.insert(p7);
        REQUIRE(pSet.size() == 5);
        pSet.insert(p8);
        REQUIRE(pSet.size() == 5);
        pSet.insert(p9);
        REQUIRE(pSet.size() == 6);
    }
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
    REQUIRE(suchThat[0]->getDecs() == std::vector<Declaration>{Declaration{"v", DesignEntity::VARIABLE}});

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

TEST_CASE("RelRef equality check") {
    Pattern p = Pattern::ofAssignPattern("a", EntRef::ofWildcard(), ExpSpec::ofFullMatch("x"));
    Pattern p1 = Pattern::ofAssignPattern("a", EntRef::ofWildcard(), ExpSpec::ofFullMatch("x"));
    REQUIRE(p == p1);
    std::shared_ptr<ModifiesS> ptr1 = std::make_shared<ModifiesS>();
    ptr1->modifiesStmt = StmtRef::ofLineNo(4);
    ptr1->modified = EntRef::ofDeclaration( Declaration { "v", DesignEntity::VARIABLE });

    std::shared_ptr<ModifiesS> ptr2 = std::make_shared<ModifiesS>();
    ptr2->modifiesStmt = StmtRef::ofLineNo(4);
    ptr2->modified = EntRef::ofDeclaration( Declaration { "v", DesignEntity::VARIABLE });

    REQUIRE(*(ptr1) == *(ptr2));
}
