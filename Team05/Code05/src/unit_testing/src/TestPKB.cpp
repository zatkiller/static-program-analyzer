#include <iostream>

#include "logging.h"
#include "PKB.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

#define TEST_LOG Logger() << "TestPKB.cpp "

/**
 * Ensures token structs and token type are correct.
 */
TEST_CASE("PKB testing") {
    std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());

    PKBField field1 = PKBField::createConcrete(PROC_NAME{ "main" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field3 = PKBField::createConcrete(VAR_NAME{ "b" });
    PKBField field4 = PKBField::createConcrete(PROC_NAME{ "foo" });
    PKBField procDeclaration = PKBField::createDeclaration(PKBEntityType::PROCEDURE);
    PKBField varDeclaration = PKBField::createDeclaration(PKBEntityType::VARIABLE);

    // isRSPRESENT should check both concrete
    pkb->insertRelationship(PKBRelationship::MODIFIES, field1, field2);
    REQUIRE(pkb->isRelationshipPresent(field1, field2, PKBRelationship::MODIFIES));
    REQUIRE(pkb->isRelationshipPresent(
        PKBField::createConcrete(PROC_NAME{ "main" }),
        PKBField::createConcrete(VAR_NAME{ "a" }),
        PKBRelationship::MODIFIES));
    REQUIRE_FALSE(pkb->isRelationshipPresent(field1, field3, PKBRelationship::MODIFIES));
    REQUIRE(*(field3.getContent<VAR_NAME>()) == VAR_NAME{ "b" });
    TEST_LOG << "Test PKB#insertRelationship MODIFIES";

    pkb->insertRelationship(PKBRelationship::MODIFIES, field1, field2);
    //REQUIRE(pkb->modifiesTable->getSize() == 1);


    TEST_LOG << "Test PKB#getRelationship MODIFIES (All concrete fields)";

    // Both fields are concrete
    PKBResponse getRes1 = pkb->getRelationship(field1, field2, PKBRelationship::MODIFIES);
    PKBResponse getExpected1 = PKBResponse{ true, Response{ FieldRowResponse{{field1, field2}} } };
    REQUIRE(getRes1 == getExpected1);

    // Both fields are not concrete
    pkb->insertRelationship(PKBRelationship::MODIFIES, field1, field3);
    pkb->insertRelationship(PKBRelationship::MODIFIES, field4, field2);
    PKBResponse getRes2 = pkb->getRelationship(procDeclaration, varDeclaration, PKBRelationship::MODIFIES);
    PKBResponse getExpected2 = PKBResponse{ true,
        Response{ FieldRowResponse{{field1, field2}, {field1, field3}, {field4, field2}} } };
    REQUIRE(getRes2 == getExpected2);


    // First field concrete, second not concrete
    PKBResponse getRes3 = pkb->getRelationship(field1, varDeclaration, PKBRelationship::MODIFIES);
    PKBResponse getExpected3 = PKBResponse{ true, Response{ FieldRowResponse{{field1, field2}, {field1, field3}} } };
    REQUIRE(getRes3 == getExpected3);

    // First field not concrete, second concrete
    PKBResponse getRes4 = pkb->getRelationship(procDeclaration, field2, PKBRelationship::MODIFIES);
    PKBResponse getExpected4 = PKBResponse{ true, Response{ FieldRowResponse{{field1, field2}, {field4, field2}} } };
    REQUIRE(getRes4 == getExpected4);

    TEST_LOG << "Test PKB#insertVariable";

    std::string varName1 = "a";
    pkb->insertVariable(varName1);
    //REQUIRE(pkb->variableTable->getSize() == 1);

    pkb->insertStatement(StatementType::Assignment, 1);
    pkb->insertStatement(StatementType::While, 2);

    PKBResponse res2 = pkb->getStatements();
    auto content3 = res2.getResponse<FieldResponse>();
    REQUIRE(content3->size() == 2);

    PKBResponse res3 = pkb->getStatements(StatementType::Assignment);
    auto content4 = res3.getResponse<FieldResponse>();
    REQUIRE(content4->size() == 1);
}

TEST_CASE("PKB STMT_LO empty type") {
    std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());
    pkb->insertStatement(StatementType::Assignment, 5);
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 5 });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });

    pkb->insertRelationship(PKBRelationship::MODIFIES, field3, field2);
    PKBResponse res = pkb->getRelationship(field1, field2, PKBRelationship::MODIFIES);
    auto content = res.getResponse<FieldRowResponse>();
    REQUIRE(content->size() == 1);

    PKBResponse res2 = pkb->getRelationship(field3, field2, PKBRelationship::MODIFIES);
    auto content2 = res2.getResponse<FieldRowResponse>();
    REQUIRE(*content == *content2); // checks that getStatementTypeOfConcreteField works
}

