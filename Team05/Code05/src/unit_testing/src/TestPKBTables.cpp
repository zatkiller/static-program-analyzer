#include "PKB/PKBTables.h"
#include "catch.hpp"

TEST_CASE("ConstantRow::getConst") {
    ConstantRow row1{ CONST{1} };

    REQUIRE(row1.getConst() == 1);
    REQUIRE(row1.getConst() == CONST{ 1 });
}

TEST_CASE("ConstantRow::==") {
    ConstantRow row1{ CONST{1} };
    ConstantRow row2{ CONST{1} };
    ConstantRow row3{ CONST{2} };

    REQUIRE(row1 == row2);
    REQUIRE(row1 == row1);
    REQUIRE_FALSE(row1 == row3);
}

TEST_CASE("ConstantTable::insert, ConstantTable::contains") {
    ConstantTable table{};
    PKBField field1 = PKBField::createConcrete(CONST{ 1 });
    PKBField field2 = PKBField::createConcrete(CONST{ 2 });
    PKBField field3 = PKBField::createConcrete(CONST{ 1 });
    REQUIRE_FALSE(table.contains(CONST{ 1 }));

    table.insert(CONST{ 1 });
    REQUIRE(table.contains(CONST{ 1 }));
    REQUIRE_FALSE(table.contains(CONST{ 2 }));
}

TEST_CASE("ConstantTable::getSize") {
    ConstantTable table{};
    REQUIRE(table.getSize() == 0);

    table.insert(CONST{ 1 });
    REQUIRE(table.getSize() == 1);

    // Insert duplicate CONST
    table.insert(CONST{ 1 });
    REQUIRE(table.getSize() == 1);
}

TEST_CASE("ConstantTable::getAllConst") {
    // Empty table
    ConstantTable table{};
    std::vector<CONST> expected{};
    REQUIRE(expected == table.getAllConst());

    table.insert(CONST{ 1 });
    table.insert(CONST{ 2 });
    expected = { CONST{1}, CONST{2} };
    REQUIRE(expected == table.getAllConst());

    std::vector<CONST> wrong_order{};
    wrong_order = { CONST{2}, CONST{1} };
    REQUIRE_FALSE(wrong_order == table.getAllConst());

    // Ordering check
    table.insert(CONST{ 4 });
    table.insert(CONST{ 3 });
    expected = { CONST{1}, CONST{2}, CONST{3}, CONST{4} };
    REQUIRE(expected == table.getAllConst());
}

TEST_CASE("ProcedureRow getProcName") {
    ProcedureRow row1{ PROC_NAME{"main"} };
    REQUIRE(row1.getProcName() == PROC_NAME{ "main" });

    ProcedureRow row2{ PROC_NAME{""} };
    REQUIRE(row2.getProcName() == PROC_NAME{ "" });
}

TEST_CASE("ProcedureRow ==") {
    ProcedureRow row1{ PROC_NAME{"main"} };
    ProcedureRow row2{ PROC_NAME{"main"} };
    ProcedureRow row3{ PROC_NAME{"foo"} };

    REQUIRE(row1 == row2);
    REQUIRE(row1 == row1);
    REQUIRE_FALSE(row1 == row3);
}

TEST_CASE("ProcedureTable insert and contains") {
    ProcedureTable table{};
    REQUIRE_FALSE(table.contains("main"));

    table.insert("main");
    table.insert("main");
    REQUIRE(table.contains("main"));

    REQUIRE(table.contains("main"));
    REQUIRE_FALSE(table.contains("foo"));
}

TEST_CASE("ProcedureTable getSize") {
    ProcedureTable table{};
    REQUIRE(table.getSize() == 0);

    table.insert("main");
    REQUIRE(table.getSize() == 1);

    table.insert("main");
    REQUIRE(table.getSize() == 1);
}

TEST_CASE("ProcedureTable getAllProcs") {
    ProcedureTable table{};
    std::vector<PROC_NAME> expected{};
    REQUIRE(expected == table.getAllProcs());

    table.insert("main");
    table.insert("foo");
    expected = { PROC_NAME{"foo"}, PROC_NAME{"main"} };
    REQUIRE(expected == table.getAllProcs());
}

TEST_CASE("StatementRow getStmt") {
    StatementRow row1{ StatementType::Assignment, 1 };

    REQUIRE(row1.getStmt() == STMT_LO{ 1, StatementType::Assignment });
}

TEST_CASE("StatementRow ==") {
    StatementRow row1{ StatementType::Assignment, 1 };
    StatementRow row2{ StatementType::Assignment, 1 };
    StatementRow row3{ StatementType::Assignment, 2 };
    StatementRow row4{ StatementType::While, 1 };
    StatementRow row5{ StatementType::While, 2 };

    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);

    REQUIRE_FALSE(row1 == row3);
    REQUIRE_FALSE(row1 == row4);
    REQUIRE_FALSE(row1 == row5);
}

