#include <iostream>
#include <unordered_set>

#include "logging.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

TEST_CASE("STMT_LO equality") {
    STMT_LO s1 = STMT_LO{ 1, StatementType::Call, "abc" };
    STMT_LO s2 = STMT_LO{ 1, StatementType::Call, "abc" };
    STMT_LO s3 = STMT_LO{ 1, StatementType::Call, "def" };
    STMT_LO s4 = STMT_LO{ 1, StatementType::Call };
    STMT_LO s5 = STMT_LO{ 1, StatementType::Assignment };
    STMT_LO s6 = STMT_LO{ 1, StatementType::Call, "abc" };

    REQUIRE(s1 == s2);
    REQUIRE_FALSE(s1 == s3);
    REQUIRE_FALSE(s1 == s4);
    REQUIRE_FALSE(s1 == s5);
}

TEST_CASE("STMT_LO set equality") {
    // sets uses < for equality intead of ==. Therefore this test is added.
    STMT_LO s1 = STMT_LO{ 1, StatementType::Call, "" };
    STMT_LO s2 = STMT_LO{ 1, StatementType::Call};
    STMT_LO s3 = STMT_LO{ 1, StatementType::Call, "a"};
    STMT_LO s4 = STMT_LO{ 1, StatementType::Call, "a"};
    REQUIRE_FALSE((s1 < s2) & (s2 < s1));
    REQUIRE_FALSE((s1 < s3) & (s3 < s1));
    REQUIRE((!(s3 < s4) & !(s4 < s3)) & (s4 == s3));
}

TEST_CASE("PKBField createConcrete statement, getContent") {
    PKBField field = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    REQUIRE(*(field.getContent<STMT_LO>()) == STMT_LO{ 1, StatementType::Assignment });
}

TEST_CASE("PKBField createConcrete variable, getContent") {
    PKBField field = PKBField::createConcrete(VAR_NAME{ "a" });
    REQUIRE(*(field.getContent<VAR_NAME>()) == VAR_NAME{ "a" });
}

TEST_CASE("PKBField createConcrete procedure, getContent") {
    PKBField field = PKBField::createConcrete(PROC_NAME{ "main" });
    REQUIRE(*(field.getContent<PROC_NAME>()) == PROC_NAME{ "main" });
}

TEST_CASE("PKBField createConcrete constant, getContent") {
    PKBField field = PKBField::createConcrete(CONST{ 1 });
    REQUIRE(*(field.getContent<CONST>()) == CONST{ 1 });
}

TEST_CASE("PKBField createWildCard") {
    PKBField field = PKBField::createWildcard(PKBEntityType::PROCEDURE);
    REQUIRE(field.fieldType == PKBFieldType::WILDCARD);
    REQUIRE(field.entityType == PKBEntityType::PROCEDURE);
    REQUIRE_THROWS_AS(field.statementType.value(), std::bad_optional_access);
    REQUIRE(field.getContent<PROC_NAME>() == nullptr);  // content is uninitialized
}

TEST_CASE("PKBField createDeclaration statement") {
    REQUIRE_THROWS_AS(PKBField::createDeclaration(PKBEntityType::STATEMENT), std::invalid_argument);

    PKBField field = PKBField::createDeclaration(StatementType::Assignment);
    REQUIRE(field.fieldType == PKBFieldType::DECLARATION);
    REQUIRE(field.entityType == PKBEntityType::STATEMENT);
    REQUIRE(field.statementType.value() == StatementType::Assignment);
    REQUIRE(field.getContent<PROC_NAME>() == nullptr);  // content is uninitialized
}

TEST_CASE("PKBField createDeclaration procedure") {
    PKBField field = PKBField::createDeclaration(PKBEntityType::PROCEDURE);
    REQUIRE(field.fieldType == PKBFieldType::DECLARATION);
    REQUIRE(field.entityType == PKBEntityType::PROCEDURE);
    REQUIRE_THROWS_AS(field.statementType.value(), std::bad_optional_access);
    REQUIRE(field.getContent<PROC_NAME>() == nullptr);  // content is uninitialized
}

TEST_CASE("PKBField createDeclaration variable") {
    PKBField field = PKBField::createDeclaration(PKBEntityType::VARIABLE);
    REQUIRE(field.fieldType == PKBFieldType::DECLARATION);
    REQUIRE(field.entityType == PKBEntityType::VARIABLE);
    REQUIRE_THROWS_AS(field.statementType.value(), std::bad_optional_access);
    REQUIRE(field.getContent<VAR_NAME>() == nullptr);  // content is uninitialized
}

