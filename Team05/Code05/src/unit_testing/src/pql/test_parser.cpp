#include "pql/parser.h"
#include "catch.hpp"
#include <memory>
#include <vector>
#include "logging.h"

TEST_CASE("Parser checkType") {
    Parser parser;
    Token token = Token { "", TokenType::INVALID };
    REQUIRE_NOTHROW(parser.checkType(token, TokenType::INVALID));

    Token token2 = Token { "", TokenType::STRING };
    REQUIRE_THROWS_WITH(parser.checkType(token2, TokenType::INVALID), "Not expecting this token type!");
}

TEST_CASE("Parser getAndCheckNextToken and peekAndCheckNextToken") {
    Parser parser;
    parser.lexer.text = "hello 123";
    REQUIRE_NOTHROW(parser.getAndCheckNextToken(TokenType::IDENTIFIER));

    REQUIRE_THROWS_WITH(parser.peekAndCheckNextToken(TokenType::IDENTIFIER), "Not expecting this token type!");
    REQUIRE_NOTHROW(parser.peekAndCheckNextToken(TokenType::NUMBER));
}

TEST_CASE("Parser getAndCheckNextReservedToken and peekAndCheckNextReservedToken") {
    Parser parser;
    parser.lexer.text = "Select such that";
    REQUIRE_NOTHROW(parser.getAndCheckNextReservedToken(TokenType::SELECT));

    REQUIRE_THROWS_WITH(parser.peekAndCheckNextReservedToken(TokenType::PATTERN), "Not expecting this token type!");
    REQUIRE_NOTHROW(parser.peekAndCheckNextReservedToken(TokenType::SUCH_THAT));
}

TEST_CASE("Parser peekNextToken and getNextToken") {
    Parser parser;
    parser.lexer.text = "assign a; \n";

    REQUIRE(parser.peekNextToken() == Token{"assign", TokenType::IDENTIFIER});
    REQUIRE(parser.peekAndCheckNextToken(TokenType::IDENTIFIER) == Token{"assign", TokenType::IDENTIFIER});

    REQUIRE(parser.getNextToken() == Token{"assign", TokenType::IDENTIFIER});
    REQUIRE(parser.getAndCheckNextToken(TokenType::IDENTIFIER) == Token{"a", TokenType::IDENTIFIER});
}