TEST_CASE("PKB sample source program test") {
    std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());

    /*
    procedure Example {
     1    x = 2;
     2    z = 3;
     3    i = 5;
     4    read monke;
     5    while (i!=0) {
     6      x = x - 1;
     7      if (x==1) then {
     8          z = x + 1; }
            else {
     9          y = z + x; }
    10      z = z + x + i;
    11      i = i - 1; }
    12    print monke;
    }
    */

    pkb->insertProcedure("Example");
    pkb->insertVariable("x");
    pkb->insertVariable("z");
    pkb->insertVariable("i");
    pkb->insertVariable("y");
    pkb->insertVariable("monke");
    pkb->insertConstant(2);
    pkb->insertConstant(3);
    pkb->insertConstant(5);
    pkb->insertConstant(1);
    pkb->insertStatement(StatementType::Assignment, 1);
    pkb->insertStatement(StatementType::Assignment, 2);
    pkb->insertStatement(StatementType::Assignment, 3);
    pkb->insertStatement(StatementType::Read, 4);
    pkb->insertStatement(StatementType::While, 5);
    pkb->insertStatement(StatementType::Assignment, 6);
    pkb->insertStatement(StatementType::If, 7);
    pkb->insertStatement(StatementType::Assignment, 8);
    pkb->insertStatement(StatementType::Assignment, 9);
    pkb->insertStatement(StatementType::Assignment, 10);
    pkb->insertStatement(StatementType::Assignment, 11);
    pkb->insertStatement(StatementType::Print, 12);

    PKBField const1 = PKBField::createConcrete(CONST{ 1 });
    PKBField const2 = PKBField::createConcrete(CONST{ 2 });
    PKBField const3 = PKBField::createConcrete(CONST{ 3 });
    PKBField const5 = PKBField::createConcrete(CONST{ 5 });
    PKBField stmt1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField stmt2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::Assignment });
    PKBField stmt3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField stmt4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Read });
    PKBField stmt5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::While });
    PKBField stmt6 = PKBField::createConcrete(STMT_LO{ 6, StatementType::Assignment });
    PKBField stmt7 = PKBField::createConcrete(STMT_LO{ 7, StatementType::If });
    PKBField stmt8 = PKBField::createConcrete(STMT_LO{ 8, StatementType::Assignment });
    PKBField stmt9 = PKBField::createConcrete(STMT_LO{ 9, StatementType::Assignment });
    PKBField stmt10 = PKBField::createConcrete(STMT_LO{ 10, StatementType::Assignment });
    PKBField stmt11 = PKBField::createConcrete(STMT_LO{ 11, StatementType::Assignment });
    PKBField stmt12 = PKBField::createConcrete(STMT_LO{ 12, StatementType::Print });
    PKBField proc = PKBField::createConcrete(PROC_NAME{ "Example" });
    PKBField x = PKBField::createConcrete(VAR_NAME{ "x" });
    PKBField y = PKBField::createConcrete(VAR_NAME{ "y" });
    PKBField z = PKBField::createConcrete(VAR_NAME{ "z" });
    PKBField i = PKBField::createConcrete(VAR_NAME{ "i" });
    PKBField monke = PKBField::createConcrete(VAR_NAME{ "monke" });

    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt1, stmt2);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt2, stmt3);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt3, stmt4);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt4, stmt5);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt5, stmt12);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt6, stmt7);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt7, stmt10);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt10, stmt11);

    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt1, x);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt2, z);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt3, i);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt4, monke);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt5, x);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt5, z);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt5, y);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt5, i);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt6, x);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt7, z);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt7, y);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt8, z);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt9, y);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt10, z);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt11, i);
    pkb->insertRelationship(PKBRelationship::MODIFIES, proc, x);
    pkb->insertRelationship(PKBRelationship::MODIFIES, proc, y);
    pkb->insertRelationship(PKBRelationship::MODIFIES, proc, z);
    pkb->insertRelationship(PKBRelationship::MODIFIES, proc, i);
    pkb->insertRelationship(PKBRelationship::MODIFIES, proc, monke);

    pkb->insertRelationship(PKBRelationship::USES, proc, x);
    pkb->insertRelationship(PKBRelationship::USES, proc, z);
    pkb->insertRelationship(PKBRelationship::USES, proc, i);
    pkb->insertRelationship(PKBRelationship::USES, proc, monke);
    pkb->insertRelationship(PKBRelationship::USES, stmt5, i);
    pkb->insertRelationship(PKBRelationship::USES, stmt5, z);
    pkb->insertRelationship(PKBRelationship::USES, stmt5, x);
    pkb->insertRelationship(PKBRelationship::USES, stmt6, x);
    pkb->insertRelationship(PKBRelationship::USES, stmt7, x);
    pkb->insertRelationship(PKBRelationship::USES, stmt7, z);
    pkb->insertRelationship(PKBRelationship::USES, stmt8, x);
    pkb->insertRelationship(PKBRelationship::USES, stmt9, x);
    pkb->insertRelationship(PKBRelationship::USES, stmt9, z);
    pkb->insertRelationship(PKBRelationship::USES, stmt10, x);
    pkb->insertRelationship(PKBRelationship::USES, stmt10, z);
    pkb->insertRelationship(PKBRelationship::USES, stmt10, i);
    pkb->insertRelationship(PKBRelationship::USES, stmt11, i);
    pkb->insertRelationship(PKBRelationship::USES, stmt12, monke);

    pkb->insertRelationship(PKBRelationship::PARENT, stmt5, stmt6);
    pkb->insertRelationship(PKBRelationship::PARENT, stmt5, stmt7);
    pkb->insertRelationship(PKBRelationship::PARENT, stmt7, stmt8);
    pkb->insertRelationship(PKBRelationship::PARENT, stmt7, stmt9);
    pkb->insertRelationship(PKBRelationship::PARENT, stmt5, stmt10);
    pkb->insertRelationship(PKBRelationship::PARENT, stmt5, stmt11);

    SECTION("PKB::getConstants, PKB::getProcedures, PKB::getVariables, PKB::getStatements") {
        REQUIRE(pkb->getConstants() == PKBResponse{ true, FieldResponse{ const1, const2, const3, const5 } });
        REQUIRE(pkb->getProcedures() == PKBResponse{ true, FieldResponse{ proc } });
        REQUIRE(pkb->getVariables() == PKBResponse{ true, FieldResponse{ x, y, z, i, monke } });
        REQUIRE(pkb->getStatements() == PKBResponse{ true, FieldResponse{ stmt1, stmt2, stmt3, stmt4, stmt5, stmt6,
            stmt7, stmt8, stmt9, stmt10, stmt11, stmt12} });
    }

    SECTION("PKB::isRelationshipPresent") {
        REQUIRE(pkb->isRelationshipPresent(stmt1, stmt2, PKBRelationship::FOLLOWS));
        REQUIRE(pkb->isRelationshipPresent(stmt1, stmt3, PKBRelationship::FOLLOWST));

        // Incomplete concrete statement fields
        REQUIRE(pkb->isRelationshipPresent(PKBField::createConcrete(STMT_LO{ 1 }), stmt2,
            PKBRelationship::FOLLOWS));
        REQUIRE(pkb->isRelationshipPresent(PKBField::createConcrete(STMT_LO{ 1 }), stmt3,
            PKBRelationship::FOLLOWST));
        REQUIRE(pkb->isRelationshipPresent(PKBField::createConcrete(STMT_LO{ 1 }),
            PKBField::createConcrete(STMT_LO{ 2 }),
            PKBRelationship::FOLLOWS));

        REQUIRE_FALSE(pkb->isRelationshipPresent(stmt5, stmt6, PKBRelationship::FOLLOWS));
        REQUIRE_FALSE(pkb->isRelationshipPresent(stmt8, stmt9, PKBRelationship::FOLLOWS));
        REQUIRE_FALSE(pkb->isRelationshipPresent(proc, stmt9, PKBRelationship::FOLLOWS));

        // Invalid 
        REQUIRE_FALSE(pkb->isRelationshipPresent(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES));
        REQUIRE_FALSE(pkb->isRelationshipPresent(PKBField::createWildcard(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES));
        REQUIRE_FALSE(pkb->isRelationshipPresent(PKBField::createWildcard(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::FOLLOWS));
    }

    SECTION("PKB::getRelationship") {
        using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

        // Follows(_, _)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWS) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt2, stmt3}, {stmt3, stmt4}, {stmt4, stmt5}, {stmt5, stmt12},
            {stmt6, stmt7}, {stmt7, stmt10}, {stmt10, stmt11} } });

        // Follows*(_, _)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt1, stmt4}, {stmt1, stmt5}, {stmt1, stmt12},
            {stmt2, stmt3}, {stmt2, stmt4}, {stmt2, stmt5}, {stmt2, stmt12}, {stmt3, stmt4}, {stmt3, stmt5},
            {stmt3, stmt12}, {stmt4, stmt5}, {stmt4, stmt12}, {stmt5, stmt12}, {stmt6, stmt7}, {stmt6, stmt10},
            {stmt6, stmt11}, {stmt7, stmt10}, {stmt7, stmt11}, {stmt10, stmt11} } });

        // Follows*(s, _)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt1, stmt4}, {stmt1, stmt5}, {stmt1, stmt12},
            {stmt2, stmt3}, {stmt2, stmt4}, {stmt2, stmt5}, {stmt2, stmt12}, {stmt3, stmt4}, {stmt3, stmt5},
            {stmt3, stmt12}, {stmt4, stmt5}, {stmt4, stmt12}, {stmt5, stmt12}, {stmt6, stmt7}, {stmt6, stmt10},
            {stmt6, stmt11}, {stmt7, stmt10}, {stmt7, stmt11}, {stmt10, stmt11} } });

        // Follows*(_, s)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createDeclaration(StatementType::All), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt1, stmt4}, {stmt1, stmt5}, {stmt1, stmt12},
            {stmt2, stmt3}, {stmt2, stmt4}, {stmt2, stmt5}, {stmt2, stmt12}, {stmt3, stmt4}, {stmt3, stmt5},
            {stmt3, stmt12}, {stmt4, stmt5}, {stmt4, stmt12}, {stmt5, stmt12}, {stmt6, stmt7}, {stmt6, stmt10},
            {stmt6, stmt11}, {stmt7, stmt10}, {stmt7, stmt11}, {stmt10, stmt11} } });

        // Follows*(a, _)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::Assignment),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt1, stmt4}, {stmt1, stmt5}, {stmt1, stmt12},
            {stmt2, stmt3}, {stmt2, stmt4}, {stmt2, stmt5}, {stmt2, stmt12}, {stmt3, stmt4}, {stmt3, stmt5},
            {stmt3, stmt12}, {stmt6, stmt7}, {stmt6, stmt10}, {stmt6, stmt11}, {stmt10, stmt11} } });

        // Follows*(_, a)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createDeclaration(StatementType::Assignment), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt2, stmt3}, {stmt6, stmt10},
            {stmt6, stmt11}, {stmt7, stmt10}, {stmt7, stmt11}, {stmt10, stmt11} } });

        // Follows*(1, _)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment }),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt1, stmt4}, {stmt1, stmt5}, {stmt1, stmt12} } });

        // Follows*(1, _) without statement type
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 1 }),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt1, stmt4}, {stmt1, stmt5}, {stmt1, stmt12} } });

        // Follows*(1, 2) 
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 1 }),
            PKBField::createConcrete(STMT_LO{ 2 }), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2} } });

        // Follows(1, 2) with wrong statement type
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 1, StatementType::While }),
            PKBField::createConcrete(STMT_LO{ 2 }), PKBRelationship::FOLLOWS) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Follows*(1, 2) with wrong statement type
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 1, StatementType::While }),
            PKBField::createConcrete(STMT_LO{ 2 }), PKBRelationship::FOLLOWST) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Follows(p, _) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWS) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Follows(13, _) invalid statement
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 13 }),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWS) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Modifies(_, _) 
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Modifies(_, _) 
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Modifies(p, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {proc, x}, {proc, y}, {proc, z}, {proc, i}, {proc, monke} } });

        // Modifies(w, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::While),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, x}, {stmt5, z}, {stmt5, y}, {stmt5, i} } });

        // Modifies(10, _) 
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 10 }),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt10, z} } });

        // Modifies(10, _) 
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 10, StatementType::All }),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt10, z} } });

        // Modifies(s, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, x}, {stmt2, z}, {stmt3, i}, {stmt4, monke}, {stmt5, x}, {stmt5, z}, {stmt5, y},
            {stmt5, i}, {stmt6, x}, {stmt7, z}, {stmt7, y}, {stmt8, z}, {stmt9, y}, {stmt10, z}, {stmt11, i} } });

        // Modifies(s, v) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, x}, {stmt2, z}, {stmt3, i}, {stmt4, monke}, {stmt5, x}, {stmt5, z}, {stmt5, y},
            {stmt5, i}, {stmt6, x}, {stmt7, z}, {stmt7, y}, {stmt8, z}, {stmt9, y}, {stmt10, z}, {stmt11, i} } });

        // Modifies(s, "z")
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All), z,
            PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt2, z}, {stmt5, z}, {stmt7, z}, {stmt8, z}, {stmt10, z} } });

        // Modifies(a, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::Assignment),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, x}, {stmt2, z}, {stmt3, i}, {stmt6, x}, {stmt8, z},
            {stmt9, y}, {stmt10, z}, {stmt11, i} } });

        // Modifies(r, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::Read),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt4, monke} } });

        // Modifies(_, p) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createDeclaration(PKBEntityType::PROCEDURE), PKBRelationship::MODIFIES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Modifies(v, _) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::VARIABLE),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::MODIFIES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Uses(_, _) 
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Uses(_, _) 
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Uses(p, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {proc, x}, {proc, z}, {proc, i}, {proc, monke} } });

        // Uses(w, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::While),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, i}, {stmt5, x}, {stmt5, z} } });

        // Uses(i, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::If),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt7, x}, {stmt7, z} } });

        // Uses(10, _) 
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 10 }),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt10, z}, {stmt10, x}, {stmt10, i} } });

        // Uses(10, _) 
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 10, StatementType::Assignment }),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt10, z}, {stmt10, x}, {stmt10, i} } });

        // Uses(10, _) 
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 10, StatementType::All }),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt10, z}, {stmt10, x}, {stmt10, i} } });

        // Uses(p, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::Print),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt12, monke} } });

        // Uses(s, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, i}, {stmt5, x}, {stmt5, z}, {stmt6, x}, {stmt7, x}, {stmt7, z}, {stmt8, x},
            {stmt9, z}, {stmt9, x}, {stmt10, z}, {stmt10, x}, {stmt10, i}, {stmt11, i}, {stmt12, monke} } });

        // Uses(s, v) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, i}, {stmt5, x}, {stmt5, z}, {stmt6, x}, {stmt7, x}, {stmt7, z}, {stmt8, x},
            {stmt9, z}, {stmt9, x}, {stmt10, z}, {stmt10, x}, {stmt10, i}, {stmt11, i}, {stmt12, monke} } });

        // Uses(s, "z")
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All), z,
            PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, z}, {stmt7, z}, {stmt9, z}, {stmt10, z} } });

        // Uses(a, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::Assignment),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt6, x}, {stmt8, x}, {stmt9, z}, {stmt9, x}, {stmt10, z}, {stmt10, x},
            {stmt10, i}, {stmt11, i} } });

        // Uses(_, p) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createDeclaration(PKBEntityType::PROCEDURE), PKBRelationship::USES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Uses(v, _) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::VARIABLE),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::USES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Parent(_, _)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt7, stmt8}, {stmt7, stmt9}, {stmt5, stmt10},
            {stmt5, stmt11}} });

        // Parent(s, s1)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(StatementType::All), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt7, stmt8}, {stmt7, stmt9}, {stmt5, stmt10},
            {stmt5, stmt11}} });

        // Parent(5, s)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5, StatementType::All }),
            PKBField::createDeclaration(StatementType::All), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent(5, s)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5, StatementType::While }),
            PKBField::createDeclaration(StatementType::All), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent(5, s)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5 }),
            PKBField::createDeclaration(StatementType::All), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent(5, r)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5 }),
            PKBField::createDeclaration(StatementType::Read), PKBRelationship::PARENT) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Parent(s, 6)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createConcrete(STMT_LO{ 6 }), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6} } });

        // Parent(s, 6)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createConcrete(STMT_LO{ 6, StatementType::All }), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6} } });

        // Parent(s, 6)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createConcrete(STMT_LO{ 6, StatementType::Assignment }), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6} } });

        // Parent(5, 6)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5 }),
            PKBField::createConcrete(STMT_LO{ 6 }), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6} } });

        // Parent*(_, _)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::PARENTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt5, stmt8}, {stmt5, stmt9}, {stmt7, stmt8},
            {stmt7, stmt9}, {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent*(5, _)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5 }),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::PARENTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt5, stmt8}, {stmt5, stmt9},
            {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent*(5, a)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5 }),
            PKBField::createDeclaration(StatementType::Assignment), PKBRelationship::PARENTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt8}, {stmt5, stmt9},
            {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent*(5, a)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5, StatementType::All }),
            PKBField::createDeclaration(StatementType::Assignment), PKBRelationship::PARENTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt8}, {stmt5, stmt9},
            {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent*(5, i)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5, StatementType::All }),
            PKBField::createDeclaration(StatementType::If), PKBRelationship::PARENTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt7} } });

        // Parent*(5, 5)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5, StatementType::All }),
            PKBField::createConcrete(STMT_LO{ 5, StatementType::All }), PKBRelationship::PARENTT) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Parent*(5, 8)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5, StatementType::All }),
            PKBField::createConcrete(STMT_LO{ 8, StatementType::All }), PKBRelationship::PARENTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt8} } });

        // Parents*(p, _) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::PARENTT) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Parents*(_, v) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createDeclaration(PKBEntityType::VARIABLE), PKBRelationship::PARENTT) == PKBResponse{ false,
            FieldRowResponse{ } });
    }
}

