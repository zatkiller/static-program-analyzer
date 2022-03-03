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
        REQUIRE(table->retrieve(proc, var2) == FieldRowResponse{ {proc, var2} });
        REQUIRE(table->retrieve(PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::PROCEDURE)) == FieldRowResponse{ });
    }
}

TEST_CASE("FollowsRelationshipTable getType") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBRelationship type = table->getType();
    REQUIRE(type == PKBRelationship::FOLLOWS);
}

TEST_CASE("FollowsRelationshipTable::insert, FollowsRelationshipTable::contains") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Print });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Print });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Print });

    // Empty table
    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    table->insert(field2, field3);
    table->insert(field2, field4);
    table->insert(field4, field5);

    REQUIRE(table->contains(field1, field2));
    REQUIRE_FALSE(table->contains(field2, field1));
    REQUIRE(table->contains(field2, field3));
    REQUIRE_FALSE(table->contains(field1, field3));
    REQUIRE_FALSE(table->contains(field1, field5));

    // Insert of an existing field will not replace it
    PKBField duplicate = PKBField::createConcrete(STMT_LO{ 1, StatementType::While });
    table->insert(duplicate, field2);
    REQUIRE(table->contains(field1, field2));
    REQUIRE_FALSE(table->contains(duplicate, field2));

    // No inserting of Follows(u, v) where u > v. Contains will not need the check
    PKBField u = PKBField::createConcrete(STMT_LO{ 7, StatementType::Print });
    PKBField v = PKBField::createConcrete(STMT_LO{ 6, StatementType::Print });
    table->insert(u, v);
    REQUIRE_FALSE(table->contains(u, v));

    table->insert(v, u);
    REQUIRE_FALSE(table->contains(u, v));
    REQUIRE(table->contains(v, u));
}

TEST_CASE("FollowsRelationshipTable::retrieve") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Print });

    // Case 1: Both query fields are statement declarations
    PKBField decl1 = PKBField::createDeclaration(StatementType::All);
    PKBField decl2 = PKBField::createDeclaration(StatementType::If);
    PKBField decl3 = PKBField::createDeclaration(StatementType::Assignment);
    PKBField decl4 = PKBField::createDeclaration(StatementType::Print);

    // Empty table
    REQUIRE(table->retrieve(decl1, decl1) == FieldRowResponse{});

    table->insert(field1, field2);
    table->insert(field2, field3);

    REQUIRE(table->retrieve(decl1, decl1) == FieldRowResponse{ { {field1, field2}, { field2, field3 }} });

    // Follows(s, s), ordering check
    REQUIRE(table->retrieve(decl1, decl1) == FieldRowResponse{ { {field2, field3}, { field1, field2 }} });
    REQUIRE(table->retrieve(decl2, decl1) == FieldRowResponse{ {{field2, field3}} });
    REQUIRE(table->retrieve(decl3, decl4) == FieldRowResponse{});

    // Case 2: First query field is a declaration, second is concrete
    PKBField conc1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField conc2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField conc3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Print });
    PKBField conc4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Print });
    PKBField incompleteStatement = PKBField::createConcrete(STMT_LO{ 2 });

    REQUIRE(table->retrieve(decl1, conc2) == FieldRowResponse{ {{field1, field2}} });

    // Follows(s,2) where 2 has no type. Note: empty result is intended as type append only occurs in PKB
    REQUIRE(table->retrieve(decl1, incompleteStatement) == FieldRowResponse{});
    REQUIRE(table->retrieve(decl1, conc4) == FieldRowResponse{});
    REQUIRE(table->retrieve(decl2, conc3) == FieldRowResponse{ { {field2, field3}} });

    // Case 3: First query field is concrete, second is a declaration
    REQUIRE(table->retrieve(conc2, decl1) == FieldRowResponse{ { {field2, field3}} });
    REQUIRE(table->retrieve(conc2, decl3) == FieldRowResponse{});
    REQUIRE(table->retrieve(conc4, decl1) == FieldRowResponse{});
}

TEST_CASE("FollowsRelationshipTable::containsT") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::Assignment });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });

    REQUIRE_FALSE(table->containsT(field1, field3));

    table->insert(field1, field2);
    table->insert(field2, field3);

    REQUIRE(table->containsT(field1, field3));

    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::If });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });
    PKBField field6 = PKBField::createConcrete(STMT_LO{ 6, StatementType::Assignment });
    PKBField field7 = PKBField::createConcrete(STMT_LO{ 7, StatementType::Assignment });
    PKBField field8 = PKBField::createConcrete(STMT_LO{ 8, StatementType::Assignment });

    table->insert(field4, field7);
    table->insert(field5, field6);
    table->insert(field7, field8);

    REQUIRE(table->containsT(field4, field8));

    REQUIRE_FALSE(table->containsT(field4, field6));
}

