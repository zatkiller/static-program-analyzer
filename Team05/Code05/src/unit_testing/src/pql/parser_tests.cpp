#include "pql/parser.h"
#include "catch.hpp"
#include <memory>
#include <vector>

#include "exceptions.h"

using qps::parser::Lexer;
using qps::parser::Parser;
using qps::parser::Token;
using qps::parser::TokenType;
using qps::parser::DesignEntity;

using qps::query::Query;
using qps::query::StmtRef;
using qps::query::EntRef;
using qps::query::RelRef;
using qps::query::RelRefType;
using qps::query::Uses;
using qps::query::Modifies;
using qps::query::Parent;
using qps::query::ParentT;
using qps::query::Follows;
using qps::query::FollowsT;
using qps::query::Pattern;
using qps::query::ExpSpec;

TEST_CASE("Parser checkType") {
    Parser parser;
    Token token = Token{"", TokenType::INVALID};
    REQUIRE_NOTHROW(parser.checkType(token, TokenType::INVALID));

    Token token2 = Token{"", TokenType::STRING};
    REQUIRE_THROWS_MATCHES(parser.checkType(token2, TokenType::INVALID), exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::notExpectingTokenMessage));
}

TEST_CASE("Parser getAndCheckNextToken and peekAndCheckNextToken") {
    Parser parser;
    parser.lexer.text = "hello 123";
    REQUIRE_NOTHROW(parser.getAndCheckNextToken(TokenType::IDENTIFIER));

    REQUIRE_THROWS_MATCHES(parser.peekAndCheckNextToken(TokenType::IDENTIFIER), exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::notExpectingTokenMessage));
    REQUIRE_NOTHROW(parser.peekAndCheckNextToken(TokenType::NUMBER));
}

TEST_CASE("Parser getAndCheckNextReservedToken and peekAndCheckNextReservedToken") {
    Parser parser;
    parser.lexer.text = "Select such that";
    REQUIRE_NOTHROW(parser.getAndCheckNextReservedToken(TokenType::SELECT));

    REQUIRE_THROWS_MATCHES(parser.peekAndCheckNextReservedToken(TokenType::PATTERN), exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::notExpectingTokenMessage));
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
    queryObj = Query{};
    parser.lexer.text = "assign a, a1;\n";
    parser.parseDeclarations(queryObj);

    REQUIRE(queryObj.hasDeclaration("a"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a") == DesignEntity::ASSIGN);

    REQUIRE(queryObj.hasDeclaration("a1"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a1") == DesignEntity::ASSIGN);

    // Multiple declarations of different Design Entities
    queryObj = Query{};
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

    queryObj = Query{};
    parser.addPql("assign a; \n Select v");

    parser.parseDeclarations(queryObj);

    REQUIRE_THROWS_MATCHES(parser.parseSelectFields(queryObj), exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::declarationDoesNotExistMessage));
}

TEST_CASE("Parser parseSuchThat - Uses") {
    SECTION ("Valid query with Uses relationship") {
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

    SECTION("Invalid first argument (wildcard) for Uses") {
        std::string testQuery = "assign a;\n Select a such that Uses (_, _)";
        Parser parser;
        parser.addPql(testQuery);

        Query queryObj;

        parser.parseDeclarations(queryObj);
        parser.parseSelectFields(queryObj);
        REQUIRE_THROWS_MATCHES(parser.parseSuchThat(queryObj), exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::cannotBeWildcardMessage));
    }

    SECTION("Invalid second argument non-variable design entity for Uses") {
        std::string testQuery = "assign a;\n Select a such that Uses (1, a)";
        Parser parser;
        parser.addPql(testQuery);

        Query queryObj;

        parser.parseDeclarations(queryObj);
        parser.parseSelectFields(queryObj);
        REQUIRE_THROWS_MATCHES(parser.parseSuchThat(queryObj), exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::notVariableSynonymMessage));
    }
}

TEST_CASE("Parser parseSuchThat - Modifies") {
    SECTION ("Valid query with Modifies relationship") {
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

    SECTION("Invalid first argument (wildcard) for Modifies") {
        std::string testQuery = "assign a;\n Select a such that Modifies (_, _)";
        Parser parser;
        parser.addPql(testQuery);

        Query queryObj;

        parser.parseDeclarations(queryObj);
        parser.parseSelectFields(queryObj);
        REQUIRE_THROWS_MATCHES(parser.parseSuchThat(queryObj), exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::cannotBeWildcardMessage));
    }

    SECTION("Invalid second argument non-variable design entity for Modifies") {
        std::string testQuery = "assign a;\n Select a such that Modifies (1, a)";
        Parser parser;
        parser.addPql(testQuery);

        Query queryObj;

        parser.parseDeclarations(queryObj);
        parser.parseSelectFields(queryObj);
        REQUIRE_THROWS_MATCHES(parser.parseSuchThat(queryObj), exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::notVariableSynonymMessage));
    }
}

