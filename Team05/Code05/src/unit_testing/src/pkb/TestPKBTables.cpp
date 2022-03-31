#include "PKB/PKBTables.h"
#include "PKB/PKBRelationshipTables.h"
#include "catch.hpp"

TEST_CASE("EntityRow::getVal") {
    EntityRow<CONST> constRow{ 1 };
    EntityRow<STMT_LO> stmtRow{ STMT_LO(1, StatementType::Assignment) };
    EntityRow<VAR_NAME> varRow{ VAR_NAME("monke") };
    EntityRow<PROC_NAME> procRow{ PROC_NAME("monkeProc") };

    REQUIRE(constRow.getVal() == 1);
    REQUIRE(stmtRow.getVal() == STMT_LO(1, StatementType::Assignment));
    REQUIRE(varRow.getVal() == VAR_NAME("monke"));
    REQUIRE(procRow.getVal() == PROC_NAME("monkeProc"));
}

TEST_CASE("EntityRow test operators") {
    EntityRow<CONST> constRow1{ 1 };
    EntityRow<STMT_LO> stmtRow1{ STMT_LO(1, StatementType::Assignment) };
    EntityRow<VAR_NAME> varRow1{ VAR_NAME("monke") };
    EntityRow<PROC_NAME> procRow1{ PROC_NAME("monkeProc") };

    EntityRow<CONST> constRow2{ 1 };
    EntityRow<STMT_LO> stmtRow2{ STMT_LO(1, StatementType::Assignment) };
    EntityRow<VAR_NAME> varRow2{ VAR_NAME("monke") };
    EntityRow<PROC_NAME> procRow2{ PROC_NAME("monkeProc") };

    EntityRow<CONST> constRow3{ 2 };

    REQUIRE(constRow1 == constRow2);
    REQUIRE(stmtRow1 == stmtRow2);
    REQUIRE(varRow1 == varRow2);
    REQUIRE(procRow1 == procRow2);

    REQUIRE_FALSE(constRow1 == varRow2);
    REQUIRE_FALSE(constRow1 == constRow3);
}

template <typename T>
bool hasSameContents(std::vector<T>* v1, std::vector<T>* v2) {
    sort(v1->begin(), v1->end());
    sort(v2->begin(), v2->end());
    return *v1 == *v2;
}
TEST_CASE("ConstantTable") {
    ConstantTable* constTable = new ConstantTable();
    std::vector<CONST> expected{};
    REQUIRE(constTable->getAllEntity() == expected);

    constTable->insert(5);
    constTable->insert(6);

    REQUIRE(constTable->contains(5));
    REQUIRE_FALSE(constTable->contains(7));

    expected = { CONST{5}, CONST{6} };
    auto actual = constTable->getAllEntity();
    REQUIRE(hasSameContents<CONST>(&actual, &expected));
    REQUIRE(constTable->getSize() == 2);
}

TEST_CASE("ProcedureTable") {
    ProcedureTable* procTable = new ProcedureTable();
    REQUIRE(procTable->getAllEntity() == std::vector<PROC_NAME>());

    procTable->insert(PROC_NAME{ "monke1" });
    procTable->insert(PROC_NAME{ "monke2" });

    REQUIRE(procTable->contains(PROC_NAME{ "monke1" }));
    REQUIRE_FALSE(procTable->contains(PROC_NAME{ "monke3" }));

    std::vector<PROC_NAME> expected = std::vector<PROC_NAME>{ PROC_NAME("monke1"), PROC_NAME("monke2") };
    auto actual = procTable->getAllEntity();
    std::sort(expected.begin(), expected.end());
    std::sort(actual.begin(), actual.end());
    REQUIRE(hasSameContents<PROC_NAME>(&actual, &expected));
    REQUIRE(procTable->getSize() == 2);
}

