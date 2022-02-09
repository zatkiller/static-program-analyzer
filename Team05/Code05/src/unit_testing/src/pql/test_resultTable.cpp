#include "pql/resultTable.h"
#include "catch.hpp"
#include "logging.h"

#define TEST_LOG Logger() << "test_resultTable.cpp "

PKBField field1{ PKBType::VARIABLE, true, Content{VAR_NAME {"main"}} };
PKBField field2{ PKBType::VARIABLE, true, Content{VAR_NAME{"a"}} };
PKBField field3{ PKBType::VARIABLE, true, Content{VAR_NAME{"b"}} };
PKBField field4{ PKBType::STATEMENT, true, Content{STMT_LO {1}} };
PKBField field5{ PKBType::STATEMENT, true, Content{STMT_LO{3}} };
PKBField field6{ PKBType::STATEMENT, true, Content{STMT_LO{6}} };

PKBField newField1(PKBType::STATEMENT, true, Content{STMT_LO {1}});
PKBField newField2(PKBType::STATEMENT, true, Content{STMT_LO {5}});
PKBField newField3(PKBType::STATEMENT, true, Content{STMT_LO {6}});
PKBField newField4(PKBType::VARIABLE, true, Content{VAR_NAME {"b"}});
PKBField newField5(PKBType::VARIABLE, true, Content{VAR_NAME {"cur"}});
PKBField newField6(PKBType::VARIABLE, true, Content{VAR_NAME {"main"}});

std::string PKBFieldToString(PKBField pkbField) {
    std::string res = "";
    if(pkbField.tag == PKBType::STATEMENT) {
        int lineNo = std::get<STMT_LO>(pkbField.content).statementNum;
        res = std::to_string(lineNo);
    } else if ( pkbField.tag == PKBType::CONST) {
        int c = std::get<CONST>(pkbField.content);
        res = std::to_string(c);
    } else if (pkbField.tag == PKBType::VARIABLE) {
        res = std::get<VAR_NAME>(pkbField.content).name;
    } else if (pkbField.tag == PKBType::PROCEDURE) {
        res = std::get<PROC_NAME>(pkbField.content).name;
    }
    return res;
}

void printTable(ResultTable table) {
    for (auto r : table.getResult()) {
        std::string record;
        for (auto f : r) {
            record = record + PKBFieldToString(f) + " ";
        }
        TEST_LOG << record;
    }
}

ResultTable createNonEmptyTable() {
    ResultTable table{};
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
    ResultTable table{};
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
    ResultTable table{};
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
    ResultTable table{};
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
    ResultTable table{};
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
    ResultTable table{};
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

    ResultTable table1{};

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
    ResultTable table = createNonEmptyTable();
    ResultTable table2 = createNonEmptyTable();

    PKBField newField1(PKBType::STATEMENT, true, Content{STMT_LO {2, StatementType::Assignment}});
    PKBField newField2(PKBType::STATEMENT, true, Content{STMT_LO {5, StatementType::Assignment}});
    table.insertSynLocationToLast("a");
    std::unordered_set<PKBField, PKBFieldHash> r1{newField1, newField2};
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
    PKBField newField3(PKBType::CONST, true, Content{CONST{45}});
    PKBField newField4(PKBType::CONST, true, Content{CONST{26}});
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
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field1, field4, newField1, newField3}) != table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field1, field4, newField1, newField4}) != table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field1, field4, newField2, newField3}) != table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field2, field5, newField1, newField3}) != table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field2, field5, newField1, newField4}) != table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field2, field5, newField2, newField3}) != table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field3, field6, newField1, newField3}) != table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field3, field6, newField1, newField4}) != table.getResult().end());
    REQUIRE(table2.getResult().find(std::vector<PKBField>{field3, field6, newField2, newField3}) != table.getResult().end());
    printTable(table2);

}

