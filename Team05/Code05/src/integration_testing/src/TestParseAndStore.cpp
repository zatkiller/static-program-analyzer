#include <set>
#include <string>
#include <vector>
#include <variant>
#include "catch.hpp"

#include "SourceProcessor.h"
#include "PKB.h"
#include "logging.h"

#define TEST_LOG Logger() << "TestParseAndStore.cpp "

auto p = [](auto first, auto second) {
    return std::make_pair<>(first, second);
};

struct DesignExtractionTestTemplate {
    std::map<PKBEntityType, std::set<Content>> expectedEntities;
    std::map<PKBRelationship, std::set<std::pair<Content, Content>>> expectedRelationships;

    template <typename T>
    bool isCorrect(std::set<T> result, PKBEntityType targetType) {
        auto targetSet = expectedEntities[targetType];
        std::set<T> expected;
        std::transform(targetSet.begin(), targetSet.end(),
            std::inserter(expected, expected.begin()),
            [](auto v) {
                return std::get<T>(v);
            }
        );
        return result == expected;
    }

    template <typename T, typename K>
    bool isCorrect(std::set<std::pair<T, K>> result, PKBRelationship targetType) {
        auto targetSet = expectedRelationships[targetType];
        std::set<std::pair<T, K>> expected;
        // transform and insert
        for (auto e : targetSet) {
            try {
                expected.insert(p(std::get<T>(e.first), std::get<K>(e.second)));
            } catch (std::bad_variant_access ex) {
                // ignore 
                continue;
            }
        }

        return result == expected;
    }



};

struct TestParseAndStorePackage1 : public DesignExtractionTestTemplate {

    std::string sourceCode = R"(
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

    TestParseAndStorePackage1() {
        initEntities();
        initRelationships();
    }

    void initEntities() {
        expectedEntities.emplace(
            PKBEntityType::VARIABLE,
            std::set<Content>({
                VAR_NAME{"number"},
                VAR_NAME{"sum"},
                VAR_NAME{"digit"}
            })
        );
        expectedEntities.emplace(
            PKBEntityType::STATEMENT,
            std::set<Content>({
                STMT_LO{1, StatementType::Read, "number"},
                STMT_LO{2, StatementType::Assignment},
                STMT_LO{3, StatementType::While},
                STMT_LO{4, StatementType::Assignment},
                STMT_LO{5, StatementType::Assignment},
                STMT_LO{6, StatementType::Assignment},
                STMT_LO{7, StatementType::Print, "sum"}
            })
        );
        expectedEntities.emplace(
            PKBEntityType::CONST,
            std::set<Content>({
                CONST{0},
                CONST{10},
            })
        );
        expectedEntities.emplace(
            PKBEntityType::PROCEDURE,
            std::set<Content>({
                PROC_NAME{"sumDigits"},
            })
        );
    }
    void initRelationships() {
        expectedRelationships.emplace(
            PKBRelationship::MODIFIES,
            std::set<std::pair<Content, Content>> {
                p(STMT_LO{ 1, StatementType::Read, "number" }, VAR_NAME{ "number" }),
                p(STMT_LO{ 2, StatementType::Assignment }, VAR_NAME{ "sum" }),
                p(STMT_LO{ 3, StatementType::While }, VAR_NAME{ "digit" }),
                p(STMT_LO{ 3, StatementType::While }, VAR_NAME{ "sum" }),
                p(STMT_LO{ 3, StatementType::While }, VAR_NAME{ "number" }),
                p(STMT_LO{ 4, StatementType::Assignment }, VAR_NAME{ "digit" }),
                p(STMT_LO{ 5, StatementType::Assignment }, VAR_NAME{ "sum" }),
                p(STMT_LO{ 6, StatementType::Assignment }, VAR_NAME{ "number" }),
                p(PROC_NAME{ "sumDigits" }, VAR_NAME{ "number" }),
                p(PROC_NAME{ "sumDigits" }, VAR_NAME{ "digit" }),
                p(PROC_NAME{ "sumDigits" }, VAR_NAME{ "sum" })
            }
        );

        expectedRelationships.emplace(
            PKBRelationship::USES,
            std::set<std::pair<Content, Content>> {
            p(PROC_NAME{ "sumDigits" }, VAR_NAME{ "number" }),
                p(PROC_NAME{ "sumDigits" }, VAR_NAME{ "digit" }),
                p(PROC_NAME{ "sumDigits" }, VAR_NAME{ "sum" }),
                p(STMT_LO{ 3, StatementType::While }, VAR_NAME{ "number" }),
                p(STMT_LO{ 3, StatementType::While }, VAR_NAME{ "sum" }),
                p(STMT_LO{ 3, StatementType::While }, VAR_NAME{ "digit" }),
                p(STMT_LO{ 4, StatementType::Assignment }, VAR_NAME{ "number" }),
                p(STMT_LO{ 5, StatementType::Assignment }, VAR_NAME{ "sum" }),
                p(STMT_LO{ 5, StatementType::Assignment }, VAR_NAME{ "digit" }),
                p(STMT_LO{ 6, StatementType::Assignment }, VAR_NAME{ "number" }),
                p(STMT_LO{ 7, StatementType::Print, "sum"}, VAR_NAME{ "sum" }),
            }
        );

        expectedRelationships.emplace(
            PKBRelationship::FOLLOWS,
            std::set<std::pair<Content, Content>> {
                p(STMT_LO{ 1, StatementType::Read, "number" }, STMT_LO{ 2, StatementType::Assignment }),
                p(STMT_LO{ 2, StatementType::Assignment }, STMT_LO{ 3, StatementType::While }),
                p(STMT_LO{ 3, StatementType::While }, STMT_LO{ 7, StatementType::Print, "sum" }),
                p(STMT_LO{ 4, StatementType::Assignment }, STMT_LO{ 5, StatementType::Assignment }),
                p(STMT_LO{ 5, StatementType::Assignment }, STMT_LO{ 6, StatementType::Assignment }),
            }
        );

        expectedRelationships.emplace(
            PKBRelationship::PARENT,
            std::set<std::pair<Content, Content>> {
                p(STMT_LO{ 3, StatementType::While }, STMT_LO{ 4, StatementType::Assignment }),
                p(STMT_LO{ 3, StatementType::While }, STMT_LO{ 5, StatementType::Assignment }),
                p(STMT_LO{ 3, StatementType::While }, STMT_LO{ 6, StatementType::Assignment }),
            }
        );
    }
};