TEST_CASE("PKBField createDeclaration constant") {
    PKBField field = PKBField::createDeclaration(PKBEntityType::CONST);
    REQUIRE(field.fieldType == PKBFieldType::DECLARATION);
    REQUIRE(field.entityType == PKBEntityType::CONST);
    REQUIRE_THROWS_AS(field.statementType.value(), std::bad_optional_access);
    REQUIRE(field.getContent<CONST>() == nullptr);  // content is uninitialized
}

TEST_CASE("PKBField isValidConcrete") {
    PKBField field = PKBField::createDeclaration(PKBEntityType::CONST);
    REQUIRE_FALSE(field.isValidConcrete(PKBEntityType::CONST));

    field = PKBField::createWildcard(PKBEntityType::CONST);
    REQUIRE_FALSE(field.isValidConcrete(PKBEntityType::CONST));

    field = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    REQUIRE(field.isValidConcrete(PKBEntityType::STATEMENT));

    field = PKBField::createConcrete(STMT_LO{ 1, StatementType::All });
    REQUIRE_FALSE(field.isValidConcrete(PKBEntityType::STATEMENT));

    field = PKBField::createConcrete(STMT_LO{ 1 });
    REQUIRE_FALSE(field.isValidConcrete(PKBEntityType::STATEMENT));
}

TEST_CASE("Hashing Test") {
    std::hash<STMT_LO> stmtLOHasher;
    std::hash<CONST> constHasher;
    std::hash<VAR_NAME> varNameHasher;
    std::hash<PROC_NAME> procNameHasher;
    
    // STMT_LO
    STMT_LO s1 = STMT_LO{ 1, StatementType::Call};
    STMT_LO s2 = STMT_LO{ 1, StatementType::Call};
    STMT_LO s3 = STMT_LO{ 2, StatementType::Call};
    REQUIRE(stmtLOHasher(s1) == stmtLOHasher(s1));
    REQUIRE(stmtLOHasher(s1) == stmtLOHasher(s2));
    REQUIRE_FALSE(stmtLOHasher(s1) == stmtLOHasher(s3));
    
    // Hashing CONST is just std::hash<int> and was not overloaded
    // Ensure no collision between hashes of the same statement number and const values
    CONST test = 1;
    REQUIRE_FALSE(constHasher(test) == stmtLOHasher(s1));
    
    // VAR_NAME
    VAR_NAME var1 = VAR_NAME("monke");
    VAR_NAME var2 = VAR_NAME("monke");
    VAR_NAME var3 = VAR_NAME("ape");
    REQUIRE(varNameHasher(var1) == varNameHasher(var1));
    REQUIRE(varNameHasher(var1) == varNameHasher(var2));
    REQUIRE_FALSE(varNameHasher(var1) == varNameHasher(var3));
    
    // PROC_NAME
    PROC_NAME proc1 = PROC_NAME("monke");
    PROC_NAME proc2 = PROC_NAME("monke");
    PROC_NAME proc3 = PROC_NAME("ape");
    REQUIRE(procNameHasher(proc1) == procNameHasher(proc1));
    REQUIRE(procNameHasher(proc1) == procNameHasher(proc2));
    REQUIRE_FALSE(procNameHasher(proc1) == procNameHasher(proc3));
    // Ensure no collision between hashes of the same VAR_NAME and PROC_NAME
    REQUIRE_FALSE(procNameHasher(proc1) == varNameHasher(var1));

    // Testing each of them again inside an unordered_set (hashtable)
    std::unordered_set<STMT_LO> stmtSet;
    stmtSet.insert(s1);
    REQUIRE(stmtSet.find(s1) != stmtSet.end());
    REQUIRE(stmtSet.find(s2) != stmtSet.end());
    REQUIRE(stmtSet.find(s3) == stmtSet.end());   
    
    std::unordered_set<VAR_NAME> varSet;
    varSet.insert(var1);
    REQUIRE(varSet.find(var1) != varSet.end());
    REQUIRE(varSet.find(var2) != varSet.end());
    REQUIRE(varSet.find(var3) == varSet.end());   
    
    std::unordered_set<PROC_NAME> procSet;
    procSet.insert(proc1);
    REQUIRE(procSet.find(proc1) != procSet.end());
    REQUIRE(procSet.find(proc2) != procSet.end());
    REQUIRE(procSet.find(proc3) == procSet.end()); 
}