TEST_CASE("Test innerJoin with empty table") {
    ResultTable table{};
    TEST_LOG << "create result table";
    table.insertSynLocationToLast("v");
    std::unordered_set<PKBField, PKBFieldHash> r{field1, field2, field3};
    PKBResponse response{true, Response{r}};

    table.innerJoin(response, true, false, std::vector<std::string>{"v"});

    REQUIRE(table.getSynLocation("v") == 0);
    REQUIRE(table.getResult().size() == 0);

    ResultTable table1{};
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
    //Test : PKBResponse = set<PKBField>

    std::unordered_set<PKBField, PKBFieldHash> testR1{newField1, newField2, newField3};
    PKBResponse response1{true, Response{testR1}};
    TEST_LOG << "========== one synonym join s";
    ResultTable table = createNonEmptyTable();
    table.innerJoin(response1, true, false, std::vector<std::string>{"s"});
    printTable(table);
    REQUIRE(table.getResult().size() == 2);
    //table: main 1 / b 6

    //when PKBResponse = set<vector<PKBField>> main 1/ a 3/ b 6
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> testR3{
            std::vector<PKBField>{newField1, newField6}, // 1 main
            std::vector<PKBField>{newField1, newField5}, // 1 cur
            std::vector<PKBField>{newField2, newField4}, // 5 b
            std::vector<PKBField>{newField3, newField6}}; //6 main
    PKBResponse response3{true, Response{testR3}};
    TEST_LOG << "========== Join s and v";
    ResultTable table2 = createNonEmptyTable();
    table2.innerJoin(response3, true, true, std::vector<std::string>{"s", "v"});
    REQUIRE(table2.getResult().size() == 1);
    printTable(table2);
    // table: main 1

    TEST_LOG << "========== Join s only";
    ResultTable table3 = createNonEmptyTable();
    table3.insertSynLocationToLast("v1");
    table3.innerJoin(response3, true, false, std::vector<std::string>{"s", "v1"});
    REQUIRE(table3.getResult().size() == 3);
    printTable(table3);
    //table: main 1 main / main 1 cur / b 6 main

    TEST_LOG << "========== Join v only";
    ResultTable table4 = createNonEmptyTable();
    table4.insertSynLocationToLast("s1");
    REQUIRE(table4.getSynLocation("s1") == 2);
    TEST_LOG << "tabe4 s1 position 2";
    table4.innerJoin(response3, false, true, std::vector<std::string>{"s1", "v"});
    REQUIRE(table4.getResult().size() == 3);
    printTable(table4);
    //table: main 1 1 / main 1 6 / b 6 5
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
    ResultTable table1{};
    std::vector<std::string> synonyms1{"v"};
    std::unordered_set<PKBField, PKBFieldHash> testR1{field1, field2, field3};
    PKBResponse response1{true, Response{testR1}};

    table1.join(response1, synonyms1);
    REQUIRE(table1.getSynLocation("v") == 0);
    REQUIRE(table1.getResult().size() == 3);
    printTable(table1);

    TEST_LOG << "========== 1 syn join (cross join 1)";
    ResultTable table2 = createNonEmptyTable();
    std::vector<std::string> synonyms2{"v1"};
    std::unordered_set<PKBField, PKBFieldHash> testR2{newField4, newField5, newField6};
    PKBResponse response2{true, Response{testR2}};
    table2.join(response2, synonyms2);
    REQUIRE(table2.getSynLocation("v1") == 2);
    REQUIRE(table2.getResult().size() == 9);
    printTable(table2);

    TEST_LOG << "========== 1 syn join (inner join 1)";
    ResultTable table3 = createNonEmptyTable();
    std::vector<std::string> synonyms3{"s"};
    std::unordered_set<PKBField, PKBFieldHash> testR3{newField1, newField2, newField3};
    PKBResponse response3{true, Response{testR3}};

    table3.join(response3, synonyms3);
    REQUIRE(table3.getSynLocation("s") == 1);
    REQUIRE(table3.getResult().size() == 2);
    printTable(table3);

    TEST_LOG << "========== 2 syns join (cross join)";
    ResultTable table4 = createNonEmptyTable();
    std::vector<std::string> synonyms4{"s1", "v1"};
    table4.join(responseVector, synonyms4);
    REQUIRE(table4.getSynLocation("s1") == 2);
    REQUIRE(table4.getSynLocation("v1") == 3);
    REQUIRE(table4.getResult().size() == 15);
    printTable(table4);

    TEST_LOG << "========== 2 syns join (inner join 1)";
    ResultTable table5{};
    table5.insertSynLocationToLast("v");
    table5.insertSynLocationToLast("s");
    std::vector<std::string> synonyms5{"s", "v"};
    table5.join(responseVector, synonyms5);
    REQUIRE(table5.getSynLocation("v") == 0);
    REQUIRE(table5.getSynLocation("s") == 1);
    REQUIRE(table5.getResult().size() == 0);
    printTable(table5);

    TEST_LOG << "========== 2 syns join (inner join 2)";
    ResultTable table6 = createNonEmptyTable();
    table6.join(responseVector, synonyms5);
    REQUIRE(table6.getSynLocation("v") == 0);
    REQUIRE(table6.getSynLocation("s") == 1);
    REQUIRE(table6.getResult().size() == 2);
    printTable(table6);

    TEST_LOG << "========== 2 syns join (inner join 3)";
    ResultTable table7 = createNonEmptyTable();
    std::vector<std::string> synonyms7{"s", "v1"};
    table7.join(responseVector, synonyms7);
    REQUIRE(table7.getSynLocation("v1") == 2);
    REQUIRE(table7.getSynLocation("s") == 1);
    REQUIRE(table7.getResult().size() == 3);
    printTable(table7);

    TEST_LOG << "========== 2 syns join (inner join 4)";
    ResultTable table8 = createNonEmptyTable();
    std::vector<std::string> synonyms8{"s1", "v"};
    table8.join(responseVector, synonyms8);
    REQUIRE(table8.getSynLocation("v") == 0);
    REQUIRE(table8.getSynLocation("s1") == 2);
    REQUIRE(table8.getResult().size() == 4);
    printTable(table8);

}