TEST_CASE("FollowsRelationshipTable::retrieveT case 1") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::While });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Assignment });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::While });

    table->insert(field1, field2);
    table->insert(field3, field4);
    table->insert(field2, field5);

    REQUIRE(table->retrieveT(field1, PKBField::createDeclaration(StatementType::While)) ==
        FieldRowResponse{ {field1, field2}, {field1, field5} });
    REQUIRE(table->retrieveT(field2, PKBField::createDeclaration(StatementType::While)) ==
        FieldRowResponse{ {field2, field5} });
    REQUIRE(table->retrieveT(field1, PKBField::createDeclaration(StatementType::All)) ==
        FieldRowResponse{ {field1, field2}, {field1, field5} });
    REQUIRE(table->retrieveT(field5, PKBField::createDeclaration(StatementType::While)) ==
        FieldRowResponse{ });
    REQUIRE(table->retrieveT(field1, PKBField::createDeclaration(StatementType::Assignment)) ==
        FieldRowResponse{ });
    REQUIRE(table->retrieveT(field3, PKBField::createDeclaration(StatementType::Assignment)) ==
        FieldRowResponse{ {field3, field4} });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::Assignment),
        PKBField::createDeclaration(StatementType::Assignment)) ==
        FieldRowResponse{ {field3, field4} });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::If), field5) == FieldRowResponse{ });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::While), field1) == FieldRowResponse{ });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::While), field5) ==
        FieldRowResponse{ {field2, field5 } });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::All), field5) ==
        FieldRowResponse{ {field1, field5 }, {field2, field5 } });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::Assignment), field5) ==
        FieldRowResponse{ {field1, field5 } });
    REQUIRE(table->retrieveT(PKBField::createWildcard(PKBEntityType::STATEMENT),
        PKBField::createWildcard(PKBEntityType::STATEMENT)) ==
        FieldRowResponse{ {field1, field2}, {field1, field5},
            {field2, field5}, {field3, field4} });
}

// incomplete
TEST_CASE("FollowsRelationshipTable::retrieveT case 2") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::While });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::If });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });
    PKBField field6 = PKBField::createConcrete(STMT_LO{ 6, StatementType::Assignment });
    PKBField field7 = PKBField::createConcrete(STMT_LO{ 7, StatementType::While });
    PKBField field8 = PKBField::createConcrete(STMT_LO{ 8, StatementType::Assignment });
    PKBField field9 = PKBField::createConcrete(STMT_LO{ 9, StatementType::Assignment });

    table->insert(field1, field2);
    table->insert(field2, field9);
    table->insert(field3, field4);
    table->insert(field5, field6);

    PKBField ifDecl = PKBField::createDeclaration(StatementType::If);
    PKBField whileDecl = PKBField::createDeclaration(StatementType::While);
    PKBField assnDecl = PKBField::createDeclaration(StatementType::Assignment);
    PKBField allDecl = PKBField::createDeclaration(StatementType::All);

    // Case 1: Both declarations
    // Follows*(s, s)
    REQUIRE(table->retrieveT(allDecl, allDecl) == FieldRowResponse{ {
            {field1, field2},
            {field2, field9},
            {field3, field4},
            {field5, field6},
            {field1, field9}} });

    REQUIRE(table->retrieveT(assnDecl, whileDecl) == FieldRowResponse{ { {field1, field2}} });
    REQUIRE(table->retrieveT(whileDecl, whileDecl) == FieldRowResponse{ });
    REQUIRE(table->retrieveT(assnDecl, ifDecl) == FieldRowResponse{ {field3, field4} });

    // Case 2: First field concrete
    PKBField conc1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    REQUIRE(table->retrieveT(conc1, allDecl) == FieldRowResponse{ { {field1, field2}, { field1, field9 }} });
    PKBField invalidConc = PKBField::createConcrete(STMT_LO{ 1, StatementType::If });

    // Follows*(1, a) where the statement type provided does not match the one stored
    REQUIRE(table->retrieveT(invalidConc, allDecl) == FieldRowResponse{ });

    PKBField conc2 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    REQUIRE(table->retrieve(conc2, allDecl) == FieldRowResponse{ {{field3, field4}} });
    REQUIRE(table->retrieveT(conc2, whileDecl) == FieldRowResponse{ });

    // Case 3: Second field concrete
    PKBField conc3 = PKBField::createConcrete(STMT_LO{ 9, StatementType::Assignment });
    REQUIRE(table->retrieveT(allDecl, conc3) == FieldRowResponse{ {{field2, field9}, {field1, field9}} });

    PKBField conc4 = PKBField::createConcrete(STMT_LO{ 7, StatementType::While });
    REQUIRE(table->retrieveT(allDecl, conc4) == FieldRowResponse{ });

    PKBField conc5 = PKBField::createConcrete(STMT_LO{ 4, StatementType::If });
    REQUIRE(table->retrieveT(assnDecl, conc5) == FieldRowResponse{ {{field3, field4}} });

    // Case 4: Both 
    REQUIRE(table->retrieveT(conc1, conc3) == FieldRowResponse{ {{field1, field9}} });
    REQUIRE(table->retrieveT(conc5, conc4) == FieldRowResponse{ });
}