TEST_CASE("Parser parseSuchThat - Follows") {
    SECTION ("Valid query with Follows relationship") {
        std::string testQuery = "stmt s1, s2; \n Select s1 such that Follows (s1, 20)";

        Parser parser;
        parser.addPql(testQuery);

        Query queryObj;

        parser.parseDeclarations(queryObj);
        parser.parseSelectFields(queryObj);
        parser.parseSuchThat(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::FOLLOWS);

        std::shared_ptr<Follows> followsPtr = std::dynamic_pointer_cast<Follows>(relRefPtr);
        REQUIRE(followsPtr->follower.isDeclaration());
        REQUIRE(followsPtr->follower.getDeclaration() == "s1");
        REQUIRE(followsPtr->followed.isLineNo());
        REQUIRE(followsPtr->followed.getLineNo() == 20);
    }
}

TEST_CASE("Parser parseSuchThat - Follows*") {
    SECTION ("Valid query with Follows* relationship") {
        std::string testQuery = "stmt s1, s2; \n Select s1 such that Follows* (s2, 20)";

        Parser parser;
        parser.addPql(testQuery);

        Query queryObj;

        parser.parseDeclarations(queryObj);
        parser.parseSelectFields(queryObj);
        parser.parseSuchThat(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::FOLLOWST);

        std::shared_ptr<FollowsT> followsPtr = std::dynamic_pointer_cast<FollowsT>(relRefPtr);
        REQUIRE(followsPtr->follower.isDeclaration());
        REQUIRE(followsPtr->follower.getDeclaration() == "s2");
        REQUIRE(followsPtr->transitiveFollowed.isLineNo());
        REQUIRE(followsPtr->transitiveFollowed.getLineNo() == 20);
    }
}

TEST_CASE("Parser parseSuchThat - Parent") {
    SECTION ("Valid query with Parent relationship") {
        std::string testQuery = "stmt s1, s2; \n Select s1 such that Parent (20, s2)";

        Parser parser;
        parser.addPql(testQuery);

        Query queryObj;

        parser.parseDeclarations(queryObj);
        parser.parseSelectFields(queryObj);
        parser.parseSuchThat(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::PARENT);

        std::shared_ptr<Parent> parentPtr = std::dynamic_pointer_cast<Parent>(relRefPtr);
        REQUIRE(parentPtr->parent.isLineNo());
        REQUIRE(parentPtr->parent.getLineNo() == 20);
        REQUIRE(parentPtr->child.isDeclaration());
        REQUIRE(parentPtr->child.getDeclaration() == "s2");
    }
}

TEST_CASE("Parser parseSuchThat - Parent*") {
    SECTION ("Valid query with Parent* relationship") {
        std::string testQuery = "stmt s1, s2; \n Select s1 such that Parent* (20, s2)";

        Parser parser;
        parser.addPql(testQuery);

        Query queryObj;

        parser.parseDeclarations(queryObj);
        parser.parseSelectFields(queryObj);
        parser.parseSuchThat(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::PARENTT);

        std::shared_ptr<ParentT> parentPtr = std::dynamic_pointer_cast<ParentT>(relRefPtr);
        REQUIRE(parentPtr->parent.isLineNo());
        REQUIRE(parentPtr->parent.getLineNo() == 20);
        REQUIRE(parentPtr->transitiveChild.isDeclaration());
        REQUIRE(parentPtr->transitiveChild.getDeclaration() == "s2");
    }
}

