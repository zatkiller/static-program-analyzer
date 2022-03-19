
#include <string>

#include "logging.h"
#include "catch.hpp"
#include "SourceProcessor.h"

#define TEST_LOG Logger() << "TestSP.cpp "

/**
 * @brief This test is to ensure SP will not crash on parsing and design extraction.
 */
TEST_CASE("SP Stability test") {
    SourceProcessor sp;
    PKB tempPKB;

    SECTION("Simple source 1") {
        std::string source1 = R"(
            procedure sumDigits {
                read number;
                sum = 0;

                while (number > 0) {
                    digit = number % 10;
                    sum = sum + digit;
                    number = number / 10;
                }

                print sum;
            }
        )";

        sp.processSimple(source1, &tempPKB);
        SUCCEED("pass source 1");
    }
    
    SECTION("Multiprocedure complex source 1") {
        std::string source2 = R"(
            procedure main {
                while (x > 1) {
                    call p1;
                }
                if (x < 1) then {
                    y = x + 1;
                    call p3;
                } else {
                    x = y + 1;
                    while (z == y) {
                        read z;
                        print y;
                    }
                }
            }

            procedure p1 {
                read y1;
                print x1;
                call p2;
            }

            procedure p2 {
                call p3;
                y2 = z2 + 1;
                while (x2 < 1) {
                    call p3;
                }
            }

            procedure p3 {
                read x3;
            }
        )";
        sp.processSimple(source2, &tempPKB);
        SUCCEED("pass source 2");
    }
}