TEST_CASE("ParentRelationshipTable::getType") {
    auto table = std::unique_ptr<ParentRelationshipTable>(new ParentRelationshipTable());
    REQUIRE(table->getType() == PKBRelationship::PARENT);
}

TEST_CASE("ParentRelationshipTable::insert, ParentRelationshipTable::contains") {
    auto table = std::unique_ptr<ParentRelationshipTable>(new ParentRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::If });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::While });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Call });

    // Empty table
    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    table->insert(field2, field3);
    table->insert(field2, field4);

    REQUIRE(table->contains(field2, field3));
    REQUIRE(table->contains(field1, field2));
    REQUIRE(table->contains(field2, field4));

    // Handles duplicate of existing field
    PKBField duplicate = PKBField::createConcrete(STMT_LO{ 1, StatementType::While });
    table->insert(duplicate, field2);
    REQUIRE(table->contains(field1, field2));
    REQUIRE_FALSE(table->contains(duplicate, field2));

    // No inserting of u->v where u is not a container stmt
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Print });
    PKBField field6 = PKBField::createConcrete(STMT_LO{ 6, StatementType::Print });
    table->insert(field5, field6);
    REQUIRE_FALSE(table->contains(field5, field6));

    // No inserting of u->v where v comes after u
    PKBField field7 = PKBField::createConcrete(STMT_LO{ 7,  StatementType::Print });
    PKBField field8 = PKBField::createConcrete(STMT_LO{ 8, StatementType::While });
    table->insert(field8, field7);
    REQUIRE_FALSE(table->contains(field8, field7));
}

TEST_CASE("ParentRelationshipTable::retrieve") {
    auto table = std::unique_ptr<ParentRelationshipTable>(new ParentRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::While });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Call });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::If });
    PKBField field6 = PKBField::createConcrete(STMT_LO{ 6, StatementType::Assignment });
    PKBField field7 = PKBField::createConcrete(STMT_LO{ 7, StatementType::Assignment });
    PKBField field8 = PKBField::createConcrete(STMT_LO{ 8, StatementType::While });
    PKBField field9 = PKBField::createConcrete(STMT_LO{ 9, StatementType::Assignment });
    PKBField field10 = PKBField::createConcrete(STMT_LO{ 10, StatementType::Assignment });


    // Case 1: Both query fields are statement declarations
    PKBField decl1 = PKBField::createDeclaration(StatementType::All);
    PKBField decl2 = PKBField::createDeclaration(StatementType::If);
    PKBField decl3 = PKBField::createDeclaration(StatementType::Assignment);
    PKBField decl4 = PKBField::createDeclaration(StatementType::Call);
    PKBField decl5 = PKBField::createDeclaration(StatementType::While);

    REQUIRE(table->retrieve(decl1, decl1) == FieldRowResponse{});

    table->insert(field1, field2);
    table->insert(field2, field3);
    table->insert(field2, field4);
    table->insert(field2, field5);
    table->insert(field2, field8);
    table->insert(field2, field10);
    table->insert(field8, field9);
    table->insert(field5, field6);
    table->insert(field5, field7);

    FieldRowResponse expected1{
        {
            {field1, field2},
            {field2, field3},
            {field2, field4},
            {field2, field5},
            {field2, field8},
            {field8, field9},
            {field5, field6},
            {field5, field7},
            {field2, field10}
        }
    };
    REQUIRE(table->retrieve(decl1, decl1) == expected1);

    FieldRowResponse expected2{ {{field8, field9}} };
    REQUIRE(table->retrieve(decl5, decl3) == expected2);


    // Case 2: First field is a declaration, second is concrete
    PKBField conc1 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField conc2 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Call });
    PKBField conc3 = PKBField::createConcrete(STMT_LO{ 8, StatementType::While });
    PKBField conc4 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField incompleteStmt = PKBField::createConcrete(STMT_LO{ 5 });

    FieldRowResponse expected3{ {{field2, field3}} };
    REQUIRE(table->retrieve(decl2, conc1) == expected3);

    REQUIRE(table->retrieve(decl1, incompleteStmt) == FieldRowResponse{});
    REQUIRE(table->retrieve(decl5, conc2) == FieldRowResponse{});

    FieldRowResponse expected4{ {{field2, field8}} };
    REQUIRE(table->retrieve(decl1, conc3) == expected4);

    // Case 3: First field is concrete, second is a declaration
    FieldRowResponse expected5{ {{field2, field3}, {field2, field10}} };
    REQUIRE(table->retrieve(conc4, decl3) == expected5);
    REQUIRE(table->retrieve(conc2, decl1) == FieldRowResponse{});
    REQUIRE(table->retrieve(conc3, decl5) == FieldRowResponse{});
    REQUIRE(table->retrieve(incompleteStmt, decl3) == FieldRowResponse{});
}

