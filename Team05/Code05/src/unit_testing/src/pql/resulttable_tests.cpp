#include "pql/resulttable.h"
#include "catch.hpp"
#include "logging.h"

#define TEST_LOG Logger() << "resulttable_tests.cpp "

PKBField field1 = PKBField::createConcrete(VAR_NAME{"main"});
PKBField field2 = PKBField::createConcrete(VAR_NAME{"a"});
PKBField field3 = PKBField::createConcrete(VAR_NAME{"b"});
PKBField field4 = PKBField::createConcrete(STMT_LO{1, StatementType::Assignment});
PKBField field5 = PKBField::createConcrete(STMT_LO{3, StatementType::Assignment});
PKBField field6 = PKBField::createConcrete(STMT_LO{6, StatementType::Assignment});

PKBField newField1 = PKBField::createConcrete(STMT_LO{1, StatementType::Assignment});
PKBField newField2 = PKBField::createConcrete(STMT_LO{5, StatementType::Assignment});
PKBField newField3 = PKBField::createConcrete(STMT_LO{6, StatementType::Assignment});
PKBField newField4 = PKBField::createConcrete(VAR_NAME{"b"});
PKBField newField5 = PKBField::createConcrete(VAR_NAME{"cur"});
PKBField newField6 = PKBField::createConcrete(VAR_NAME{"main"});


std::string PKBFieldToString(PKBField pkbField) {
    std::string res = "";
    if (pkbField.entityType == PKBEntityType::STATEMENT) {
        int lineNo = std::get<STMT_LO>(pkbField.content).statementNum;
        res = std::to_string(lineNo);
    } else if (pkbField.entityType == PKBEntityType::CONST) {
        int c = std::get<CONST>(pkbField.content);
        res = std::to_string(c);
    } else if (pkbField.entityType == PKBEntityType::VARIABLE) {
        res = std::get<VAR_NAME>(pkbField.content).name;
    } else if (pkbField.entityType == PKBEntityType::PROCEDURE) {
        res = std::get<PROC_NAME>(pkbField.content).name;
    }
    return res;
}

void printTable(qps::evaluator::ResultTable table) {
    for (auto r : table.getResult()) {
        std::string record;
        for (auto f : r) {
            record = record + PKBFieldToString(f) + " ";
        }
        TEST_LOG << record;
    }
}

qps::evaluator::ResultTable createNonEmptyTable() {
    qps::evaluator::ResultTable table{};
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> r{
            std::vector<PKBField>{field1, field4},
            std::vector<PKBField>{field2, field5},
            std::vector<PKBField>{field3, field6}};
    PKBResponse response{true, Response{r}};

    table.insert(response);
    table.insertSynLocationToLast("v");
    table.insertSynLocationToLast("s");
    return table;
}

TEST_CASE("Test getSynPos") {
    qps::evaluator::ResultTable table{};
    TEST_LOG << "create result table";
    table.insertSynLocationToLast("s");
    table.insertSynLocationToLast("v");
    table.insertSynLocationToLast("a");

    REQUIRE(table.getSynMap().size() == 3);

    REQUIRE(table.getSynLocation("s") == 0);
    REQUIRE(table.getSynLocation("v") == 1);
    REQUIRE(table.getSynLocation("a") == 2);
}

TEST_CASE("Test synExists") {
    qps::evaluator::ResultTable table{};
    REQUIRE(table.synExists("x") == false);
    TEST_LOG << "create result table";
    table.insertSynLocationToLast("s");
    table.insertSynLocationToLast("v");
    table.insertSynLocationToLast("a");


    REQUIRE(table.synExists("s"));
    REQUIRE(table.synExists("v"));
    REQUIRE(table.synExists("a"));
}