TEST_CASE("StatementTable insert and contains") {
    StatementTable table{};
    StatementType type1 = StatementType::Assignment;
    StatementType type2 = StatementType::While;
    int statementNumber1 = 1;
    int statementNumber2 = 2;

    REQUIRE_FALSE(table.contains(type1, statementNumber1));

    table.insert(type1, statementNumber1);
    table.insert(type1, statementNumber1);
    REQUIRE(table.getSize() == 1);

    REQUIRE(table.contains(type1, statementNumber1));
    REQUIRE_FALSE(table.contains(type2, statementNumber2));
    REQUIRE_FALSE(table.contains(type1, statementNumber2));
    REQUIRE_FALSE(table.contains(type2, statementNumber2));
}

TEST_CASE("StatementTable getSize") {
    StatementTable table{};
    REQUIRE(table.getSize() == 0);

    table.insert(StatementType::Assignment, 1);
    REQUIRE(table.getSize() == 1);

    table.insert(StatementType::Assignment, 1);
    REQUIRE(table.getSize() == 1);
}

TEST_CASE("StatementTable getAllStmt") {
    StatementTable table{};
    std::vector<STMT_LO> expected{};
    REQUIRE(expected == table.getAllStmt());

    table.insert(StatementType::Assignment, 2);
    table.insert(StatementType::Assignment, 1);
    expected = { STMT_LO{1, StatementType::Assignment}, STMT_LO{2, StatementType::Assignment} };
    REQUIRE(expected == table.getAllStmt());
}

TEST_CASE("VariableRow getStmt") {
    VariableRow row1{ VAR_NAME{"a"} };
    REQUIRE(row1.getVarName() == VAR_NAME{ "a" });
}

TEST_CASE("VariableRow ==") {
    VariableRow row1{ VAR_NAME{"a"} };
    VariableRow row2{ VAR_NAME{"a"} };
    VariableRow row3{ VAR_NAME{"b"} };

    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);
    REQUIRE_FALSE(row1 == row3);
}

TEST_CASE("VariableTable insert and contains") {
    VariableTable table{};
    std::string s1{ "a" };
    std::string s2{ "b" };
    REQUIRE_FALSE(table.contains(s1));

    table.insert(s1);
    table.insert(s1);
    REQUIRE(table.getSize() == 1);
    REQUIRE(table.contains(s1));
    REQUIRE_FALSE(table.contains(s2));
}

TEST_CASE("VariableTable getSize") {
    VariableTable table{};
    REQUIRE(table.getSize() == 0);

    table.insert("a");
    REQUIRE(table.getSize() == 1);

    table.insert("a");
    REQUIRE(table.getSize() == 1);
}

TEST_CASE("VariableTable getAllVars") {
    VariableTable table{};
    std::vector<VAR_NAME> expected{};
    REQUIRE(expected == table.getAllVars());

    table.insert("b");
    table.insert("a");
    expected = { VAR_NAME{"a"}, VAR_NAME{"b"} };
    REQUIRE(expected == table.getAllVars());
}

TEST_CASE("ReationshipRow ==") {
    PKBField field1 = PKBField::createConcrete(VAR_NAME{ "test" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "test" });
    PKBField field3 = PKBField::createConcrete(VAR_NAME{ "proc_test" });
    RelationshipRow row1{ field1, field2 };
    RelationshipRow row2{ field1, field2 };
    RelationshipRow row3{ field1, field3 };

    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);

    REQUIRE_FALSE(row1 == row3);
}

TEST_CASE("ReationshipRow getFirst getSecond") {
    PKBField field1 = PKBField::createConcrete(VAR_NAME{ "test" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "test" });
    PKBField field3 = PKBField::createConcrete(VAR_NAME{ "proc_test" });
    RelationshipRow row1{ field1, field2 };
    RelationshipRow row2{ field1, field2 };

    REQUIRE(field1 == row1.getFirst());
    REQUIRE(field1 == row1.getSecond());
    REQUIRE(row1.getFirst() == row2.getFirst());
    REQUIRE(row1.getSecond() == row2.getSecond());
}

TEST_CASE("ModifiesRelationshipTable::getType") {
    auto table = std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
    REQUIRE(table->getType() == PKBRelationship::MODIFIES);
}