TEST_CASE("ParentRelationshipTable::containsT") {
    auto table = std::unique_ptr<ParentRelationshipTable>(new ParentRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::While });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Call });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::If });
    PKBField field6 = PKBField::createConcrete(STMT_LO{ 6, StatementType::Assignment });
    PKBField field7 = PKBField::createConcrete(STMT_LO{ 7, StatementType::Assignment });
    PKBField field8 = PKBField::createConcrete(STMT_LO{ 8, StatementType::While });
    PKBField field9 = PKBField::createConcrete(STMT_LO{ 9, StatementType::Assignment });
    PKBField field10 = PKBField::createConcrete(STMT_LO{ 10, StatementType::Assignment });

    // Empty table
    REQUIRE_FALSE(table->containsT(field1, field1));

    table->insert(field1, field2);
    table->insert(field2, field3);
    table->insert(field2, field4);
    table->insert(field2, field5);
    table->insert(field2, field8);
    table->insert(field2, field10);
    table->insert(field8, field9);
    table->insert(field5, field6);
    table->insert(field5, field7);

    REQUIRE(table->containsT(field1, field7));
    REQUIRE(table->containsT(field2, field9));
    REQUIRE_FALSE(table->containsT(field8, field7));
    REQUIRE_FALSE(table->containsT(field7, field1));
}

TEST_CASE("ParentRelationshipTable::retrieveT") {
    auto table = std::unique_ptr<ParentRelationshipTable>(new ParentRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::While });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Call });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Print });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });

    // Empty Table
    REQUIRE(table->retrieveT(field1, field2) == FieldRowResponse{});

    table->insert(field1, field2);
    table->insert(field2, field3);
    table->insert(field2, field4);
    table->insert(field2, field5);

    // Case 1: Both fields declarations
    PKBField decl1 = PKBField::createDeclaration(StatementType::All);
    PKBField decl2 = PKBField::createDeclaration(StatementType::While);
    PKBField decl3 = PKBField::createDeclaration(StatementType::Print);
    PKBField decl4 = PKBField::createDeclaration(StatementType::If);
    PKBField decl5 = PKBField::createDeclaration(StatementType::Call);

    PKBField conc1 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Print });
    PKBField conc2 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });
    PKBField conc3 = PKBField::createConcrete(STMT_LO{ 1, StatementType::While });
    PKBField conc4 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField invalidConc = PKBField::createConcrete(STMT_LO{ 1, StatementType::If });

    FieldRowResponse expected1{
        {
            {field1, field2},
            {field1, field3},
            {field1, field4},
            {field1, field5},
            {field2, field3},
            {field2, field4},
            {field2, field5}
        }
    };
    REQUIRE(table->retrieveT(decl1, decl1) == expected1);

    FieldRowResponse expected2{ {{field1, field4}} };
    REQUIRE(table->retrieveT(decl2, decl3) == expected2);

    REQUIRE(table->retrieveT(decl2, decl2) == FieldRowResponse{});


    // Case 2: First field declaration, second field concrete
    FieldRowResponse expected3{ {{field1, field4}, {field2, field4}} };
    REQUIRE(table->retrieveT(decl1, conc1) == expected3);

    FieldRowResponse expected4{ {{field2, field5}} };
    REQUIRE(table->retrieveT(decl4, conc2) == expected4);

    REQUIRE(table->retrieveT(decl1, conc3) == FieldRowResponse{});
    REQUIRE(table->retrieve(decl1, invalidConc) == FieldRowResponse{});


    // Case 3: First field concrete, second field declaration
    FieldRowResponse expected5{
        {
            {field1, field2},
            {field1, field3},
            {field1, field4},
            {field1, field5}
        }
    };
    REQUIRE(table->retrieveT(conc3, decl1) == expected5);

    FieldRowResponse expected6{ {{field2, field3}} };
    REQUIRE(table->retrieveT(conc4, decl5) == expected6);

    REQUIRE(table->retrieveT(conc3, decl2) == FieldRowResponse{});
    REQUIRE(table->retrieveT(invalidConc, decl1) == FieldRowResponse{});
}