TEST_CASE("Test insert single synonym") {
    qps::evaluator::ResultTable table{};
    REQUIRE(table.synExists("x") == false);
    TEST_LOG << "create result table";

    std::unordered_set<PKBField, PKBFieldHash> r{field1, field2, field3};
    PKBResponse response{true, Response{r}};

    table.insert(response);
    table.insertSynLocationToLast("v");

    REQUIRE(table.getSynLocation("v") == 0);
    REQUIRE(table.getResult().size() == 3);
    REQUIRE(table.getResult().find(std::vector<PKBField>{field1}) != table.getResult().end());
    REQUIRE(table.getResult().find(std::vector<PKBField>{field2}) != table.getResult().end());
    REQUIRE(table.getResult().find(std::vector<PKBField>{field3}) != table.getResult().end());
}

TEST_CASE("Test insert vector") {
    qps::evaluator::ResultTable table{};
    REQUIRE(table.synExists("x") == false);
    TEST_LOG << "create result table";

    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> r{
            std::vector<PKBField>{field1, field4},
            std::vector<PKBField>{field2, field5},
            std::vector<PKBField>{field3, field6}};
    PKBResponse response{true, Response{r}};

    table.insert(response);
    table.insertSynLocationToLast("v");
    table.insertSynLocationToLast("s");

    REQUIRE(table.getSynLocation("v") == 0);
    REQUIRE(table.getResult().size() == 3);
    REQUIRE(table.getResult().find(std::vector<PKBField>{field1, field4}) != table.getResult().end());
    REQUIRE(table.getResult().find(std::vector<PKBField>{field2, field5}) != table.getResult().end());
    REQUIRE(table.getResult().find(std::vector<PKBField>{field3, field6}) != table.getResult().end());
}

TEST_CASE("Test Empty table cross join") {
    qps::evaluator::ResultTable table{};
    TEST_LOG << "create result table";
    std::unordered_set<PKBField, PKBFieldHash> r{field1, field2, field3};
    PKBResponse response{true, Response{r}};

    table.crossJoin(response);
    table.insertSynLocationToLast("v");

    REQUIRE(table.getSynLocation("v") == 0);
    REQUIRE(table.getResult().size() == 3);
    REQUIRE(table.getResult().find(std::vector<PKBField>{field1}) != table.getResult().end());
    REQUIRE(table.getResult().find(std::vector<PKBField>{field2}) != table.getResult().end());
    REQUIRE(table.getResult().find(std::vector<PKBField>{field3}) != table.getResult().end());

    qps::evaluator::ResultTable table1{};

    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> r1{
            std::vector<PKBField>{field1, field4},
            std::vector<PKBField>{field2, field5},
            std::vector<PKBField>{field3, field6}};
    PKBResponse response1{true, Response{r1}};

    table1.crossJoin(response1);
    table1.insertSynLocationToLast("v");
    table1.insertSynLocationToLast("s");

    REQUIRE(table1.getSynLocation("v") == 0);
    REQUIRE(table1.getSynLocation("s") == 1);
    REQUIRE(table1.getResult().size() == 3);
    REQUIRE(table1.getResult().find(std::vector<PKBField>{field1, field4}) != table.getResult().end());
    REQUIRE(table1.getResult().find(std::vector<PKBField>{field2, field5}) != table.getResult().end());
    REQUIRE(table1.getResult().find(std::vector<PKBField>{field3, field6}) != table.getResult().end());
}