TEST_CASE("PKB regression test") {
    SECTION("PKB regression test #140.1") {
        std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());
        pkb->getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE),
            PKBRelationship::USES
        );
    }
    SECTION("PKB regression test #140.2") {
        std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());
        pkb->insertRelationship(PKBRelationship::USES,
            PKBField::createConcrete(STMT_LO{ 1, StatementType::Print }),
            PKBField::createConcrete(VAR_NAME("x"))
        );
        pkb->getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE),
            PKBRelationship::USES
        );
    }

    SECTION("PKB regression test #142.1") {
        std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());
        pkb->insertRelationship(PKBRelationship::USES,
            PKBField::createConcrete(STMT_LO{ 1, StatementType::Print }),
            PKBField::createConcrete(VAR_NAME("x"))
        );
        pkb->insertRelationship(PKBRelationship::USES,
            PKBField::createConcrete(STMT_LO{ 2, StatementType::Assignment }),
            PKBField::createConcrete(VAR_NAME("y"))
        );
        pkb->insertRelationship(PKBRelationship::USES,
            PKBField::createConcrete(PROC_NAME{ "main" }),
            PKBField::createConcrete(VAR_NAME("y"))
        );
        REQUIRE_NOTHROW(pkb->getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE),
            PKBRelationship::USES
        ));
    }

    SECTION("PKB regression test #142.2") {
        std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());
        pkb->insertRelationship(PKBRelationship::MODIFIES,
            PKBField::createConcrete(STMT_LO{ 1, StatementType::Print }),
            PKBField::createConcrete(VAR_NAME("x"))
        );
        pkb->insertRelationship(PKBRelationship::MODIFIES,
            PKBField::createConcrete(STMT_LO{ 2, StatementType::Assignment }),
            PKBField::createConcrete(VAR_NAME("y"))
        );
        pkb->insertRelationship(PKBRelationship::MODIFIES,
            PKBField::createConcrete(PROC_NAME{ "main" }),
            PKBField::createConcrete(VAR_NAME("y"))
        );
        REQUIRE_NOTHROW(pkb->getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE),
            PKBRelationship::MODIFIES
        ));
    }

    SECTION("PKB regression test #148") {
        std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());
        pkb->insertStatement(StatementType::Assignment, 2);
        PKBField field1 = PKBField::createConcrete(STMT_LO{ 2, StatementType::Assignment });
        PKBField field2 = PKBField::createConcrete(STMT_LO{ 2 });
        PKBField field3 = PKBField::createConcrete(VAR_NAME{ "a" });
        pkb->insertRelationship(PKBRelationship::MODIFIES, field2, field3);
        REQUIRE_FALSE(pkb->isRelationshipPresent(field2, field3, PKBRelationship::MODIFIES));

        pkb->insertRelationship(PKBRelationship::MODIFIES, field1, field3);
        REQUIRE(pkb->isRelationshipPresent(field1, field3, PKBRelationship::MODIFIES));
        REQUIRE(pkb->isRelationshipPresent(field2, field3, PKBRelationship::MODIFIES));

        // statement not in the statement table and STMT_LO initialised without a type
        PKBField field4 = PKBField::createConcrete(STMT_LO{ 3 });
        REQUIRE_FALSE(pkb->isRelationshipPresent(field4, field3, PKBRelationship::MODIFIES));
    }
}
