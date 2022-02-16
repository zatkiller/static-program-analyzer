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

struct TestParseAndStorePackage1 {
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
    bool isCorrect(std::set<std::pair<T,K>> result, PKBRelationship targetType) {
        auto targetSet = expectedRelationships[targetType];
        std::set<std::pair<T,K>> expected;
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
                STMT_LO{1, StatementType::Read},
                STMT_LO{2, StatementType::Assignment},
                STMT_LO{3, StatementType::While},
                STMT_LO{4, StatementType::Assignment},
                STMT_LO{5, StatementType::Assignment},
                STMT_LO{6, StatementType::Assignment},
                STMT_LO{7, StatementType::Print}
            })
        );
    }
    void initRelationships() {
        expectedRelationships.emplace(
            PKBRelationship::MODIFIES,
            std::set<std::pair<Content, Content>> {
                p(STMT_LO{1, StatementType::Read}, VAR_NAME{"number"}),
                p(STMT_LO{2, StatementType::Assignment}, VAR_NAME{"sum"}),
                p(STMT_LO{3, StatementType::While}, VAR_NAME{"digit"}),
                p(STMT_LO{3, StatementType::While}, VAR_NAME{"sum"}),
                p(STMT_LO{3, StatementType::While}, VAR_NAME{"number"}),
                p(STMT_LO{4, StatementType::Assignment}, VAR_NAME{"digit"}),
                p(STMT_LO{5, StatementType::Assignment}, VAR_NAME{"sum"}),
                p(STMT_LO{6, StatementType::Assignment}, VAR_NAME{"number"}),
                p(PROC_NAME{"sumDigits"}, VAR_NAME{"number"}),
                p(PROC_NAME{"sumDigits"}, VAR_NAME{"digit"}),
                p(PROC_NAME{"sumDigits"}, VAR_NAME{"sum"})
            }
        );

        expectedRelationships.emplace(
            PKBRelationship::USES,
            std::set<std::pair<Content, Content>> {
                p(PROC_NAME{"sumDigits"}, VAR_NAME{"number"}),
                p(PROC_NAME{"sumDigits"}, VAR_NAME{"digit"}),
                p(PROC_NAME{"sumDigits"}, VAR_NAME{"sum"}),
                p(STMT_LO{3, StatementType::While}, VAR_NAME{"number"}),
                p(STMT_LO{3, StatementType::While}, VAR_NAME{"sum"}),
                p(STMT_LO{3, StatementType::While}, VAR_NAME{"digit"}),
                p(STMT_LO{4, StatementType::Assignment}, VAR_NAME{"number"}),
                p(STMT_LO{5, StatementType::Assignment}, VAR_NAME{"sum"}),
                p(STMT_LO{5, StatementType::Assignment}, VAR_NAME{"digit"}),
                p(STMT_LO{6, StatementType::Assignment}, VAR_NAME{"number"}),
                p(STMT_LO{7, StatementType::Print}, VAR_NAME{"sum"}),
            }
        );

        expectedRelationships.emplace(
            PKBRelationship::FOLLOWS,
            std::set<std::pair<Content, Content>> {
                p(STMT_LO{1, StatementType::Read}, STMT_LO{2, StatementType::Assignment}),
                p(STMT_LO{2, StatementType::Assignment}, STMT_LO{3, StatementType::While}),
                p(STMT_LO{3, StatementType::While}, STMT_LO{7, StatementType::Print}),
                p(STMT_LO{4, StatementType::Assignment}, STMT_LO{5, StatementType::Assignment}),
                p(STMT_LO{5, StatementType::Assignment}, STMT_LO{6, StatementType::Assignment}),
            }
        );

        expectedRelationships.emplace(
            PKBRelationship::PARENT,
            std::set<std::pair<Content, Content>> {
                p(STMT_LO{3, StatementType::While}, STMT_LO{4, StatementType::Assignment}),
                p(STMT_LO{3, StatementType::While}, STMT_LO{5, StatementType::Assignment}),
                p(STMT_LO{3, StatementType::While}, STMT_LO{6, StatementType::Assignment}),
            }
        );

    }
    TestParseAndStorePackage1() {
        initEntities();
        initRelationships();
    }
};

TEST_CASE("Test parse and store") {
    PKB pkb;
    SourceProcessor sp;
    TestParseAndStorePackage1 test1;
    sp.processSimple(test1.sourceCode, &pkb);
    using Rows = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

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
        for (auto &row : resultSet) {
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
        for (auto &row : resultSet) {
            followsPairs.insert(p(
                std::get<STMT_LO>(row[0].content), 
                std::get<STMT_LO>(row[1].content)
            ));
        }
        REQUIRE(test1.isCorrect(followsPairs, PKBRelationship::PARENT));
    }
}