TEST_CASE("Test crossJoin with records inside") {
    qps::evaluator::ResultTable table = createNonEmptyTable();
    qps::evaluator::ResultTable table2 = createNonEmptyTable();

    PKBField newField1 = PKBField::createConcrete(STMT_LO{2, StatementType::Assignment});
    PKBField newField2 = PKBField::createConcrete(STMT_LO{5, StatementType::Assignment});
    table.insertSynLocationToLast("a");
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> r1{
        std::vector<PKBField>{newField1},
        std::vector<PKBField>{newField2}
    };
    PKBResponse response1{true, Response{r1}};
    table.crossJoin(response1);

    REQUIRE(table.getResult().size() == 6);
    REQUIRE(table.getResult().find(std::vector<PKBField>{field1, field4, newField1}) != table.getResult().end());
    REQUIRE(table.getResult().find(std::vector<PKBField>{field1, field4, newField2}) != table.getResult().end());
    REQUIRE(table.getResult().find(std::vector<PKBField>{field2, field5, newField1}) != table.getResult().end());
    REQUIRE(table.getResult().find(std::vector<PKBField>{field2, field5, newField2}) != table.getResult().end());
    REQUIRE(table.getResult().find(std::vector<PKBField>{field3, field6, newField1}) != table.getResult().end());
    REQUIRE(table.getResult().find(std::vector<PKBField>{field3, field6, newField2}) != table.getResult().end());
    printTable(table);
    TEST_LOG << "Cross join a vector of response";
    PKBField newField3 = PKBField::createConcrete(Content{45});
    PKBField newField4 = PKBField::createConcrete(Content{26});
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> r2{
            std::vector<PKBField>{newField1, newField3},
            std::vector<PKBField>{newField1, newField4},
            std::vector<PKBField>{newField2, newField3}};
    PKBResponse response2{true, Response{r2}};
    table2.insertSynLocationToLast("a");
    table2.insertSynLocationToLast("c");
    REQUIRE(table2.getSynLocation("a") == 2);
    REQUIRE(table2.getSynLocation("c") == 3);
    table2.crossJoin(response2);
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field1, field4, newField1, newField3}) !=
            table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field1, field4, newField1, newField4}) !=
            table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field1, field4, newField2, newField3}) !=
            table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field2, field5, newField1, newField3}) !=
            table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field2, field5, newField1, newField4}) !=
            table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field2, field5, newField2, newField3}) !=
            table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field3, field6, newField1, newField3}) !=
            table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field3, field6, newField1, newField4}) !=
            table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field3, field6, newField2, newField3}) !=
            table.getResult().end());
    printTable(table2);
}

TEST_CASE("Test innerJoin with empty table") {
    qps::evaluator::ResultTable table{};
    TEST_LOG << "create result table";
    table.insertSynLocationToLast("v");
    std::unordered_set<PKBField, PKBFieldHash> r{field1, field2, field3};
    PKBResponse response{true, Response{r}};

    table.innerJoin(response, true, false, std::vector<std::string>{"v"});

    REQUIRE(table.getSynLocation("v") == 0);
    REQUIRE(table.getResult().size() == 0);

    qps::evaluator::ResultTable table1{};
    table1.insertSynLocationToLast("v");
    table1.insertSynLocationToLast("s");

    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> r1{
            std::vector<PKBField>{field1, field4},
            std::vector<PKBField>{field2, field5},
            std::vector<PKBField>{field3, field6}};
    PKBResponse response1{true, Response{r1}};

    table1.innerJoin(response1, true, true, std::vector<std::string>{"v", "s"});

    REQUIRE(table1.getSynLocation("v") == 0);
    REQUIRE(table1.getSynLocation("s") == 1);
    REQUIRE(table1.getResult().size() == 0);
}

