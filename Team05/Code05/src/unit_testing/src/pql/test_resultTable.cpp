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
    ResultTable table = createNonEmptyTable();
    ResultTable table2 = createNonEmptyTable();
    TEST_LOG << "create result table";


    //Initial table created

    //Test : PKBResponse = set<PKBField>
    PKBField newField1(PKBType::STATEMENT, true, Content{STMT_LO {1}});
    PKBField newField2(PKBType::STATEMENT, true, Content{STMT_LO {5}});
    PKBField newField3(PKBType::STATEMENT, true, Content{STMT_LO {6}});

    std::unordered_set<PKBField, PKBFieldHash> testR1{newField1, newField2, newField3};
    std::unordered_set<PKBField, PKBFieldHash> testR2{newField2};
    PKBResponse response1{true, Response{testR1}};
    PKBResponse response2{true, Response{testR2}};
//    table.innerJoin(response2, true, false, std::vector<std::string>{"s"});
//    printTable(table);
//    REQUIRE(table.getResult().size() == 0);
    TEST_LOG << "========== one synonym join s";
    table.innerJoin(response1, true, false, std::vector<std::string>{"s"});
    printTable(table);
    REQUIRE(table.getResult().size() == 2);
    //table: main 1 / b 6

    //when PKBResponse = set<vector<PKBField>> main 1/ a 3/ b 6
    PKBField newField4(PKBType::VARIABLE, true, Content{VAR_NAME {"b"}});
    PKBField newField5(PKBType::VARIABLE, true, Content{VAR_NAME {"cur"}});
    PKBField newField6(PKBType::VARIABLE, true, Content{VAR_NAME {"main"}});
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> testR3{
            std::vector<PKBField>{newField1, newField6}, // 1 main
            std::vector<PKBField>{newField1, newField5}, // 1 cur
            std::vector<PKBField>{newField2, newField4}, // 5 b
            std::vector<PKBField>{newField3, newField6}}; //6 main
    PKBResponse response3{true, Response{testR3}};
    TEST_LOG << "========== Join s and v";
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
    table4.innerJoin(response3, false, true, std::vector<std::string>{"s1", "v"});
    REQUIRE(table4.getResult().size() == 3);
    printTable(table4);
    //table: main 1 1 / main 1 6 / b 6 5
}