TEST_CASE("VariableTable") {
    VariableTable* varTable = new VariableTable();
    REQUIRE(varTable->getAllEntity() == std::vector<VAR_NAME>());

    varTable->insert(VAR_NAME{ "monke1" });
    varTable->insert(VAR_NAME{ "monke2" });

    REQUIRE(varTable->contains(VAR_NAME{ "monke1" }));
    REQUIRE_FALSE(varTable->contains(VAR_NAME{ "monke3" }));

    std::vector<VAR_NAME> expected = std::vector<VAR_NAME>{ VAR_NAME("monke1"), VAR_NAME("monke2") };
    auto actual = varTable->getAllEntity();
    std::sort(expected.begin(), expected.end());
    std::sort(actual.begin(), actual.end());
    REQUIRE(hasSameContents<VAR_NAME>(&actual, &expected));
    REQUIRE(varTable->getSize() == 2);
}

TEST_CASE("StatementTable") {
    StatementTable* stmtTable = new StatementTable();
    REQUIRE(stmtTable->getAllEntity() == std::vector<STMT_LO>());

    STMT_LO stmt1{ 1, StatementType::Assignment };
    STMT_LO stmt2{ 2, StatementType::Call, "foo" };
    STMT_LO stmt3{ 3, StatementType::Print, "x" };
    STMT_LO stmt4{ 4, StatementType::Assignment };

    STMT_LO stmt5{ 5, StatementType::Read, "y" };
    STMT_LO stmt6{ 3, StatementType::If };
    STMT_LO stmt7{ 4, StatementType::Print, "z" };

    stmtTable->insert(STMT_LO{ 1, StatementType::Assignment });
    stmtTable->insert(STMT_LO{ 2, StatementType::Call, "foo" });
    stmtTable->insert(STMT_LO{ 3, StatementType::Print, "x" });
    stmtTable->insert(STMT_LO{ 4, StatementType::Assignment });

    REQUIRE(stmtTable->contains(1));
    REQUIRE(stmtTable->contains(StatementType::Assignment, 1));
    REQUIRE_FALSE(stmtTable->contains(5));
    REQUIRE_FALSE(stmtTable->contains(StatementType::If, 3));
    REQUIRE_FALSE(stmtTable->contains(StatementType::Print, 4));

    std::vector<STMT_LO> expected = std::vector<STMT_LO>{ stmt1, stmt2, stmt3, stmt4 };
    auto actual = stmtTable->getAllEntity();
    std::sort(expected.begin(), expected.end());
    std::sort(actual.begin(), actual.end());
    REQUIRE(hasSameContents<STMT_LO>(&actual, &expected));

    actual = stmtTable->getStmtOfType(StatementType::Assignment);
    expected = std::vector<STMT_LO>{ stmt1, stmt4 };
    REQUIRE(hasSameContents<STMT_LO>(&actual, &expected));

    REQUIRE(stmtTable->getSize() == 4);
    stmtTable->insert(STMT_LO{4, StatementType::Print });
    REQUIRE(stmtTable->getSize() == 4);
    REQUIRE_FALSE(stmtTable->contains(StatementType::Print, 4));
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

    auto asdas = table->retrieveT(assnDecl, whileDecl);
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

TEST_CASE("CallsRelationshipTable::getType") {
    auto table = std::unique_ptr<CallsRelationshipTable>(new CallsRelationshipTable());
    REQUIRE(table->getType() == PKBRelationship::CALLS);
}

TEST_CASE("CallsRelationshipTable test 1") {
    auto table = std::unique_ptr<CallsRelationshipTable>(new CallsRelationshipTable());

    /*
    procedure main {
        call foo;
        call bar;
    }

    procedure foo {
        call foo_sub;
    }

    procedure bar {
        call bar_sub;
    }
    */
    PKBField field1 = PKBField::createConcrete(PROC_NAME{ "main" });
    PKBField field2 = PKBField::createConcrete(PROC_NAME{ "foo" });
    PKBField field3 = PKBField::createConcrete(PROC_NAME{ "bar" });
    PKBField field4 = PKBField::createConcrete(PROC_NAME{ "foo_sub" });
    PKBField field5 = PKBField::createConcrete(PROC_NAME{ "bar_sub" });

    // Empty table
    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    table->insert(field1, field3);
    table->insert(field2, field4);
    table->insert(field3, field5);

    PKBField invalid_1 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField invalid_2 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField invalid_3 = PKBField::createDeclaration(PKBEntityType::PROCEDURE);
    PKBField invalid_4 = PKBField::createDeclaration(StatementType::Assignment);
    PKBField decl1 = PKBField::createDeclaration(PKBEntityType::PROCEDURE);
    PKBField decl2 = PKBField::createDeclaration(StatementType::Assignment);
    PKBField decl3 = PKBField::createDeclaration(PKBEntityType::VARIABLE);
    PKBField wild1 = PKBField::createWildcard(PKBEntityType::PROCEDURE);
    PKBField wild2 = PKBField::createWildcard(PKBEntityType::STATEMENT);
    PKBField wild3 = PKBField::createWildcard(PKBEntityType::VARIABLE);

    SECTION("CallsRelationshipTable::insert, CallsRelationshipTable::contains") {
        REQUIRE(table->contains(field1, field2));
        REQUIRE(table->contains(field1, field3));
        REQUIRE(table->contains(field2, field4));
        REQUIRE(table->contains(field3, field5));

        // Both fields must be the same types
        table->insert(field1, invalid_1);
        REQUIRE_FALSE(table->contains(field1, invalid_1));
        table->insert(field1, invalid_2);
        REQUIRE_FALSE(table->contains(field1, invalid_2));
        table->insert(field1, invalid_3);
        REQUIRE_FALSE(table->contains(field1, invalid_3));
        table->insert(field1, invalid_4);
        REQUIRE_FALSE(table->contains(field1, invalid_4));

        // Both fields are the same type but are not PROC_NAMEs
        table->insert(invalid_2, invalid_2);
        REQUIRE_FALSE(table->contains(invalid_2, invalid_2));

        REQUIRE_FALSE(table->contains(field1, decl1));
        REQUIRE_FALSE(table->contains(field1, wild1));
    }

    SECTION("CallsRelationshipTable::retrieve") {
        REQUIRE(table->retrieve(field1, field2) == FieldRowResponse{ {field1, field2} });
        REQUIRE(table->retrieve(field1, field4) == FieldRowResponse{ });
        REQUIRE(table->retrieve(field1, decl1) == FieldRowResponse{ {field1, field2}, {field1, field3} });
        REQUIRE(table->retrieve(field1, wild1) == FieldRowResponse{ {field1, field2}, {field1, field3} });
        REQUIRE(table->retrieve(field2, decl1) == FieldRowResponse{ {field2, field4} });
        REQUIRE(table->retrieve(decl1, field4) == FieldRowResponse{ {field2, field4} });
        REQUIRE(table->retrieve(decl1, decl1) == FieldRowResponse{ {field1, field2}, {field1, field3},
            {field2, field4}, {field3, field5} });
        REQUIRE(table->retrieve(decl1, decl1) == FieldRowResponse{ {field1, field2}, {field1, field3},
            {field2, field4}, {field3, field5} });
        REQUIRE(table->retrieve(wild1, wild1) == FieldRowResponse{ {field1, field2}, {field1, field3},
            {field2, field4}, {field3, field5} });

        // Invalid queries
        REQUIRE(table->retrieve(field1, decl2) == FieldRowResponse{ });
        REQUIRE(table->retrieve(decl3, field4) == FieldRowResponse{ });
        REQUIRE(table->retrieve(wild3, field4) == FieldRowResponse{ });
        REQUIRE(table->retrieve(decl2, decl2) == FieldRowResponse{ });
        REQUIRE(table->retrieve(wild2, wild2) == FieldRowResponse{ });
    }

    SECTION("CallsRelationshipTable::containsT") {
        REQUIRE(table->containsT(field1, field2));
        REQUIRE(table->containsT(field1, field3));
        REQUIRE(table->containsT(field2, field4));
        REQUIRE(table->containsT(field3, field5));
        REQUIRE(table->containsT(field1, field4));
        REQUIRE(table->containsT(field1, field5));

        REQUIRE_FALSE(table->containsT(field2, field5));
        REQUIRE_FALSE(table->containsT(field1, decl1));
        REQUIRE_FALSE(table->containsT(field1, wild1));
    }

    SECTION("CallsRelationshipTable::retrieveT") {
        REQUIRE(table->retrieveT(field1, field2) == FieldRowResponse{ {field1, field2} });
        REQUIRE(table->retrieveT(field1, decl1) == FieldRowResponse{ {field1, field2}, {field1, field3},
            {field1, field4}, {field1, field5} });
        REQUIRE(table->retrieveT(field1, wild1) == FieldRowResponse{ {field1, field2}, {field1, field3},
            {field1, field4}, {field1, field5} });
        REQUIRE(table->retrieveT(field2, decl1) == FieldRowResponse{ {field2, field4} });
        REQUIRE(table->retrieveT(decl1, field4) == FieldRowResponse{ {field1, field4}, {field2, field4} });
        REQUIRE(table->retrieveT(decl1, decl1) == FieldRowResponse{ {field1, field2}, {field1, field3},
            {field1, field4}, {field1, field5}, {field2, field4}, {field3, field5} });
        REQUIRE(table->retrieveT(decl1, decl1) == FieldRowResponse{ {field1, field2}, {field1, field3},
            {field1, field4}, {field1, field5}, {field2, field4}, {field3, field5} });
        REQUIRE(table->retrieveT(wild1, wild1) == FieldRowResponse{ {field1, field2}, {field1, field3},
            {field1, field4}, {field1, field5}, {field2, field4}, {field3, field5} });

        // Invalid queries
        REQUIRE(table->retrieveT(field1, decl2) == FieldRowResponse{ });
        REQUIRE(table->retrieveT(decl3, field4) == FieldRowResponse{ });
        REQUIRE(table->retrieveT(wild3, field4) == FieldRowResponse{ });
        REQUIRE(table->retrieveT(decl2, decl2) == FieldRowResponse{ });
        REQUIRE(table->retrieveT(wild2, wild2) == FieldRowResponse{ });
    }
}

TEST_CASE("AffectsEvaluator test") {
    /**
    *   procedure main {
    * 1   x = 2 + a;
    * 2   y = x + 3;
    * 3   if (b == 3) then {
    * 4     while (x == 2) {
    * 5        x = x + 1;
    *       }
    *     } else {
    * 6     a = x + c;
    *     }
    * 7   print x;
    * 8   a = x - a;
    * 9   y = a + x;
    *   }
    */
    StatementType ASSIGN = StatementType::Assignment;
    StatementType IF = StatementType::If;
    StatementType WHILE = StatementType::While;
    StatementType PRINT = StatementType::Print;

    auto affEval = std::unique_ptr<AffectsEvaluator>(new AffectsEvaluator());
    auto root = std::make_shared<sp::cfg::CFGNode>();
    auto stmt1 = std::make_shared<sp::cfg::CFGNode>(1, ASSIGN);
    auto stmt2 = std::make_shared<sp::cfg::CFGNode>(2, ASSIGN);
    auto stmt3 = std::make_shared<sp::cfg::CFGNode>(3, IF);
    auto stmt4 = std::make_shared<sp::cfg::CFGNode>(4, WHILE);
    auto stmt5 = std::make_shared<sp::cfg::CFGNode>(5, ASSIGN);
    auto stmt6 = std::make_shared<sp::cfg::CFGNode>(6, ASSIGN);
    auto stmt7 = std::make_shared<sp::cfg::CFGNode>(7, PRINT);
    auto dummy = std::make_shared<sp::cfg::CFGNode>();
    auto stmt8 = std::make_shared<sp::cfg::CFGNode>(8, ASSIGN);
    auto stmt9 = std::make_shared<sp::cfg::CFGNode>(9, ASSIGN);
    auto exit = std::make_shared<sp::cfg::CFGNode>();
    
    VAR_NAME x("x");
    VAR_NAME a{ "a" };
    VAR_NAME b{ "b" };
    VAR_NAME c{ "c" };
    VAR_NAME y{ "y" };

    root->insert(stmt1);
    
    stmt1->insert(stmt2);
    stmt1->modifies.emplace(stmt1->stmt.value(), std::unordered_set<VAR_NAME>({x}));
    stmt1->uses.emplace(stmt1->stmt.value(), std::unordered_set<VAR_NAME>({ a }));

    stmt2->insert(stmt3);
    stmt2->modifies.emplace(stmt2->stmt.value(), std::unordered_set<VAR_NAME>({ y }));
    stmt2->uses.emplace(stmt2->stmt.value(), std::unordered_set<VAR_NAME>({ x }));

    stmt3->insert(stmt4);
    stmt3->insert(stmt6);
    stmt3->uses.emplace(stmt3->stmt.value(), std::unordered_set<VAR_NAME>({ b }));
    
    stmt4->insert(stmt5);
    stmt4->insert(dummy);
    stmt4->uses.emplace(stmt4->stmt.value(), std::unordered_set<VAR_NAME>({ x }));

    stmt5->insert(stmt4);
    stmt5->modifies.emplace(stmt5->stmt.value(), std::unordered_set<VAR_NAME>({ x }));
    stmt5->uses.emplace(stmt5->stmt.value(), std::unordered_set<VAR_NAME>({ x }));

    stmt6->insert(dummy);
    stmt6->modifies.emplace(stmt6->stmt.value(), std::unordered_set<VAR_NAME>({ a }));
    stmt6->uses.emplace(stmt6->stmt.value(), std::unordered_set<VAR_NAME>({ x, c }));

    dummy->insert(stmt7);
    stmt7->insert(stmt8);
    stmt7->uses.emplace(stmt7->stmt.value(), std::unordered_set<VAR_NAME>({ x }));

    stmt8->insert(stmt9);
    stmt8->modifies.emplace(stmt8->stmt.value(), std::unordered_set<VAR_NAME>({ a }));
    stmt8->uses.emplace(stmt8->stmt.value(), std::unordered_set<VAR_NAME>({ x, a }));

    stmt9->insert(exit);
    stmt9->modifies.emplace(stmt9->stmt.value(), std::unordered_set<VAR_NAME>({ y }));
    stmt9->uses.emplace(stmt9->stmt.value(), std::unordered_set<VAR_NAME>({ b, a }));

    ProcToCfgMap cfgRoots;
    cfgRoots.emplace("main", root);

    // Initialize affEval
    affEval->initCFG(cfgRoots);

    PKBField conc1 = PKBField::createConcrete(STMT_LO(1, ASSIGN));
    PKBField conc2 = PKBField::createConcrete(STMT_LO(8, ASSIGN));
    REQUIRE(affEval->contains(conc1, conc2));

    PKBField conc3 = PKBField::createConcrete(STMT_LO(5, ASSIGN));
    PKBField conc4 = PKBField::createConcrete(STMT_LO(5, ASSIGN));
    REQUIRE(affEval->contains(conc3, conc4));

    PKBField conc5 = PKBField::createConcrete(STMT_LO(2, ASSIGN));
    REQUIRE_FALSE(affEval->contains(conc5, conc3));

    PKBField conc6 = PKBField::createConcrete(STMT_LO(9, ASSIGN));

    // Currently not working due to issues with Graph<T> transitive traversal, 
    // unrelated to AffectsEval
    // REQUIRE(affEval->contains(conc1, conc6, true));
    // REQUIRE_FALSE(affEval->contains(conc1, conc6));
}
