#include "PKB/PKBResponse.h"
#include "logging.h"
#include <iostream>
#include "PKB/PKBField.h"

#include "catch.hpp"

#define TEST_LOG Logger() << "TestPKBResponse.cpp"

TEST_CASE("PKBResponse == Testing") {
    // Initialization of PKBfields and PKBResponses

    // Statement fields
    PKBField stmt1 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField stmt2 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });


    // Var fields
    PKBField var1 = PKBField::createConcrete(VAR_NAME{"a"});

    // Empty PKBResponses
    PKBResponse empty1{ false, Response{} };
    PKBResponse empty2{ false, Response{} };

    // PKBResponse that contain a FieldResponse
    PKBResponse fieldRes1{ true, Response{ FieldResponse{stmt1, var1} } };
    PKBResponse fieldRes2{ true, Response{ FieldResponse{stmt1, var1} } };
    PKBResponse fieldRes3{ true, Response{ FieldResponse{stmt2, var1} } };

    // PKBResponse that contain a FieldRowResponse
    PKBResponse fieldRow1{ 
        true, 
        Response{ 
            FieldRowResponse{ 
                { {stmt1, var1}, {stmt2, var1} }
            } 
        } 
    };

    PKBResponse fieldRow2{
        true,
        Response{
            FieldRowResponse{
                { {stmt1, var1}, {stmt2, var1} }
            }
        }
    };

    PKBResponse fieldRow3{
        true,
        Response{
            FieldRowResponse{
                { {stmt1, var1} }
            }
        }
    };

    PKBResponse fieldRow4{
        true,
        Response{
            FieldRowResponse{
                { {stmt1, var1} }
            }
        }
    };


    TEST_LOG << "Test equality of empty PKBResponse";
    
    REQUIRE(empty1 == empty2);
    REQUIRE(empty1 == empty1);


    TEST_LOG << "Test equality of PKBResponse containing a FieldResponse";

    REQUIRE(fieldRes1 == fieldRes2);
    REQUIRE_FALSE(fieldRes1 == fieldRes3);
    REQUIRE_FALSE(fieldRes1 == empty1);

    TEST_LOG << "Test equality of PKBResponse containing a FieldRowResponse";

    REQUIRE(fieldRow1 == fieldRow2);
    REQUIRE(fieldRow1 == fieldRow1);
    REQUIRE(fieldRow3 == fieldRow4);
    REQUIRE_FALSE(fieldRow2 == fieldRow3);
}