TEST_CASE("Parser parseExpSpec") {
    std::string testQuery = "_";

    Parser parser;
    parser.addPql(testQuery);

    REQUIRE(parser.parseExpSpec() == ExpSpec::ofWildcard());

    testQuery = "_\"x\"_";
    parser.addPql(testQuery);

    REQUIRE(parser.parseExpSpec() == ExpSpec::ofPartialMatch("x"));

    testQuery = "\"x\"";
    parser.addPql(testQuery);

    REQUIRE(parser.parseExpSpec() == ExpSpec::ofFullMatch("x"));

    testQuery = "_\"x\"";
    parser.addPql(testQuery);
    REQUIRE_THROWS_MATCHES(parser.parseExpSpec(), exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::notExpectingTokenMessage));
}

TEST_CASE("Parser parsePattern - wildcard expression") {
    std::string testQuery = "assign a; variable v;\n Select a pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);
    parser.parsePattern(queryObj);

    std::vector<Pattern> patterns = queryObj.getPattern();
    Pattern pattern = patterns[0];

    REQUIRE(pattern.getSynonym() == "a");
    bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
    REQUIRE(validDeclaration);
    REQUIRE(pattern.getExpression() == ExpSpec::ofWildcard());
}

TEST_CASE("Parser parsePattern - string expression") {
    std::string testQuery = "assign a; variable v;\n Select a pattern a (v, \"x\")";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);
    parser.parseDeclarations(queryObj);
    parser.parseSelectFields(queryObj);
    parser.parsePattern(queryObj);

    std::vector<Pattern> patterns = queryObj.getPattern();
    Pattern pattern = patterns[0];

    REQUIRE(pattern.getSynonym() == "a");
    bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
    REQUIRE(validDeclaration);
    REQUIRE(pattern.getExpression() == ExpSpec::ofFullMatch("x"));
}

TEST_CASE("Parser parsePattern - string expression with wildcard") {

    SECTION("Valid Pattern semantics") {
        std::string testQuery = "assign a; variable v;\n Select a pattern a (v, _\"x\"_)";

        Parser parser;
        parser.addPql(testQuery);

        Query queryObj;

        parser.parseDeclarations(queryObj);
        parser.parseDeclarations(queryObj);
        parser.parseSelectFields(queryObj);
        parser.parsePattern(queryObj);

        std::vector<Pattern> patterns = queryObj.getPattern();
        Pattern pattern = patterns[0];

        REQUIRE(pattern.getSynonym() == "a");
        bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
        REQUIRE(validDeclaration);
        REQUIRE(pattern.getExpression() == ExpSpec::ofPartialMatch("x"));
    }

    SECTION("Invalid Pattern semantics") {
        std::string testQuery = "assign a;\n Select a pattern a (a, _\"x\"_)";

        Parser parser;
        parser.addPql(testQuery);

        Query queryObj;

        parser.parseDeclarations(queryObj);
        parser.parseSelectFields(queryObj);
        REQUIRE_THROWS_MATCHES(parser.parsePattern(queryObj),
                               exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::notVariableSynonymMessage));
    }
}

TEST_CASE("Parser isEntRef") {
    Parser parser;
    Query query;
    Token token = {"", TokenType::STRING};
    REQUIRE(parser.isEntRef(token, query));
    token = {"", TokenType::PATTERN};
    REQUIRE(!(parser.isEntRef(token, query)));
}