TEST_CASE("Test innerJoin with records in table") {
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> testR1{std::vector<PKBField>{newField1},
                                                                         std::vector<PKBField>{newField2},
                                                                         std::vector<PKBField>{newField3}};
    PKBResponse response1{true, Response{testR1}};
    TEST_LOG << "========== one synonym join s";
    qps::evaluator::ResultTable table = createNonEmptyTable();
    table.innerJoin(response1, true, false, std::vector<std::string>{"s"});
    printTable(table);
    REQUIRE(table.getResult().size() == 2);
    //  table: main 1 / b 6

    //  when PKBResponse = set<vector<PKBField>> main 1/ a 3/ b 6
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> testR3{
            std::vector<PKBField>{newField1, newField6},   // 1 main
            std::vector<PKBField>{newField1, newField5},   // 1 cur
            std::vector<PKBField>{newField2, newField4},   // 5 b
            std::vector<PKBField>{newField3, newField6}};  // 6 main
    PKBResponse response3{true, Response{testR3}};
    TEST_LOG << "========== Join s and v";
    qps::evaluator::ResultTable table2 = createNonEmptyTable();
    table2.innerJoin(response3, true, true, std::vector<std::string>{"s", "v"});
    REQUIRE(table2.getResult().size() == 1);
    printTable(table2);
    // table: main 1

    TEST_LOG << "========== Join s only";
    qps::evaluator::ResultTable table3 = createNonEmptyTable();
    table3.insertSynLocationToLast("v1");
    table3.innerJoin(response3, true, false, std::vector<std::string>{"s", "v1"});
    REQUIRE(table3.getResult().size() == 3);
    printTable(table3);
    // table: main 1 main / main 1 cur / b 6 main

    TEST_LOG << "========== Join v only";
    qps::evaluator::ResultTable table4 = createNonEmptyTable();
    table4.insertSynLocationToLast("s1");
    REQUIRE(table4.getSynLocation("s1") == 2);
    table4.innerJoin(response3, false, true, std::vector<std::string>{"s1", "v"});
    REQUIRE(table4.getResult().size() == 3);
    printTable(table4);
    //  table: main 1 1 / main 1 6 / b 6 5
}

