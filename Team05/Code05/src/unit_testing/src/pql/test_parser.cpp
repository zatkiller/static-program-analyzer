#include "pql/parser.h"
#include "catch.hpp"
#include <memory>
#include "logging.h"
#include <vector>

#define TEST_LOG Logger() << "TestLexer.cpp"

TEST_CASE("Test Parser peekNext and getNext") {
    std::string testQuery = "assign a; \n Select a such that Uses (a, v) pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery);

    REQUIRE(parser.peekNextToken() == Token{"assign", TokenType::Identifier});
    REQUIRE(parser.peekAndCheckNextToken(TokenType::Identifier) == Token{"assign", TokenType::Identifier});

    REQUIRE(parser.getNextToken() == Token{"assign", TokenType::Identifier});
    REQUIRE(parser.getAndCheckNextToken(TokenType::Identifier) == Token{"a", TokenType::Identifier});
}

TEST_CASE("Test Parser addPql on generating new Lexer") {
    std::string testQuery1 = "assign a; \n Select a such that Uses (a, v) pattern a (v, _)";
    std::string testQuery2 = "variable v; \n Select v such that Uses (a, v) pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery1);

    Lexer lexer1 = parser.lexer;
    REQUIRE(parser.getParsedText() == testQuery1);

    parser.addPql(testQuery2);

    REQUIRE(parser.getParsedText() == testQuery2);

    REQUIRE(!(lexer1 == parser.lexer));
}

TEST_CASE("Test Parser parseDeclarations method on single Design Entity") {
    std::string testQuery = "assign a, a1; \n Select a such that Uses (a, v) pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);

    REQUIRE(queryObj.hasDeclaration("a"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a") == DesignEntity::ASSIGN);

    REQUIRE(queryObj.hasDeclaration("a"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a1") == DesignEntity::ASSIGN);
}

TEST_CASE("Test Parser parseDeclarations method on multiple Design Entity") {
    std::string testQuery = "assign a; variable v, v1;\n Select a such that Uses (a, v) pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);

    REQUIRE(queryObj.hasDeclaration("a"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a") == DesignEntity::ASSIGN);

    parser.parseDeclarations(queryObj);

    REQUIRE(queryObj.hasDeclaration("v"));
    REQUIRE(queryObj.getDeclarationDesignEntity("v") == DesignEntity::VARIABLE);

    REQUIRE(queryObj.hasDeclaration("v1"));
    REQUIRE(queryObj.getDeclarationDesignEntity("v1") == DesignEntity::VARIABLE);

    REQUIRE(queryObj.getVariable().size() == 0);
}

TEST_CASE("Test Parser parseSelectFields") {
    std::string testQuery = "assign a; \n Select a such that Uses (a, v) pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);

    REQUIRE(queryObj.hasVariable("a"));
    REQUIRE(queryObj.getSuchthat().size() == 0);
}

TEST_CASE("Test Parse such that for Uses") {
    std::string testQuery = "assign a; \n Select a such that Uses (a, _) pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);
    parser.parseSuchThat(queryObj);

    std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
    REQUIRE(relRefPtr->getType() == RelRefType::USESS);

    std::shared_ptr<Uses> usesPtr = std::dynamic_pointer_cast<Uses>(relRefPtr);
    REQUIRE(usesPtr->useStmt.declaration == "a");
    REQUIRE(usesPtr->useStmt.isDeclaration());
    REQUIRE(usesPtr->used.isWildcard());

    REQUIRE(queryObj.getPattern().size() == 0);
}

TEST_CASE("Test Parse such that for MOdifies") {
    std::string testQuery = "assign a; \n Select a such that Modifies (a, _) pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);
    parser.parseSuchThat(queryObj);

    std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
    REQUIRE(relRefPtr->getType() == RelRefType::MODIFIESS);

    std::shared_ptr<Modifies> modifiesPtr = std::dynamic_pointer_cast<Modifies>(relRefPtr);
    REQUIRE(modifiesPtr->modifiesStmt.declaration == "a");
    REQUIRE(modifiesPtr->modifiesStmt.isDeclaration());
    REQUIRE(modifiesPtr->modified.isWildcard());

    REQUIRE(queryObj.getPattern().size() == 0);
}
TEST_CASE("Test Parse pattern wildcard expression") {
    std::string testQuery = "assign a; \n Select a such that Modifies (a, _) pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);
    parser.parseSuchThat(queryObj);
    parser.parsePattern(queryObj);

    std::vector<Pattern> patterns = queryObj.getPattern();
    Pattern pattern = patterns[0];

    REQUIRE(pattern.getSynonym() == "a");
    bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().declaration == "v");
    REQUIRE(validDeclaration);
    REQUIRE(pattern.getExpression() == "_");

}

TEST_CASE("Test Parse pattern string") {
    std::string testQuery = "assign a; \n Select a such that Modifies (a, _) pattern a (v, \"x\")";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);
    parser.parseSuchThat(queryObj);
    parser.parsePattern(queryObj);

    std::vector<Pattern> patterns = queryObj.getPattern();
    Pattern pattern = patterns[0];

    REQUIRE(pattern.getSynonym() == "a");
    bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().declaration == "v");
    REQUIRE(validDeclaration);
    REQUIRE(pattern.getExpression() == "x");
}

TEST_CASE("Test Parse pattern string with wildcards") {
    std::string testQuery = "assign a; \n Select a such that Modifies (a, _) pattern a (v, _\"x\"_)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);
    parser.parseSuchThat(queryObj);
    parser.parsePattern(queryObj);

    std::vector<Pattern> patterns = queryObj.getPattern();
    Pattern pattern = patterns[0];

    REQUIRE(pattern.getSynonym() == "a");
    bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().declaration == "v");
    REQUIRE(validDeclaration);
    REQUIRE(pattern.getExpression() == "_x_");
}