TEST_CASE("Parser isSmtRef") {
    Parser parser;
    Query query;
    Token token = {"", TokenType::NUMBER};
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

TEST_CASE("Parser parseRelRefVariables") {

    SECTION("parseRelRefVariables - Follows") {
        Query queryObj;
        Parser parser;
        parser.lexer.text = "(3, 5)";
        std::shared_ptr<RelRef> ptr = parser.parseRelRefVariables<Follows>(queryObj, &Follows::follower,
                                                                           &Follows::followed);
        REQUIRE(ptr.get()->getType() == RelRefType::FOLLOWS);

        Follows *fPtr = std::dynamic_pointer_cast<Follows>(ptr).get();
        REQUIRE(fPtr->follower.isLineNo());
        REQUIRE(fPtr->follower.getLineNo() == 3);
        REQUIRE(fPtr->followed.isLineNo());
        REQUIRE(fPtr->followed.getLineNo() == 5);
    }

    SECTION("parseRelRefVariables - FollowsT") {
        Query queryObj;
        Parser parser;
        parser.lexer.text = "(3, 10)";
        std::shared_ptr<RelRef> ptr = parser.parseRelRefVariables<FollowsT>(queryObj, &FollowsT::follower,
                                                                            &FollowsT::transitiveFollowed);
        REQUIRE(ptr.get()->getType() == RelRefType::FOLLOWST);

        FollowsT *fPtr = std::dynamic_pointer_cast<FollowsT>(ptr).get();
        REQUIRE(fPtr->follower.isLineNo());
        REQUIRE(fPtr->follower.getLineNo() == 3);
        REQUIRE(fPtr->transitiveFollowed.isLineNo());
        REQUIRE(fPtr->transitiveFollowed.getLineNo() == 10);
    }

    SECTION("parseRelRefVariables - Parent") {
        Query queryObj;
        Parser parser;
        parser.lexer.text = "(3, 5)";
        std::shared_ptr<RelRef> ptr = parser.parseRelRefVariables<Parent>(queryObj, &Parent::parent, &Parent::child);
        REQUIRE(ptr.get()->getType() == RelRefType::PARENT);

        Parent *pPtr = std::dynamic_pointer_cast<Parent>(ptr).get();
        REQUIRE(pPtr->parent.isLineNo());
        REQUIRE(pPtr->parent.getLineNo() == 3);
        REQUIRE(pPtr->child.isLineNo());
        REQUIRE(pPtr->child.getLineNo() == 5);
    }

    SECTION("parseRelRefVariables - ParentT") {
        Query queryObj;
        Parser parser;
        parser.lexer.text = "(3, 10)";
        std::shared_ptr<RelRef> ptr = parser.parseRelRefVariables<ParentT>(queryObj, &ParentT::parent,
                                                                           &ParentT::transitiveChild);
        REQUIRE(ptr.get()->getType() == RelRefType::PARENTT);

        ParentT *pPtr = std::dynamic_pointer_cast<ParentT>(ptr).get();
        REQUIRE(pPtr->parent.isLineNo());
        REQUIRE(pPtr->parent.getLineNo() == 3);
        REQUIRE(pPtr->transitiveChild.isLineNo());
        REQUIRE(pPtr->transitiveChild.getLineNo() == 10);
    }

    SECTION("parseRelRefVariables - Follows") {
        Query queryObj;
        Parser parser;
        queryObj.addDeclaration("v", DesignEntity::VARIABLE);
        parser.lexer.text = "(v, v)";
        REQUIRE_THROWS_MATCHES(parser.parseRelRefVariables<Follows>(queryObj, &Follows::follower, &Follows::followed),
                               exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::notStatementSynonymMessage));
    }
}