TEST_CASE("Test join method") {
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> testVector{
            std::vector<PKBField>{newField1, newField4},
            std::vector<PKBField>{newField1, newField6},
            std::vector<PKBField>{newField2, newField5},
            std::vector<PKBField>{newField2, newField6},
            std::vector<PKBField>{newField3, newField4},
    };
    PKBResponse responseVector{true, Response{testVector}};

    TEST_LOG << "========== 1 syn join (cross join 1)";
    qps::evaluator::ResultTable table1{};
    std::vector<std::string> synonyms1{"v"};
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> testR1{
            std::vector<PKBField>{field1},
            std::vector<PKBField>{field2},
            std::vector<PKBField>{field3}
    };
    PKBResponse response1{true, Response{testR1}};

    table1.join(response1, synonyms1);
    REQUIRE(table1.getSynLocation("v") == 0);
    REQUIRE(table1.getResult().size() == 3);
    printTable(table1);

    TEST_LOG << "========== 1 syn join (cross join 2)";
    qps::evaluator::ResultTable table2 = createNonEmptyTable();
    std::vector<std::string> synonyms2{"v1"};
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> testR2{
            std::vector<PKBField>{newField4},
            std::vector<PKBField>{newField5},
            std::vector<PKBField>{newField6}
    };
    PKBResponse response2{true, Response{testR2}};
    table2.join(response2, synonyms2);
    REQUIRE(table2.getSynLocation("v1") == 2);
    REQUIRE(table2.getResult().size() == 9);
    printTable(table2);

    TEST_LOG << "========== 1 syn join (empty result cross join 3)";
    qps::evaluator::ResultTable table3 = createNonEmptyTable();
    std::vector<std::string> synonyms3{"v1"};
    std::unordered_set<PKBField, PKBFieldHash> testR3{};
    PKBResponse response3{true, Response{testR3}};
    table3.join(response3, synonyms3);
    REQUIRE(table3.getSynLocation("v1") == 2);
    REQUIRE(table3.getResult().size() == 0);
    printTable(table3);

    TEST_LOG << "========== 1 syn join (inner join 1)";
    qps::evaluator::ResultTable table4 = createNonEmptyTable();
    std::vector<std::string> synonyms4{"s"};
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> testR4{std::vector<PKBField>{newField1},
                                                      std::vector<PKBField>{newField2},
                                                      std::vector<PKBField>{newField3}};
    PKBResponse response4{true, Response{testR4}};
    table4.join(response4, synonyms4);
    REQUIRE(table4.getSynLocation("s") == 1);
    REQUIRE(table4.getResult().size() == 2);
    printTable(table4);

    TEST_LOG << "========== 2 syns join (cross join)";
    qps::evaluator::ResultTable table5 = createNonEmptyTable();
    std::vector<std::string> synonyms5{"s1", "v1"};
    table5.join(responseVector, synonyms5);
    REQUIRE(table5.getSynLocation("s1") == 2);
    REQUIRE(table5.getSynLocation("v1") == 3);
    REQUIRE(table5.getResult().size() == 15);
    printTable(table5);

    TEST_LOG << "========== 2 syns join (inner join 1)";
    qps::evaluator::ResultTable table6{};
    table6.insertSynLocationToLast("v");
    table6.insertSynLocationToLast("s");
    std::vector<std::string> synonyms6{"s", "v"};
    table6.join(responseVector, synonyms6);
    REQUIRE(table6.getSynLocation("v") == 0);
    REQUIRE(table6.getSynLocation("s") == 1);
    REQUIRE(table6.getResult().size() == 0);
    printTable(table6);

    TEST_LOG << "========== 2 syns join (inner join 2)";
    qps::evaluator::ResultTable table7 = createNonEmptyTable();
    std::vector<std::string> synonyms7{"s", "v"};
    table7.join(responseVector, synonyms7);
    REQUIRE(table7.getSynLocation("v") == 0);
    REQUIRE(table7.getSynLocation("s") == 1);
    REQUIRE(table7.getResult().size() == 2);
    printTable(table7);

    TEST_LOG << "========== 2 syns join (inner join 3)";
    qps::evaluator::ResultTable table8 = createNonEmptyTable();
    std::vector<std::string> synonyms8{"s", "v1"};
    table8.join(responseVector, synonyms8);
    REQUIRE(table8.getSynLocation("v1") == 2);
    REQUIRE(table8.getSynLocation("s") == 1);
    REQUIRE(table8.getResult().size() == 3);
    printTable(table8);

    TEST_LOG << "========== 2 syns join (inner join 4)";
    qps::evaluator::ResultTable table9 = createNonEmptyTable();
    std::vector<std::string> synonyms9{"s1", "v"};
    table9.join(responseVector, synonyms9);
    REQUIRE(table9.getSynLocation("v") == 0);
    REQUIRE(table9.getSynLocation("s1") == 2);
    REQUIRE(table9.getResult().size() == 4);
    printTable(table9);

    TEST_LOG << "========== 1 syn join (empty response inner join 5)";
    qps::evaluator::ResultTable table10 = createNonEmptyTable();
    std::vector<std::string> synonyms10{"v"};
    std::unordered_set<PKBField, PKBFieldHash> testR10{};
    PKBResponse response10{false, Response{testR10}};
    table10.join(response10, synonyms10);
    REQUIRE(table10.getSynLocation("v") == 0);
    REQUIRE(table10.getResult().size() == 0);
    printTable(table10);

    TEST_LOG << "========== 2 syn join (empty response inner join 6)";
    qps::evaluator::ResultTable table11 = createNonEmptyTable();
    std::vector<std::string> synonyms11{"v", "s"};
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> testR11{};
    PKBResponse response11{false, Response{testR11}};
    table11.join(response11, synonyms11);
    REQUIRE(table11.getSynLocation("v") == 0);
    REQUIRE(table11.getResult().size() == 0);
    printTable(table11);

    TEST_LOG << "========== 2 syn join (empty response cross join 2)";
    qps::evaluator::ResultTable table12 = createNonEmptyTable();
    std::vector<std::string> synonyms12{"v1", "s1"};
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> testR12{};
    PKBResponse response12{false, Response{testR12}};
    table12.join(response12, synonyms12);
    REQUIRE(table12.getSynLocation("v1") == 2);
    REQUIRE(table12.getSynLocation("s1") == 3);
    REQUIRE(table12.getResult().size() == 0);
    printTable(table12);
}