struct TestParseAndStorePackage2 : public DesignExtractionTestTemplate {

    std::string sourceCode = R"(
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

    TestParseAndStorePackage2() {
        initEntities();
        initRelationships();
    }

    void initEntities() {
    }
    void initRelationships() {

        
        auto s1 = STMT_LO{1, StatementType::While};
        auto s2 = STMT_LO{2, StatementType::Call, "p1"};
        auto s3 = STMT_LO{3, StatementType::If};
        auto s4 = STMT_LO{4, StatementType::Assignment};
        auto s5 = STMT_LO{5, StatementType::Call, "p3"};
        auto s6 = STMT_LO{6, StatementType::Assignment};
        auto s7 = STMT_LO{7, StatementType::While};
        auto s8 = STMT_LO{8, StatementType::Read, "z"};
        auto s9 = STMT_LO{9, StatementType::Print, "y"};
        auto s10 = STMT_LO{10, StatementType::Read, "y1"};
        auto s11 = STMT_LO{11, StatementType::Print, "x1"};
        auto s12 = STMT_LO{12, StatementType::Call, "p2"};
        auto s13 = STMT_LO{13, StatementType::Call, "p3"};
        auto s14 = STMT_LO{14, StatementType::Assignment};
        auto s15 = STMT_LO{15, StatementType::While};
        auto s16 = STMT_LO{16, StatementType::Call, "p3"};
        auto s17 = STMT_LO{17, StatementType::Read, "x3"};

        expectedRelationships.emplace(
            PKBRelationship::CALLS,
            std::set<std::pair<Content, Content>> {
                p(PROC_NAME{"main"}, PROC_NAME{"p1"}),
                p(PROC_NAME{"main"}, PROC_NAME{"p3"}),
                p(PROC_NAME{"p1"}, PROC_NAME{"p2"}),
                p(PROC_NAME{"p2"}, PROC_NAME{"p3"}),
            }
        );
        expectedRelationships.emplace(
            PKBRelationship::NEXT,
            std::set<std::pair<Content, Content>> {
                p(s1, s2),
                p(s2, s1),
                p(s1, s3),
                p(s3, s4),
                p(s4, s5),
                p(s3, s6),
                p(s6, s7),
                p(s7, s8),
                p(s8, s9),
                p(s9, s7),
                p(s10, s11),
                p(s11, s12),
                p(s13, s14),
                p(s14, s15),
                p(s15, s16),
                p(s16, s15),
            }
        );
    }
};