TEST_CASE("Parser parseModifiesOrUsesVariables") {
    Query queryObj;
    Parser parser;

    SECTION("Valid Modifies Rel Variables") {
        parser.lexer.text = "(3, \"x\")";
        std::shared_ptr<RelRef> ptr = parser.parseModifiesOrUsesVariables(queryObj, TokenType::MODIFIES);
        REQUIRE(ptr.get()->getType() == RelRefType::MODIFIESS);

        std::shared_ptr<Modifies> sharedPtr = std::dynamic_pointer_cast<Modifies>(ptr);
        Modifies *mPtr = sharedPtr.get();
        REQUIRE(mPtr->modifiesStmt.isLineNo());
        REQUIRE(mPtr->modifiesStmt.getLineNo() == 3);
        REQUIRE(mPtr->modified.isVarName());
        REQUIRE(mPtr->modified.getVariableName() == "x");
    }

    SECTION("Invalid Modifies Rel Variables - Wildcard in first argument") {
        parser.lexer.text = "(_, \"x\")";
        REQUIRE_THROWS_MATCHES(parser.parseModifiesOrUsesVariables(queryObj, TokenType::MODIFIES),
                               exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::cannotBeWildcardMessage));
    }

    SECTION("Invalid Modifies Rel Variables - synonym does not belong to variable") {
        parser.lexer.text = "(_, x)";
        queryObj = {};
        queryObj.addDeclaration("x", DesignEntity::ASSIGN);
        REQUIRE_THROWS_MATCHES(parser.parseModifiesOrUsesVariables(queryObj, TokenType::MODIFIES),
                               exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::cannotBeWildcardMessage));
    }

    SECTION("Valid Uses Rel Variables") {
        parser.lexer.text = "(3, \"x\")";
        std::shared_ptr<RelRef> ptr = parser.parseModifiesOrUsesVariables(queryObj, TokenType::USES);
        REQUIRE(ptr.get()->getType() == RelRefType::USESS);

        std::shared_ptr<Uses> sharedPtr = std::dynamic_pointer_cast<Uses>(ptr);
        Uses *uPtr = sharedPtr.get();
        REQUIRE(uPtr->useStmt.isLineNo());
        REQUIRE(uPtr->useStmt.getLineNo() == 3);
        REQUIRE(uPtr->used.isVarName());
        REQUIRE(uPtr->used.getVariableName() == "x");
    }

    SECTION("Invalid Uses Rel Variables - Wildcard in first argument") {
        parser.lexer.text = "(_, \"x\")";
        REQUIRE_THROWS_MATCHES(parser.parseModifiesOrUsesVariables(queryObj, TokenType::USES),
                               exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::cannotBeWildcardMessage));
    }
}

TEST_CASE("Parser parseRelRef") {
    Query queryObj;
    queryObj.addDeclaration("x", DesignEntity::VARIABLE);
    Parser parser;
    parser.lexer.text = "Uses(x,_)";
    std::shared_ptr<RelRef> ptr = parser.parseRelRef(queryObj);
    REQUIRE(ptr.get()->getType() == RelRefType::USESS);

    std::shared_ptr<Uses> sharedPtr = std::dynamic_pointer_cast<Uses>(ptr);
    Uses *uPtr = sharedPtr.get();
    REQUIRE(uPtr->useStmt.isDeclaration());
    REQUIRE(uPtr->used.isWildcard());
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
    REQUIRE(pattern.getExpression() == ExpSpec::ofPartialMatch("x"));
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
    REQUIRE(pattern.getExpression() == ExpSpec::ofPartialMatch("x"));
}

TEST_CASE("Parser isValidStatement") {
    Query query;
    Parser p;

    query.addDeclaration("s", DesignEntity::STMT);
    query.addDeclaration("a", DesignEntity::ASSIGN);
    query.addDeclaration("pn", DesignEntity::PRINT);
    query.addDeclaration("r", DesignEntity::READ);
    query.addDeclaration("ifs", DesignEntity::IF);
    query.addDeclaration("w", DesignEntity::WHILE);
    query.addDeclaration("cl", DesignEntity::CALL);

    query.addDeclaration("v", DesignEntity::VARIABLE);
    query.addDeclaration("p", DesignEntity::PROCEDURE);
    query.addDeclaration("c", DesignEntity::CONSTANT);

    // Valid declarations
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("s")));
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("a")));
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("pn")));
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("r")));
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("ifs")));
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("w")));
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("cl")));

    // Invalid statement types
    REQUIRE(!(p.isValidStatementType(query, StmtRef::ofDeclaration("v"))));
    REQUIRE(!(p.isValidStatementType(query, StmtRef::ofDeclaration("p"))));
    REQUIRE(!(p.isValidStatementType(query, StmtRef::ofDeclaration("c"))));

    // Declarations which do not exist
    REQUIRE_THROWS_MATCHES(p.isValidStatementType(query, StmtRef::ofDeclaration("x")),
                           exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::declarationDoesNotExistMessage));
}