TEST_CASE("ModifiesRelationshipTable::insert, ModifiesRelationshipTable::contains valid") {
    auto table = std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
    PKBField field1 = PKBField::createConcrete(PROC_NAME{ "main" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field3 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    table->insert(field4, field2);
    REQUIRE(table->contains(field1, field2));
    REQUIRE(table->contains(field1, field3));
    REQUIRE(table->contains(field4, field2));
}

TEST_CASE("ModifiesRelationshipTable::insert, ModifiesRelationshipTable::contains invalid") {
    auto table = std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
    PKBField validStmt = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField invalidStmt = PKBField::createConcrete(STMT_LO{ 2 });
    PKBField field1 = PKBField::createConcrete(VAR_NAME{ "invalid" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    REQUIRE_FALSE(table->contains(field1, field2));
    REQUIRE(table->getSize() == 0);

    PKBField field3 = PKBField::createConcrete(STMT_LO{ 1 });
    table->insert(field3, field2);
    REQUIRE_FALSE(table->contains(field3, field2));
    REQUIRE(table->getSize() == 0);

    PKBField field4 = PKBField::createDeclaration(PKBEntityType::VARIABLE);
    table->insert(validStmt, field4);
    REQUIRE_FALSE(table->contains(validStmt, field4));
    REQUIRE(table->getSize() == 0);

    PKBField field5 = PKBField::createDeclaration(PKBEntityType::PROCEDURE);
    table->insert(field5, field2);
    REQUIRE_FALSE(table->contains(field5, field2));
    REQUIRE(table->getSize() == 0);

    table->insert(invalidStmt, field2);
    REQUIRE_FALSE(table->contains(invalidStmt, field2));
    REQUIRE(table->getSize() == 0);
}

TEST_CASE("ModifiesRelationshipTable::getSize") {
    auto table = std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
    PKBField field1 = PKBField::createConcrete(PROC_NAME{ "main" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field3 = PKBField::createConcrete(VAR_NAME{ "a" });
    REQUIRE(table->getSize() == 0);

    table->insert(field1, field2);
    REQUIRE(table->getSize() == 1);
}


TEST_CASE("UsesRelationshipTable::getType") {
    auto table = std::unique_ptr<UsesRelationshipTable>(new UsesRelationshipTable());
    REQUIRE(table->getType() == PKBRelationship::USES);
}

TEST_CASE("UsesRelationshipTable::insert, UsesRelationshipTable::contains valid") {
    auto table = std::unique_ptr<UsesRelationshipTable>(new UsesRelationshipTable());
    PKBField field1 = PKBField::createConcrete(PROC_NAME{ "main" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field3 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    table->insert(field4, field2);
    REQUIRE(table->contains(field1, field2));
    REQUIRE(table->contains(field1, field3));
    REQUIRE(table->contains(field4, field2));
}

TEST_CASE("UsesRelationshipTable::insert, UsesRelationshipTable::contains invalid") {
    auto table = std::unique_ptr<UsesRelationshipTable>(new UsesRelationshipTable());
    PKBField validStmt = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField invalidStmt = PKBField::createConcrete(STMT_LO{ 2 });
    PKBField field1 = PKBField::createConcrete(VAR_NAME{ "invalid" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    REQUIRE_FALSE(table->contains(field1, field2));
    REQUIRE(table->getSize() == 0);

    PKBField field3 = PKBField::createConcrete(STMT_LO{ 1 });
    table->insert(field3, field2);
    REQUIRE_FALSE(table->contains(field3, field2));
    REQUIRE(table->getSize() == 0);

    PKBField field4 = PKBField::createDeclaration(PKBEntityType::VARIABLE);
    table->insert(validStmt, field4);
    REQUIRE_FALSE(table->contains(validStmt, field4));
    REQUIRE(table->getSize() == 0);

    PKBField field5 = PKBField::createDeclaration(PKBEntityType::PROCEDURE);
    table->insert(field5, field2);
    REQUIRE_FALSE(table->contains(field5, field2));
    REQUIRE(table->getSize() == 0);

    table->insert(invalidStmt, field2);
    REQUIRE_FALSE(table->contains(invalidStmt, field2));
    REQUIRE(table->getSize() == 0);
}

TEST_CASE("UsesRelationshipTable regression test") {
    SECTION("UsesRelationshipTable regression test #140") {
        auto table = std::unique_ptr<UsesRelationshipTable>(new UsesRelationshipTable());
        table->retrieve(PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE));

        PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Print });
        PKBField field2 = PKBField::createConcrete(VAR_NAME("x"));
        table->insert(field1, field2);
        REQUIRE(table->contains(field1, field2));
    }

    SECTION("UsesRelationshipTable regression test #142") {
        auto table = std::unique_ptr<UsesRelationshipTable>(new UsesRelationshipTable());
        PKBField stmt1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Print });
        PKBField stmt2 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
        PKBField var1 = PKBField::createConcrete(VAR_NAME{ "x" });
        PKBField var2 = PKBField::createConcrete(VAR_NAME{ "y" });
        PKBField proc = PKBField::createConcrete(PROC_NAME{ "main" });

        table->insert(stmt1, var1);
        table->insert(stmt2, var2);
        table->insert(proc, var2);
        REQUIRE(table->retrieve(stmt1, var1) == FieldRowResponse{ {stmt1, var1} });
        REQUIRE(table->retrieve(stmt2, var2) == FieldRowResponse{ {stmt2, var2} });
        REQUIRE(table->retrieve(proc, var2) == FieldRowResponse{ {proc,var2} });
        REQUIRE(table->retrieve(PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::PROCEDURE)) == FieldRowResponse{ });
    }
}