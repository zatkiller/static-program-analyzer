#include <set>
#include <string>
#include <vector>
#include <variant>
#include "catch.hpp"

#include "SourceProcessor.h"
#include "PKB.h"
#include "logging.h"

#define TEST_LOG Logger() << "TestParseAndStore.cpp "

// helper type for the visitor #4
template<typename... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<typename... Ts> overloaded(Ts...)->overloaded<Ts...>;

std::variant<int, std::string> extractField(PKBField v) {
    if (!v.isConcrete) {
        throw std::invalid_argument("No value to extract from PKBField, not concrete");
    }

    switch (v.tag) {
    case PKBType::VARIABLE: return std::get<VAR_NAME>(v.content).name;
    case PKBType::STATEMENT: return std::get<STMT_LO>(v.content).statementNum;
    case PKBType::PROCEDURE: return std::get<PROC_NAME>(v.content).name;
    case PKBType::CONST: return std::get<CONST>(v.content);
    }

}


TEST_CASE("Test parse and store") {
    
    PKB pkb;
    SourceProcessor sp;
    sp.processSimple(R"(
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
    )", &pkb);


    TEST_LOG << "Variable extraction from PKB";
    {
        auto response = pkb.getVariables();
        REQUIRE(response.hasResult);

        auto resultSet = std::get<std::unordered_set<PKBField, PKBFieldHash>>(response.res);

        std::set<std::string> vars;
        for (auto v : resultSet) {
            vars.insert(std::get<std::string>(extractField(v)));
        }
        std::set<std::string> expected = { "number", "sum", "digit" };

        REQUIRE(vars == expected);
    }

    TEST_LOG << "Modifies extraction from PKB";
    {
        using Rows = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

        TEST_LOG << "Statement modifies extraction from PKB";
        {
            // 3rd param in PKBField construction is unused in getRelationship.
            auto response = pkb.getRelationship(
                PKBField{ PKBType::STATEMENT, false, STMT_LO{1} },
                PKBField{ PKBType::VARIABLE, false, VAR_NAME{"a"} },
                PKBRelationship::MODIFIES);
            REQUIRE(response.hasResult);
            auto resultSet = std::get<Rows>(response.res);

            std::set<std::pair<int, std::string>> stmtModifies;
            for (auto& row : resultSet) {
                auto stmt = std::get<int>(extractField(row[0]));
                auto var = std::get<std::string>(extractField(row[1]));
                stmtModifies.insert(std::make_pair<>(stmt, var));
            }

            auto p = [](int s, std::string v) {
                return std::make_pair<>(s, v);
            };

            std::set<std::pair<int, std::string>> expected = {
                p(1, "number"),
                p(2, "sum"),
                p(3, "digit"),
                p(3, "sum"),
                p(3, "number"),
                p(4, "digit"),
                p(5, "sum"),
                p(6, "number"),
            };
            REQUIRE(expected == stmtModifies);
        }
        TEST_LOG << "Procedure modifies extraction from PKB";
        {
            // 3rd param in PKBField construction is unused in getRelationship.
            auto response = pkb.getRelationship(
                PKBField{ PKBType::PROCEDURE, false, PROC_NAME{"a"}},
                PKBField{ PKBType::VARIABLE, false, VAR_NAME{"a"} },
                PKBRelationship::MODIFIES);
            REQUIRE(response.hasResult);
            auto resultSet = std::get<Rows>(response.res);

            std::set<std::pair<std::string, std::string>> stmtModifies;
            for (auto& row : resultSet) {
                auto proc = std::get<std::string>(extractField(row[0]));
                auto var = std::get<std::string>(extractField(row[1]));
                stmtModifies.insert(std::make_pair<>(proc, var));
            }

            auto p = [](std::string p, std::string v) {
                return std::make_pair<>(p, v);
            };

            std::set<std::pair<std::string, std::string>> expected = {
                p("sumDigits", "number"),
                p("sumDigits", "sum"),
                p("sumDigits", "digit")
            };
            REQUIRE(expected == stmtModifies);
        }
    }

    TEST_LOG << "Statement extraction from PKB"; 
    {
        auto response = pkb.getStatements();
        REQUIRE(response.hasResult);
        auto resultSet = std::get<std::unordered_set<PKBField, PKBFieldHash>>(response.res);

        std::set<STMT_LO> statements;
        for (auto v : resultSet) {
            statements.insert(std::get<STMT_LO>(v.content));
        }
        std::set<STMT_LO> expected = {
            STMT_LO{1, StatementType::Read},
            STMT_LO{2, StatementType::Assignment},
            STMT_LO{3, StatementType::While},
            STMT_LO{4, StatementType::Assignment},
            STMT_LO{5, StatementType::Assignment},
            STMT_LO{6, StatementType::Assignment},
            STMT_LO{7, StatementType::Print}
        };
        REQUIRE(statements == expected);
    }

}

