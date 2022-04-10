#include "PKB/PKBResponse.h"
#include "logging.h"
#include <iostream>
#include "PKB/PKBField.h"
#include "catch.hpp"

TEST_CASE("PKBResponse getResponse uninitialized") {
    PKBResponse response{ false, Response{} };
    REQUIRE(response.getResponse<FieldResponse>() == nullptr);
}

TEST_CASE("PKBResponse getResponse empty") {
    PKBResponse response{ false, Response{FieldResponse{}} };
    REQUIRE(response.getResponse<FieldResponse>() == nullptr);
}

TEST_CASE("PKBResponse getResponse single") {
    PKBField stmt1 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBResponse response{ true, Response{FieldResponse{stmt1}} };

    REQUIRE(*(response.getResponse<FieldResponse>()) == FieldResponse{ stmt1 });
}

TEST_CASE("PKBResponse getResponse multiple") {
    PKBField stmt1 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField stmt2 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });
    PKBResponse response{ true, Response{FieldResponse{stmt1, stmt2}} };

    REQUIRE(*(response.getResponse<FieldResponse>()) == FieldResponse{ stmt1, stmt2 });
}

TEST_CASE("PKBResponse == empty") {
    PKBResponse response1{ false, Response{} };
    PKBResponse response2{ false, Response{FieldResponse{}} };
    REQUIRE(response1 == response1);
    REQUIRE(response2 == response2);
    REQUIRE(response1 == response2);
}

TEST_CASE("PKBResponse ==") {
    PKBField stmt1 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField stmt2 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });
    PKBField var1 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBResponse response1{ true, Response{ FieldResponse{stmt1, var1} } };
    PKBResponse response2{ true, Response{ FieldResponse{stmt1, var1} } };
    PKBResponse response3{ true, Response{ FieldResponse{stmt2, var1} } };
    REQUIRE(response1 == response2);
    REQUIRE_FALSE(response1 == response3);

    REQUIRE_FALSE(response1 == PKBResponse{ false, Response{} });
    REQUIRE_FALSE(response1 == PKBResponse{ true, Response{FieldRowResponse{std::vector<PKBField>{stmt1, var1}}} });
}