TEST_CASE("Test parse and store for basic package 1") {
    PKB pkb;
    SourceProcessor sp;
    TestParseAndStorePackage1 test1;
    sp.processSimple(test1.sourceCode, &pkb);

    auto row = [](auto a, auto b) {
        return std::vector<PKBField>({
            PKBField::createConcrete(a),
            PKBField::createConcrete(b),
        });
    };

    auto expectedResponse = [](auto rows) {
        return PKBResponse{ true, rows };
    };

    using Rows = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;


    REQUIRE(pkb.match(StatementType::Assignment, std::nullopt, "digit") ==
        expectedResponse(FieldRowResponse{
            row(STMT_LO{ 5, StatementType::Assignment }, VAR_NAME{"sum"})}));

    REQUIRE(pkb.match(StatementType::Assignment, "sum", std::nullopt) ==
        expectedResponse(FieldRowResponse{
            row(STMT_LO{ 5, StatementType::Assignment }, VAR_NAME{"sum"}),
            row(STMT_LO{ 2, StatementType::Assignment }, VAR_NAME{"sum"})}));

    REQUIRE(pkb.match(StatementType::Assignment, std::nullopt, std::nullopt) ==
        expectedResponse(FieldRowResponse{
            row(STMT_LO{ 5, StatementType::Assignment }, VAR_NAME{"sum"}),
            row(STMT_LO{ 2, StatementType::Assignment }, VAR_NAME{"sum"}),
            row(STMT_LO{ 4, StatementType::Assignment }, VAR_NAME{"digit"}),
            row(STMT_LO{ 6, StatementType::Assignment }, VAR_NAME{"number"})}));


    TEST_LOG << "Variable extraction from PKB";
    {
        auto response = pkb.getVariables();
        REQUIRE(response.hasResult);

        auto resultSet = std::get<std::unordered_set<PKBField, PKBFieldHash>>(response.res);

        std::set<VAR_NAME> vars;
        for (auto v : resultSet) {
            vars.insert(std::get<VAR_NAME>(v.content));
        }

        REQUIRE(test1.isCorrect(vars, PKBEntityType::VARIABLE));
    }


    TEST_LOG << "Const extraction from PKB";
    {
        auto response = pkb.getConstants();
        REQUIRE(response.hasResult);

        auto resultSet = std::get<std::unordered_set<PKBField, PKBFieldHash>>(response.res);

        std::set<CONST> consts;
        for (auto v : resultSet) {
            consts.insert(std::get<CONST>(v.content));
        }

        REQUIRE(test1.isCorrect(consts, PKBEntityType::CONST));
    }

    TEST_LOG << "Procedure extraction from PKB";
    {
        auto response = pkb.getProcedures();
        REQUIRE(response.hasResult);

        auto resultSet = std::get<std::unordered_set<PKBField, PKBFieldHash>>(response.res);

        std::set<PROC_NAME> procedures;
        for (auto v : resultSet) {
            procedures.insert(std::get<PROC_NAME>(v.content));
        }

        REQUIRE(test1.isCorrect(procedures, PKBEntityType::PROCEDURE));
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
        REQUIRE(test1.isCorrect(statements, PKBEntityType::STATEMENT));
    }

    TEST_LOG << "Modifies extraction from PKB";
    {
        TEST_LOG << "Statement modifies extraction from PKB";
        {
            auto response = pkb.getRelationship(
                PKBField::createDeclaration(StatementType::All),
                PKBField::createDeclaration(PKBEntityType::VARIABLE),
                PKBRelationship::MODIFIES);
            REQUIRE(response.hasResult);
            auto resultSet = std::get<Rows>(response.res);

            std::set<std::pair<STMT_LO, VAR_NAME>> stmtModifies;
            for (auto& row : resultSet) {
                auto stmt = std::get<STMT_LO>(row[0].content);
                auto var = std::get<VAR_NAME>(row[1].content);
                stmtModifies.insert(std::make_pair<>(stmt, var));
            }
            REQUIRE(test1.isCorrect(stmtModifies, PKBRelationship::MODIFIES));
        }
        TEST_LOG << "Procedure modifies extraction from PKB";
        {
            // 3rd param in PKBField construction is unused in getRelationship.
            auto response = pkb.getRelationship(
                PKBField::createDeclaration(PKBEntityType::PROCEDURE),
                PKBField::createDeclaration(PKBEntityType::VARIABLE),
                PKBRelationship::MODIFIES);
            REQUIRE(response.hasResult);
            auto resultSet = std::get<Rows>(response.res);

            std::set<std::pair<PROC_NAME, VAR_NAME>> procModifies;
            for (auto& row : resultSet) {
                auto proc = std::get<PROC_NAME>(row[0].content);
                auto var = std::get<VAR_NAME>(row[1].content);
                procModifies.insert(std::make_pair<>(proc, var));
            }
            REQUIRE(test1.isCorrect(procModifies, PKBRelationship::MODIFIES));
        }
    }

    TEST_LOG << "Uses extraction from PKB";
    {
        TEST_LOG << "Statement uses extraction from PKB";
        {
            auto response = pkb.getRelationship(
                PKBField::createDeclaration(StatementType::All),
                PKBField::createDeclaration(PKBEntityType::VARIABLE),
                PKBRelationship::USES);
            REQUIRE(response.hasResult);
            auto resultSet = std::get<Rows>(response.res);

            std::set<std::pair<STMT_LO, VAR_NAME>> stmtUses;
            for (auto& row : resultSet) {
                auto stmt = std::get<STMT_LO>(row[0].content);
                auto var = std::get<VAR_NAME>(row[1].content);
                stmtUses.insert(std::make_pair<>(stmt, var));
            }
            REQUIRE(test1.isCorrect(stmtUses, PKBRelationship::USES));
        }
        TEST_LOG << "Procedure uses extraction from PKB";
        {
            // 3rd param in PKBField construction is unused in getRelationship.
            auto response = pkb.getRelationship(
                PKBField::createDeclaration(PKBEntityType::PROCEDURE),
                PKBField::createDeclaration(PKBEntityType::VARIABLE),
                PKBRelationship::USES);
            REQUIRE(response.hasResult);
            auto resultSet = std::get<Rows>(response.res);

            std::set<std::pair<PROC_NAME, VAR_NAME>> procUses;
            for (auto& row : resultSet) {
                auto proc = std::get<PROC_NAME>(row[0].content);
                auto var = std::get<VAR_NAME>(row[1].content);
                procUses.insert(std::make_pair<>(proc, var));
            }
            REQUIRE(test1.isCorrect(procUses, PKBRelationship::USES));
        }
    }

    TEST_LOG << "Follows extractions from PKB";
    {
        auto response = pkb.getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(StatementType::All),
            PKBRelationship::FOLLOWS
        );
        REQUIRE(response.hasResult);
        auto resultSet = std::get<Rows>(response.res);
        std::set<std::pair<STMT_LO, STMT_LO>> followsPairs;
        for (auto& row : resultSet) {
            followsPairs.insert(p(
                std::get<STMT_LO>(row[0].content),
                std::get<STMT_LO>(row[1].content)
            ));
        }
        REQUIRE(test1.isCorrect(followsPairs, PKBRelationship::FOLLOWS));
    }

    TEST_LOG << "Follows extractions from PKB";
    {
        auto response = pkb.getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(StatementType::All),
            PKBRelationship::PARENT
        );
        REQUIRE(response.hasResult);
        auto resultSet = std::get<Rows>(response.res);
        std::set<std::pair<STMT_LO, STMT_LO>> followsPairs;
        for (auto& row : resultSet) {
            followsPairs.insert(p(
                std::get<STMT_LO>(row[0].content),
                std::get<STMT_LO>(row[1].content)
            ));
        }
        REQUIRE(test1.isCorrect(followsPairs, PKBRelationship::PARENT));
    }
}

TEST_CASE("Test parse and store for multi procedure package 2") {

    // Declarations and extractions

    PKB pkb;
    SourceProcessor sp;
    TestParseAndStorePackage2 test;
    sp.processSimple(test.sourceCode, &pkb);

    auto row = [](auto a, auto b) {
        return std::vector<PKBField>({
            PKBField::createConcrete(a),
            PKBField::createConcrete(b),
        });
    };

    auto expectedResponse = [](auto rows) {
        return PKBResponse{ true, rows };
    };

    using Rows = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;


    // Testing for results
    TEST_LOG << "Follows extractions from PKB";
    {
        auto response = pkb.getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(StatementType::All),
            PKBRelationship::NEXT
        );
        REQUIRE(response.hasResult);
        auto resultSet = std::get<Rows>(response.res);
        std::set<std::pair<STMT_LO, STMT_LO>> results;
        for (auto& row : resultSet) {
            results.insert(p(
                std::get<STMT_LO>(row[0].content),
                std::get<STMT_LO>(row[1].content)
            ));
        }
        REQUIRE(test.isCorrect(results, PKBRelationship::NEXT));
    }
}