TEST_CASE("Parser addPql") {
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

TEST_CASE("Parser parseDeclaration") {
    Parser parser;
    parser.lexer.text = "a;\n";

    Query queryObj;
    parser.parseDeclaration(queryObj, DesignEntity::ASSIGN);

    REQUIRE(queryObj.hasDeclaration("a"));
}

TEST_CASE("Parser parseDeclarations") {
    Parser parser;
    parser.lexer.text = "assign a;\n";

    Query queryObj;
    parser.parseDeclarations(queryObj);
    REQUIRE(queryObj.hasDeclaration("a"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a") == DesignEntity::ASSIGN);

    // Multiple declarations
    queryObj = Query {};
    parser.lexer.text = "assign a, a1;\n";
    parser.parseDeclarations(queryObj);

    REQUIRE(queryObj.hasDeclaration("a"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a") == DesignEntity::ASSIGN);

    REQUIRE(queryObj.hasDeclaration("a1"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a1") == DesignEntity::ASSIGN);

    // Multiple declarations of different Design Entities
    queryObj = Query {};
    parser.lexer.text = "assign a, a1; variable v;";
    parser.parseDeclarations(queryObj);

    REQUIRE(queryObj.hasDeclaration("a"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a") == DesignEntity::ASSIGN);

    REQUIRE(queryObj.hasDeclaration("a1"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a1") == DesignEntity::ASSIGN);

    parser.parseDeclarations(queryObj);
    REQUIRE(queryObj.hasDeclaration("v"));
    REQUIRE(queryObj.getDeclarationDesignEntity("v") == DesignEntity::VARIABLE);
}

TEST_CASE("Parser parseSelectFields") {
    Parser parser;
    parser.addPql("assign a; \n Select a");

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);

    REQUIRE(queryObj.hasVariable("a"));

    queryObj = Query {};
    parser.addPql("assign a; \n Select v");

    parser.parseDeclarations(queryObj);
    REQUIRE_THROWS_WITH(parser.parseSelectFields(queryObj), "Variable does not exist in declaration!");
}

TEST_CASE("Parser parseSuchThat - Uses") {
    std::string testQuery = "assign a;\n Select a such that Uses (a, _)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);
    parser.parseSuchThat(queryObj);

    std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
    REQUIRE(relRefPtr->getType() == RelRefType::USESS);

    std::shared_ptr<Uses> usesPtr = std::dynamic_pointer_cast<Uses>(relRefPtr);
    REQUIRE(usesPtr->useStmt.getDeclaration() == "a");
    REQUIRE(usesPtr->useStmt.isDeclaration());
    REQUIRE(usesPtr->used.isWildcard());
}

TEST_CASE("Parser parseSuchThat - Modifies") {
    std::string testQuery = "assign a; \n Select a such that Modifies (a, _)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);
    parser.parseSuchThat(queryObj);

    std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
    REQUIRE(relRefPtr->getType() == RelRefType::MODIFIESS);

    std::shared_ptr<Modifies> modifiesPtr = std::dynamic_pointer_cast<Modifies>(relRefPtr);
    REQUIRE(modifiesPtr->modifiesStmt.getDeclaration() == "a");
    REQUIRE(modifiesPtr->modifiesStmt.isDeclaration());
    REQUIRE(modifiesPtr->modified.isWildcard());
}

TEST_CASE("Parser parsePattern - wildcard expression") {
    std::string testQuery = "assign a; \n Select a pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);
    parser.parsePattern(queryObj);

    std::vector<Pattern> patterns = queryObj.getPattern();
    Pattern pattern = patterns[0];

    REQUIRE(pattern.getSynonym() == "a");
    bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
    REQUIRE(validDeclaration);
    REQUIRE(pattern.getExpression() == "_");
}

TEST_CASE("Parser parsePattern - string expression") {
    std::string testQuery = "assign a; \n Select a pattern a (v, \"x\")";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);
    parser.parsePattern(queryObj);

    std::vector<Pattern> patterns = queryObj.getPattern();
    Pattern pattern = patterns[0];

    REQUIRE(pattern.getSynonym() == "a");
    bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
    REQUIRE(validDeclaration);
    REQUIRE(pattern.getExpression() == "x");
}

TEST_CASE("Parser parsePattern - string expression with wildcard") {
    std::string testQuery = "assign a; \n Select a pattern a (v, _\"x\"_)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);
    parser.parsePattern(queryObj);

    std::vector<Pattern> patterns = queryObj.getPattern();
    Pattern pattern = patterns[0];

    REQUIRE(pattern.getSynonym() == "a");
    bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
    REQUIRE(validDeclaration);
    REQUIRE(pattern.getExpression() == "_x_");
}

TEST_CASE("Parser isEntRef") {
    Parser parser;
    Query query;
    Token token = { "", TokenType::STRING };
    REQUIRE(parser.isEntRef(token, query));
    token = {"", TokenType::PATTERN};
    REQUIRE(!(parser.isEntRef(token, query)));
}

TEST_CASE("Parser isSmtRef") {
    Parser parser;
    Query query;
    Token token = { "", TokenType::NUMBER };
    REQUIRE(parser.isStmtRef(token, query));
    token = {"", TokenType::PATTERN};
    REQUIRE(!(parser.isEntRef(token, query)));
}

TEST_CASE("Parser parseStmtRef") {
    Parser parser;
    parser.addPql("3");
    Query query;
    StmtRef sr = parser.parseStmtRef(query);
    REQUIRE(sr.isLineNo());
    REQUIRE(sr.getLineNo() == 3);
}

TEST_CASE("Parser parseEntRef") {
    Parser parser;
    parser.addPql("\"x\"");
    Query query;
    EntRef er = parser.parseEntRef(query);
    REQUIRE(er.isVarName());
    REQUIRE(er.getVariableName() == "x");
}

TEST_CASE("Parser parseRelRefVariabels") {
    Query queryObj;
    Parser parser;
    parser.lexer.text = "(3, \"x\")";
    std::shared_ptr<RelRef> ptr = parser.parseRelRefVariables<Modifies>(queryObj,
        &Modifies::modifiesStmt, &Modifies::modified);
    REQUIRE(ptr.get()->getType() == RelRefType::MODIFIESS);

    std::shared_ptr<Modifies> sharedPtr = std::dynamic_pointer_cast<Modifies>(ptr);
    Modifies* mPtr = sharedPtr.get();
    REQUIRE(mPtr->modifiesStmt.isLineNo());
    REQUIRE(mPtr->modifiesStmt.getLineNo() == 3);
    REQUIRE(mPtr->modified.isVarName());
    REQUIRE(mPtr->modified.getVariableName() == "x");
}

TEST_CASE("Parser parseRelRef") {
    Query queryObj;
    Parser parser;
    parser.lexer.text = "Uses(_,_)";
    std::shared_ptr<RelRef> ptr = parser.parseRelRef(queryObj);
    REQUIRE(ptr.get()->getType() == RelRefType::USESS);

    std::shared_ptr<Uses> sharedPtr = std::dynamic_pointer_cast<Uses>(ptr);
    Uses* uPtr = sharedPtr.get();
    REQUIRE(uPtr->useStmt.isWildcard());
    REQUIRE(uPtr->used.isWildcard());
}

TEST_CASE("Parser parseExpSpec") {
    Query queryObj;
    Parser parser;
    parser.lexer.text = "_\"x\"_";
    REQUIRE(parser.parseExpSpec() == "_x_");

    parser.lexer.text = "\"x\"";
    REQUIRE(parser.parseExpSpec() == "x");

    parser.lexer.text = "_";
    REQUIRE(parser.parseExpSpec() == "_");

    parser.lexer.text = "_\"x\"";
    REQUIRE(parser.parseExpSpec() == "_x");

    parser.lexer.text = "\"x\"_";
    REQUIRE(parser.parseExpSpec() == "x_");
}

TEST_CASE("Parser parseQuery") {
    Parser parser;
    parser.lexer.text = "Select c";
    Query query;
    query.addDeclaration("c", DesignEntity::CONSTANT);
    parser.parseQuery(query);

    REQUIRE(query.hasVariable("c"));

    parser.lexer.text = "Select a such that Modifies (a, v1) pattern a1 (v, _\"x\"_)";
    query = {};
    query.addDeclaration("a", DesignEntity::ASSIGN);
    query.addDeclaration("a1", DesignEntity::ASSIGN);
    query.addDeclaration("v", DesignEntity::VARIABLE);
    query.addDeclaration("v1", DesignEntity::VARIABLE);
    parser.parseQuery(query);

    REQUIRE(query.hasVariable("a"));

    // Check such that
    std::shared_ptr<RelRef> relRefPtr = (query.getSuchthat())[0];
    REQUIRE(relRefPtr->getType() == RelRefType::MODIFIESS);

    std::shared_ptr<Modifies> modifiesPtr = std::dynamic_pointer_cast<Modifies>(relRefPtr);
    REQUIRE(modifiesPtr->modifiesStmt.isDeclaration());
    REQUIRE(modifiesPtr->modifiesStmt.getDeclaration() == "a");
    REQUIRE(modifiesPtr->modified.isDeclaration());
    REQUIRE(modifiesPtr->modified.getDeclaration() == "v1");

    // Check pattern
    std::vector<Pattern> patterns = query.getPattern();
    Pattern pattern = patterns[0];

    REQUIRE(pattern.getSynonym() == "a1");
    bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
    REQUIRE(validDeclaration);
    REQUIRE(pattern.getExpression() == "_x_");
}

TEST_CASE("Parser parsePql") {
    std::string testQuery = "assign a, a1; variable v; Select a such that Modifies (a, _) pattern a (v, _\"x\"_)";

    Parser parser;
    Query queryObj = parser.parsePql(testQuery);

    // Check declaration
    REQUIRE(queryObj.hasDeclaration("a"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a") == DesignEntity::ASSIGN);

    REQUIRE(queryObj.hasDeclaration("a1"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a1") == DesignEntity::ASSIGN);

    REQUIRE(queryObj.hasDeclaration("v"));
    REQUIRE(queryObj.getDeclarationDesignEntity("v") == DesignEntity::VARIABLE);

    // Check Select
    REQUIRE(queryObj.hasVariable("a"));

    // Check such that
    std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
    REQUIRE(relRefPtr->getType() == RelRefType::MODIFIESS);

    std::shared_ptr<Modifies> modifiesPtr = std::dynamic_pointer_cast<Modifies>(relRefPtr);
    REQUIRE(modifiesPtr->modifiesStmt.getDeclaration() == "a");
    REQUIRE(modifiesPtr->modifiesStmt.isDeclaration());
    REQUIRE(modifiesPtr->modified.isWildcard());

    // Check pattern
    std::vector<Pattern> patterns = queryObj.getPattern();
    Pattern pattern = patterns[0];

    REQUIRE(pattern.getSynonym() == "a");
    bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
    REQUIRE(validDeclaration);
    REQUIRE(pattern.getExpression() == "_x_");
}



