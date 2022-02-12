#include <iostream>

#include "logging.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

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
