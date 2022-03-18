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
using qps::query::UsesS;
using qps::query::UsesP;
using qps::query::ModifiesS;
using qps::query::ModifiesP;
using qps::query::Parent;
using qps::query::ParentT;
using qps::query::Follows;
using qps::query::FollowsT;
using qps::query::Next;
using qps::query::NextT;
using qps::query::Calls;
using qps::query::CallsT;
using qps::query::Pattern;
using qps::query::ExpSpec;
using qps::query::AttrRef;
using qps::query::AttrName;
using qps::query::AttrCompareRef;
using qps::query::AttrCompare;

TEST_CASE("Parser checkType") {
    Parser parser;
    Token token = Token{"", TokenType::INVALID};
    REQUIRE_NOTHROW(parser.checkType(token, TokenType::INVALID));

    Token token2 = Token{"", TokenType::STRING};
    REQUIRE_THROWS_MATCHES(parser.checkType(token2, TokenType::INVALID), exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::notExpectingTokenMessage));
}

TEST_CASE("Parser checkSurroundingWhitespace") {
    Parser parser;
    parser.lexer.text = " hi ";
    auto f = [](){};
    REQUIRE_THROWS_MATCHES(parser.checkSurroundingWhitespace(f), exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::unexpectedWhitespaceMessage));
}

TEST_CASE("Parser checkDesignEntityAndAttrNameMatch") {
    Parser parser;
    REQUIRE_THROWS_MATCHES(parser.checkDesignEntityAndAttrNameMatch(DesignEntity::CONSTANT, AttrName::PROCNAME),
                           exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::invalidAttrNameForDesignEntity));
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

TEST_CASE("Parser addInput") {
    std::string testQuery1 = "assign a; \n Select a such that Uses (a, v) pattern a (v, _)";
    std::string testQuery2 = "variable v; \n Select v such that Uses (a, v) pattern a (v, _)";

    Parser parser;
    parser.addInput(testQuery1);

    Lexer lexer1 = parser.lexer;
    REQUIRE(parser.getParsedText() == testQuery1);

    parser.addInput(testQuery2);

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
    parser.lexer.text = "assign a, a1; variable v; call cl;";
    parser.parseDeclarations(queryObj);

    REQUIRE(queryObj.hasDeclaration("a"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a") == DesignEntity::ASSIGN);

    REQUIRE(queryObj.hasDeclaration("a1"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a1") == DesignEntity::ASSIGN);

    parser.parseDeclarations(queryObj);
    REQUIRE(queryObj.hasDeclaration("v"));
    REQUIRE(queryObj.getDeclarationDesignEntity("v") == DesignEntity::VARIABLE);

    parser.parseDeclarations(queryObj);
    REQUIRE(queryObj.hasDeclaration("cl"));
    REQUIRE(queryObj.getDeclarationDesignEntity("cl") == DesignEntity::CALL);
}

TEST_CASE("Parser parseSelectFields") {
    Parser parser;
    parser.addInput("Select a");

    Query queryObj;

    queryObj.addDeclaration("a", DesignEntity::ASSIGN);
    parser.parseSelectFields(queryObj);

    REQUIRE(queryObj.hasVariable("a"));

    queryObj = Query{};
    parser.addInput("Select v");

    REQUIRE_THROWS_MATCHES(parser.parseSelectFields(queryObj), exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::declarationDoesNotExistMessage));
}

TEST_CASE("Parser parseRelRef - Uses") {
    SECTION ("Valid query with Uses relationship") {
        std::string testQuery = "Uses (a, _)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("a", DesignEntity::ASSIGN);

        parser.parseRelRef(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::USESS);

        std::shared_ptr<UsesS> usesPtr = std::dynamic_pointer_cast<UsesS>(relRefPtr);
        REQUIRE(usesPtr->useStmt.getDeclaration() == "a");
        REQUIRE(usesPtr->useStmt.isDeclaration());
        REQUIRE(usesPtr->used.isWildcard());
    }

    SECTION("Invalid first argument (wildcard) for Uses") {
        std::string testQuery = "Uses (_, _)";
        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("a", DesignEntity::ASSIGN);
        REQUIRE_THROWS_MATCHES(parser.parseRelRef(queryObj), exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::cannotBeWildcardMessage));
    }

    SECTION("Invalid second argument non-variable design entity for Uses") {
        std::string testQuery = "Uses (1, p)";
        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("p", DesignEntity::PROCEDURE);
        REQUIRE_THROWS_MATCHES(parser.parseRelRef(queryObj), exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::notVariableSynonymMessage));
    }
}

TEST_CASE("Parser parseRelRef - Modifies") {
    SECTION ("Valid query with Modifies relationship") {
        std::string testQuery = "Modifies (a, _)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("a", DesignEntity::ASSIGN);
        parser.parseRelRef(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::MODIFIESS);

        std::shared_ptr<ModifiesS> modifiesPtr = std::dynamic_pointer_cast<ModifiesS>(relRefPtr);
        REQUIRE(modifiesPtr->modifiesStmt.getDeclaration() == "a");
        REQUIRE(modifiesPtr->modifiesStmt.isDeclaration());
        REQUIRE(modifiesPtr->modified.isWildcard());
    }

    SECTION("Invalid first argument (wildcard) for Modifies") {
        std::string testQuery = "Modifies (_, _)";
        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("a", DesignEntity::ASSIGN);
        REQUIRE_THROWS_MATCHES(parser.parseRelRef(queryObj), exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::cannotBeWildcardMessage));
    }

    SECTION("Invalid second argument non-variable design entity for Modifies") {
        std::string testQuery = "Modifies (1, p)";
        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("p", DesignEntity::PROCEDURE);
        REQUIRE_THROWS_MATCHES(parser.parseRelRef(queryObj), exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::notVariableSynonymMessage));
    }
}

TEST_CASE("Parser parseRelRef - Follows") {
    SECTION ("Valid query with Follows relationship") {
        std::string testQuery = "Follows (s1, 20)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("s1", DesignEntity::STMT);
        queryObj.addDeclaration("s2", DesignEntity::STMT);
        parser.parseRelRef(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::FOLLOWS);

        std::shared_ptr<Follows> followsPtr = std::dynamic_pointer_cast<Follows>(relRefPtr);
        REQUIRE(followsPtr->follower.isDeclaration());
        REQUIRE(followsPtr->follower.getDeclaration() == "s1");
        REQUIRE(followsPtr->followed.isLineNo());
        REQUIRE(followsPtr->followed.getLineNo() == 20);
    }
}

TEST_CASE("Parser parseRelRef - Follows*") {
    SECTION ("Valid query with Follows* relationship") {
        std::string testQuery = "Follows* (s2, 20)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("s1", DesignEntity::STMT);
        queryObj.addDeclaration("s2", DesignEntity::STMT);
        parser.parseRelRef(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::FOLLOWST);

        std::shared_ptr<FollowsT> followsPtr = std::dynamic_pointer_cast<FollowsT>(relRefPtr);
        REQUIRE(followsPtr->follower.isDeclaration());
        REQUIRE(followsPtr->follower.getDeclaration() == "s2");
        REQUIRE(followsPtr->transitiveFollowed.isLineNo());
        REQUIRE(followsPtr->transitiveFollowed.getLineNo() == 20);
    }
}

TEST_CASE("Parser parseRelRef - Parent") {
    SECTION ("Valid query with Parent relationship") {
        std::string testQuery = "Parent (20, s2)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("s1", DesignEntity::STMT);
        queryObj.addDeclaration("s2", DesignEntity::STMT);

        parser.parseRelRef(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::PARENT);

        std::shared_ptr<Parent> parentPtr = std::dynamic_pointer_cast<Parent>(relRefPtr);
        REQUIRE(parentPtr->parent.isLineNo());
        REQUIRE(parentPtr->parent.getLineNo() == 20);
        REQUIRE(parentPtr->child.isDeclaration());
        REQUIRE(parentPtr->child.getDeclaration() == "s2");
    }
}

TEST_CASE("Parser parseRelRef - Parent*") {
    SECTION ("Valid query with Parent* relationship") {
        std::string testQuery = "Parent* (20, s2)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("s1", DesignEntity::STMT);
        queryObj.addDeclaration("s2", DesignEntity::STMT);

        parser.parseRelRef(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::PARENTT);

        std::shared_ptr<ParentT> parentPtr = std::dynamic_pointer_cast<ParentT>(relRefPtr);
        REQUIRE(parentPtr->parent.isLineNo());
        REQUIRE(parentPtr->parent.getLineNo() == 20);
        REQUIRE(parentPtr->transitiveChild.isDeclaration());
        REQUIRE(parentPtr->transitiveChild.getDeclaration() == "s2");
    }
}

TEST_CASE("Parser parseRelRef - Next") {
    SECTION ("Valid query with Next relationship") {
        std::string testQuery = "Next (s1, s2)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("s1", DesignEntity::STMT);
        queryObj.addDeclaration("s2", DesignEntity::STMT);

        parser.parseRelRef(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::NEXT);

        std::shared_ptr<Next> nPtr = std::dynamic_pointer_cast<Next>(relRefPtr);
        REQUIRE(nPtr->before.isDeclaration());
        REQUIRE(nPtr->before.getDeclaration() == "s1");
        REQUIRE(nPtr->after.isDeclaration());
        REQUIRE(nPtr->after.getDeclaration() == "s2");
    }
}

TEST_CASE("Parser parseRelRef - Next*") {
    SECTION ("Valid query with Next* relationship") {
        std::string testQuery = "Next* (s1, s2)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("s1", DesignEntity::STMT);
        queryObj.addDeclaration("s2", DesignEntity::STMT);

        parser.parseRelRef(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::NEXTT);

        std::shared_ptr<NextT> nPtr = std::dynamic_pointer_cast<NextT>(relRefPtr);
        REQUIRE(nPtr->before.isDeclaration());
        REQUIRE(nPtr->before.getDeclaration() == "s1");
        REQUIRE(nPtr->transitiveAfter.isDeclaration());
        REQUIRE(nPtr->transitiveAfter.getDeclaration() == "s2");
    }
}

TEST_CASE("Parser parseRelRef - Calls") {
    SECTION ("Valid query with Calls relationship") {
        std::string testQuery = "Calls (p1, p2)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("p1", DesignEntity::PROCEDURE);
        queryObj.addDeclaration("p2", DesignEntity::PROCEDURE);

        parser.parseRelRef(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::CALLS);

        std::shared_ptr<Calls> cPtr = std::dynamic_pointer_cast<Calls>(relRefPtr);
        REQUIRE(cPtr->caller.isDeclaration());
        REQUIRE(cPtr->caller.getDeclaration() == "p1");
        REQUIRE(cPtr->callee.isDeclaration());
        REQUIRE(cPtr->callee.getDeclaration() == "p2");
    }

    SECTION ("Invalid query with Calls relationship - synonym is not procedure") {
        std::string testQuery = "Calls (p, v)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("p", DesignEntity::PROCEDURE);
        queryObj.addDeclaration("v", DesignEntity::VARIABLE);

        REQUIRE_THROWS_MATCHES(parser.parseRelRef(queryObj), exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::notProcedureSynonymMessage));
    }
}

TEST_CASE("Parser parseRelRef - Calls*") {
    SECTION ("Valid query with Calls* relationship") {
        std::string testQuery = "Calls* (p1, p2)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("p1", DesignEntity::PROCEDURE);
        queryObj.addDeclaration("p2", DesignEntity::PROCEDURE);

        parser.parseRelRef(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::CALLST);

        std::shared_ptr<CallsT> cPtr = std::dynamic_pointer_cast<CallsT>(relRefPtr);
        REQUIRE(cPtr->caller.isDeclaration());
        REQUIRE(cPtr->caller.getDeclaration() == "p1");
        REQUIRE(cPtr->transitiveCallee.isDeclaration());
        REQUIRE(cPtr->transitiveCallee.getDeclaration() == "p2");
    }

    SECTION ("Invalid query with Calls* relationship - synonym is not procedure") {
        std::string testQuery = "Calls* (v, p)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("p", DesignEntity::PROCEDURE);
        queryObj.addDeclaration("v", DesignEntity::VARIABLE);

        REQUIRE_THROWS_MATCHES(parser.parseRelRef(queryObj), exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::notProcedureSynonymMessage));
    }
}

TEST_CASE("Parser parseSuchThatClause") {
        SECTION ("Single such that clause") {
            std::string testQuery = "such that Calls* (p1, p2)";

            Parser parser;
            parser.addInput(testQuery);

            Query queryObj;

            queryObj.addDeclaration("p1", DesignEntity::PROCEDURE);
            queryObj.addDeclaration("p2", DesignEntity::PROCEDURE);
            queryObj.addDeclaration("s1", DesignEntity::STMT);
            queryObj.addDeclaration("s2", DesignEntity::STMT);

            parser.parseSuchThatClause(queryObj);

            std::vector<std::shared_ptr<RelRef>> suchThat = queryObj.getSuchthat();

            REQUIRE(suchThat.size() == 1);

            std::shared_ptr<RelRef> relRefPtr = suchThat[0];
            REQUIRE(relRefPtr->getType() == RelRefType::CALLST);

            std::shared_ptr<CallsT> cPtr = std::dynamic_pointer_cast<CallsT>(relRefPtr);
            REQUIRE(cPtr->caller.isDeclaration());
            REQUIRE(cPtr->caller.getDeclaration() == "p1");
            REQUIRE(cPtr->transitiveCallee.isDeclaration());
            REQUIRE(cPtr->transitiveCallee.getDeclaration() == "p2");
    }

    SECTION ("Multiple such that clause") {
        SECTION("Valid Multiple such that clause") {
            std::string testQuery = "such that Calls* (p1, p2) and Next* (s1, s2)";

            Parser parser;
            parser.addInput(testQuery);

            Query queryObj;

            queryObj.addDeclaration("p1", DesignEntity::PROCEDURE);
            queryObj.addDeclaration("p2", DesignEntity::PROCEDURE);
            queryObj.addDeclaration("s1", DesignEntity::STMT);
            queryObj.addDeclaration("s2", DesignEntity::STMT);

            parser.parseSuchThatClause(queryObj);

            std::vector<std::shared_ptr<RelRef>> suchThat = queryObj.getSuchthat();

            REQUIRE(suchThat.size() == 2);

            std::shared_ptr<RelRef> relRefPtr = suchThat[0];
            REQUIRE(relRefPtr->getType() == RelRefType::CALLST);

            std::shared_ptr<CallsT> cPtr = std::dynamic_pointer_cast<CallsT>(relRefPtr);
            REQUIRE(cPtr->caller.isDeclaration());
            REQUIRE(cPtr->caller.getDeclaration() == "p1");
            REQUIRE(cPtr->transitiveCallee.isDeclaration());
            REQUIRE(cPtr->transitiveCallee.getDeclaration() == "p2");

            std::shared_ptr<RelRef> relRefPtr2 = suchThat[1];
            REQUIRE(relRefPtr2->getType() == RelRefType::NEXTT);

            std::shared_ptr<NextT> nPtr = std::dynamic_pointer_cast<NextT>(relRefPtr2);
            REQUIRE(nPtr->before.isDeclaration());
            REQUIRE(nPtr->before.getDeclaration() == "s1");
            REQUIRE(nPtr->transitiveAfter.isDeclaration());
            REQUIRE(nPtr->transitiveAfter.getDeclaration() == "s2");
        }

        SECTION ("Invalid Multiple such that clause") {
            std::string testQuery = "such that Calls* (p1, p2) and such that Next* (s1, s2)";

            Parser parser;
            parser.addInput(testQuery);

            Query queryObj;

            queryObj.addDeclaration("p1", DesignEntity::PROCEDURE);
            queryObj.addDeclaration("p2", DesignEntity::PROCEDURE);
            queryObj.addDeclaration("s1", DesignEntity::STMT);
            queryObj.addDeclaration("s2", DesignEntity::STMT);

            REQUIRE_THROWS_MATCHES(parser.parseSuchThatClause(queryObj), exceptions::PqlSyntaxException,
                                   Catch::Message(messages::qps::parser::notExpectingTokenMessage));
        }
    }
}

TEST_CASE("Parser parseExpSpec") {
    std::string testQuery = "_";

    Parser parser;
    parser.addInput(testQuery);

    REQUIRE(parser.parseExpSpec() == ExpSpec::ofWildcard());

    testQuery = "_\"x\"_";
    parser.addInput(testQuery);

    REQUIRE(parser.parseExpSpec() == ExpSpec::ofPartialMatch("x"));

    testQuery = "\"x\"";
    parser.addInput(testQuery);

    REQUIRE(parser.parseExpSpec() == ExpSpec::ofFullMatch("x"));

    testQuery = "_\"x\"";
    parser.addInput(testQuery);
    REQUIRE_THROWS_MATCHES(parser.parseExpSpec(), exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::notExpectingTokenMessage));
}

TEST_CASE("Parser validateExpr") {
    Parser parser;

    REQUIRE_NOTHROW(parser.validateExpr("1"));
    REQUIRE_NOTHROW(parser.validateExpr("x"));
    REQUIRE_NOTHROW(parser.validateExpr("1+2-3"));
    REQUIRE_NOTHROW(parser.validateExpr("1+3*x"));
    REQUIRE_NOTHROW(parser.validateExpr("3/(1+x)"));
    REQUIRE_NOTHROW(parser.validateExpr("(1+x)%3"));

    REQUIRE_THROWS_MATCHES(parser.validateExpr("+temp"), exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::expressionInvalidGrammarMessage));

    REQUIRE_THROWS_MATCHES(parser.validateExpr("temp-"), exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::expressionUnexpectedEndMessage));
}

TEST_CASE("Parser parsePatternLhs") {
    SECTION("Invalid Pattern semantics") {
        std::string testQuery = "p";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("p", DesignEntity::PROCEDURE);

        REQUIRE_THROWS_MATCHES(parser.parsePatternLhs(queryObj, "a"),
                               exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::notVariableSynonymMessage));
    }
}

TEST_CASE("Parser parsePattern") {
    SECTION ("parsePattern - pattern is assign") {
        SECTION ("with wildcard expression") {
            std::string testQuery = "a (v, _)";

            Parser parser;
            parser.addInput(testQuery);

            Query queryObj;

            queryObj.addDeclaration("a", DesignEntity::ASSIGN);
            queryObj.addDeclaration("v", DesignEntity::VARIABLE);

            parser.parsePattern(queryObj);

            std::vector<Pattern> patterns = queryObj.getPattern();
            Pattern pattern = patterns[0];

            REQUIRE(pattern.getSynonym() == "a");
            REQUIRE(pattern.getSynonymType() == DesignEntity::ASSIGN);
            bool validDeclaration = 
                (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
            REQUIRE(validDeclaration);
            REQUIRE(pattern.getExpression() == ExpSpec::ofWildcard());
        }

        SECTION ("With string expression") {
            std::string testQuery = "a (v, \"x\")";

            Parser parser;
            parser.addInput(testQuery);

            Query queryObj;

            queryObj.addDeclaration("a", DesignEntity::ASSIGN);
            queryObj.addDeclaration("v", DesignEntity::VARIABLE);
            parser.parsePattern(queryObj);

            std::vector<Pattern> patterns = queryObj.getPattern();
            Pattern pattern = patterns[0];

            REQUIRE(pattern.getSynonym() == "a");
            REQUIRE(pattern.getSynonymType() == DesignEntity::ASSIGN);
            bool validDeclaration = 
                (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
            REQUIRE(validDeclaration);
            REQUIRE(pattern.getExpression() == ExpSpec::ofFullMatch("x"));
        }

        SECTION("With partial submatch argument") {
            std::string testQuery = "a (v, _\"x\"_)";

            Parser parser;
            parser.addInput(testQuery);

            Query queryObj;

            queryObj.addDeclaration("a", DesignEntity::ASSIGN);
            queryObj.addDeclaration("v", DesignEntity::VARIABLE);
            parser.parsePattern(queryObj);

            std::vector<Pattern> patterns = queryObj.getPattern();
            Pattern pattern = patterns[0];

            REQUIRE(pattern.getSynonym() == "a");
            REQUIRE(pattern.getSynonymType() == DesignEntity::ASSIGN);
            bool validDeclaration = 
                (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
            REQUIRE(validDeclaration);
            REQUIRE(pattern.getExpression() == ExpSpec::ofPartialMatch("x"));
        }
    }

    SECTION ("parsePattern - pattern is if") {
        std::string testQuery = "ifs (v, _ , _ )";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("ifs", DesignEntity::IF);
        queryObj.addDeclaration("v", DesignEntity::VARIABLE);

        parser.parsePattern(queryObj);

        std::vector<Pattern> patterns = queryObj.getPattern();
        Pattern pattern = patterns[0];

        REQUIRE(pattern.getSynonym() == "ifs");
        REQUIRE(pattern.getSynonymType() == DesignEntity::IF);
        bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
        REQUIRE(validDeclaration);
        REQUIRE_THROWS_MATCHES(pattern.getExpression(),
                               exceptions::PqlSyntaxException,
                               Catch::Message(messages::qps::parser::notAnAssignPatternMessage));
    }

    SECTION ("parsePattern - pattern is while") {
        std::string testQuery = "w (v, _ )";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("w", DesignEntity::WHILE);
        queryObj.addDeclaration("v", DesignEntity::VARIABLE);

        parser.parsePattern(queryObj);

        std::vector<Pattern> patterns = queryObj.getPattern();
        Pattern pattern = patterns[0];

        REQUIRE(pattern.getSynonym() == "w");
        REQUIRE(pattern.getSynonymType() == DesignEntity::WHILE);
        bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
        REQUIRE(validDeclaration);
        REQUIRE_THROWS_MATCHES(pattern.getExpression(),
                               exceptions::PqlSyntaxException,
                               Catch::Message(messages::qps::parser::notAnAssignPatternMessage));
    }

    SECTION ("Invalid pattern synonym type") {
        std::string testQuery = "rd (v, _ )";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("rd", DesignEntity::READ);
        queryObj.addDeclaration("v", DesignEntity::VARIABLE);

        REQUIRE_THROWS_MATCHES(parser.parsePattern(queryObj),
                               exceptions::PqlSyntaxException,
                               Catch::Message(messages::qps::parser::notValidPatternType));
    }
}

TEST_CASE("Parser parsePatternClause") {
    SECTION ("Single pattern clause") {
        std::string testQuery = "pattern a (v, _)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("a", DesignEntity::ASSIGN);
        queryObj.addDeclaration("v", DesignEntity::VARIABLE);

        parser.parsePatternClause(queryObj);

        std::vector<Pattern> patterns = queryObj.getPattern();
        REQUIRE(patterns.size() == 1);

        Pattern pattern1 = patterns[0];

        REQUIRE(pattern1.getSynonym() == "a");
        bool validDeclaration = (pattern1.getEntRef().isDeclaration()) &&
                (pattern1.getEntRef().getDeclaration() == "v");
        REQUIRE(validDeclaration);
        REQUIRE(pattern1.getExpression() == ExpSpec::ofWildcard());
    }

    SECTION ("Multi pattern clause") {
        SECTION ("Valid Multi pattern clause") {
            std::string testQuery = "pattern a (v, _) and a1 (\"x\", _)";

            Parser parser;
            parser.addInput(testQuery);

            Query queryObj;

            queryObj.addDeclaration("a", DesignEntity::ASSIGN);
            queryObj.addDeclaration("a1", DesignEntity::ASSIGN);
            queryObj.addDeclaration("v", DesignEntity::VARIABLE);

            parser.parsePatternClause(queryObj);

            std::vector<Pattern> patterns = queryObj.getPattern();
            REQUIRE(patterns.size() == 2);

            Pattern pattern1 = patterns[0];

            REQUIRE(pattern1.getSynonym() == "a");
            bool validDeclaration = (pattern1.getEntRef().isDeclaration()) &&
                                    (pattern1.getEntRef().getDeclaration() == "v");
            REQUIRE(validDeclaration);
            REQUIRE(pattern1.getExpression() == ExpSpec::ofWildcard());


            Pattern pattern2 = patterns[1];

            REQUIRE(pattern2.getSynonym() == "a1");
            validDeclaration = (pattern2.getEntRef().isVarName()) && (pattern2.getEntRef().getVariableName() == "x");
            REQUIRE(validDeclaration);
            REQUIRE(pattern2.getExpression() == ExpSpec::ofWildcard());
        }

        SECTION ("Invalid Multi pattern clause") {
            std::string testQuery = "pattern a (v, _) and pattern a1 (\"x\", _)";

            Parser parser;
            parser.addInput(testQuery);

            Query queryObj;

            queryObj.addDeclaration("a", DesignEntity::ASSIGN);
            queryObj.addDeclaration("a1", DesignEntity::ASSIGN);
            queryObj.addDeclaration("v", DesignEntity::VARIABLE);

            REQUIRE_THROWS_MATCHES(parser.parsePatternClause(queryObj), exceptions::PqlSyntaxException,
                                   Catch::Message(messages::qps::parser::declarationDoesNotExistMessage));
        }
    }
}

TEST_CASE("Parser parseStmtRef") {
    SECTION ("Valid StmtRef") {
        Parser parser;
        parser.addInput("3");
        Query query;
        StmtRef sr = parser.parseStmtRef(query);
        REQUIRE(sr.isLineNo());
        REQUIRE(sr.getLineNo() == 3);
    }

    SECTION ("Invalid StmtRef - argument is a string") {
        Parser parser;
        parser.addInput("\"x\"");
        Query query;
        REQUIRE_THROWS_MATCHES(parser.parseStmtRef(query), exceptions::PqlSyntaxException,
                               Catch::Message(messages::qps::parser::invalidStmtRefMessage));
    }

    SECTION ("Invalid StmtRef - synonym is not of statement type") {
        Parser parser;
        parser.addInput("v");
        Query query;
        query.addDeclaration("v", DesignEntity::VARIABLE);
        REQUIRE_THROWS_MATCHES(parser.parseStmtRef(query), exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::synonymNotStatementTypeMessage));
    }
}

TEST_CASE("Parser parseEntRef") {
    SECTION ("Valid EntRef") {
        Parser parser;
        parser.addInput("\"x\"");
        Query query;
        EntRef er = parser.parseEntRef(query);
        REQUIRE(er.isVarName());
        REQUIRE(er.getVariableName() == "x");
    }

    SECTION ("Invalid EntRef - argument is a number") {
        Parser parser;
        parser.addInput("3");
        Query query;
        REQUIRE_THROWS_MATCHES(parser.parseEntRef(query), exceptions::PqlSyntaxException,
                               Catch::Message(messages::qps::parser::invalidEntRefMessage));
    }

    SECTION ("Invalid EntRef - synonym is not of entity type") {
        Parser parser;
        parser.addInput("v");
        Query query;
        query.addDeclaration("v", DesignEntity::STMT);
        REQUIRE_THROWS_MATCHES(
            parser.parseEntRef(query),
            exceptions::PqlSemanticException,
            Catch::Message(messages::qps::parser::synonymNotEntityTypeMessage)
        );
    }
}

TEST_CASE("Parser parseRelRefVariables") {
    SECTION("parseRelRefVariables - Follows") {
        Query queryObj;
        Parser parser;
        parser.lexer.text = "3, 5";
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
        parser.lexer.text = "3, 10";
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
        parser.lexer.text = "3, 5";
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
        parser.lexer.text = "3, 10";
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
        parser.lexer.text = "v, v";
        REQUIRE_THROWS_MATCHES(parser.parseRelRefVariables<Follows>(queryObj, &Follows::follower, &Follows::followed),
                               exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::synonymNotStatementTypeMessage));
    }

    SECTION("parseRelRefVariables - Next") {
        Query queryObj;
        Parser parser;
        parser.lexer.text = "3, 5";
        std::shared_ptr<RelRef> ptr = parser.parseRelRefVariables<Next>(queryObj, &Next::before,
                                                                           &Next::after);
        REQUIRE(ptr.get()->getType() == RelRefType::NEXT);

        Next *nPtr = std::dynamic_pointer_cast<Next>(ptr).get();
        REQUIRE(nPtr->before.isLineNo());
        REQUIRE(nPtr->before.getLineNo() == 3);
        REQUIRE(nPtr->after.isLineNo());
        REQUIRE(nPtr->after.getLineNo() == 5);
    }

    SECTION("parseRelRefVariables - NextT") {
        Query queryObj;
        Parser parser;
        parser.lexer.text = "s1, s2";
        queryObj.addDeclaration("s1", DesignEntity::STMT);
        queryObj.addDeclaration("s2", DesignEntity::STMT);
        std::shared_ptr<RelRef> ptr = parser.parseRelRefVariables<NextT>(queryObj, &NextT::before,
                                                                        &NextT::transitiveAfter);
        REQUIRE(ptr.get()->getType() == RelRefType::NEXTT);

        NextT *nPtr = std::dynamic_pointer_cast<NextT>(ptr).get();
        REQUIRE(nPtr->before.isDeclaration());
        REQUIRE(nPtr->before.getDeclaration() == "s1");
        REQUIRE(nPtr->transitiveAfter.isDeclaration());
        REQUIRE(nPtr->transitiveAfter.getDeclaration() == "s2");
    }

    SECTION("parseRelRefVariables - Calls") {
        Query queryObj;
        Parser parser;
        parser.lexer.text = "p1, p2";
        queryObj.addDeclaration("p1", DesignEntity::PROCEDURE);
        queryObj.addDeclaration("p2", DesignEntity::PROCEDURE);

        std::shared_ptr<RelRef> ptr = parser.parseRelRefVariables<Calls>(queryObj, &Calls::caller,
                                                                            &Calls::callee);
        REQUIRE(ptr.get()->getType() == RelRefType::CALLS);

        Calls *cPtr = std::dynamic_pointer_cast<Calls>(ptr).get();
        REQUIRE(cPtr->caller.isDeclaration());
        REQUIRE(cPtr->caller.getDeclaration() == "p1");
        REQUIRE(cPtr->callee.isDeclaration());
        REQUIRE(cPtr->callee.getDeclaration() == "p2");
    }

    SECTION("parseRelRefVariables - Calls*") {
        Query queryObj;
        Parser parser;
        parser.lexer.text = "\"first\", \"second\"";

        std::shared_ptr<RelRef> ptr = parser.parseRelRefVariables<CallsT>(queryObj, &CallsT::caller,
                                                                         &CallsT::transitiveCallee);
        REQUIRE(ptr.get()->getType() == RelRefType::CALLST);

        CallsT *cPtr = std::dynamic_pointer_cast<CallsT>(ptr).get();
        REQUIRE(cPtr->caller.isVarName());
        REQUIRE(cPtr->caller.getVariableName() == "first");
        REQUIRE(cPtr->transitiveCallee.isVarName());
        REQUIRE(cPtr->transitiveCallee.getVariableName() == "second");
    }
}

TEST_CASE("Parser parseModifiesOrUsesVariables") {
    Query queryObj;
    Parser parser;

    SECTION("Valid ModifiesS Rel Variables") {
        parser.lexer.text = "3, \"x\"";
        std::shared_ptr<RelRef> ptr = parser.parseModifiesOrUsesVariables(queryObj, TokenType::MODIFIES);
        REQUIRE(ptr.get()->getType() == RelRefType::MODIFIESS);

        std::shared_ptr<ModifiesS> sharedPtr = std::dynamic_pointer_cast<ModifiesS>(ptr);
        ModifiesS *mPtr = sharedPtr.get();
        REQUIRE(mPtr->modifiesStmt.isLineNo());
        REQUIRE(mPtr->modifiesStmt.getLineNo() == 3);
        REQUIRE(mPtr->modified.isVarName());
        REQUIRE(mPtr->modified.getVariableName() == "x");
    }

    SECTION("Invalid ModifiesS Rel Variables - Wildcard in first argument") {
        parser.lexer.text = "_, \"x\"";
        REQUIRE_THROWS_MATCHES(parser.parseModifiesOrUsesVariables(queryObj, TokenType::MODIFIES),
                               exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::cannotBeWildcardMessage));
    }

    SECTION("Invalid ModifiesS Rel Variables - synonym does not belong to variable") {
        parser.lexer.text = "3, x";
        queryObj = {};
        queryObj.addDeclaration("x", DesignEntity::PROCEDURE);
        REQUIRE_THROWS_MATCHES(parser.parseModifiesOrUsesVariables(queryObj, TokenType::MODIFIES),
                               exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::notVariableSynonymMessage));
    }

    SECTION("Valid ModifiesP Rel Variables - First argument is of design entity procedure") {
        parser.lexer.text = "p, _";
        queryObj = {};
        queryObj.addDeclaration("p", DesignEntity::PROCEDURE);

        std::shared_ptr<RelRef> ptr = parser.parseModifiesOrUsesVariables(queryObj, TokenType::MODIFIES);
        REQUIRE(ptr.get()->getType() == RelRefType::MODIFIESP);

        std::shared_ptr<ModifiesP> sharedPtr = std::dynamic_pointer_cast<ModifiesP>(ptr);
        ModifiesP *mPtr = sharedPtr.get();
        REQUIRE(mPtr->modifiesProc.isDeclaration());
        REQUIRE(mPtr->modifiesProc.getDeclaration() == "p");
        REQUIRE(mPtr->modified.isWildcard());
    }

    SECTION("Valid UsesP Rel Variables - First argument is of string (Procedure name) ") {
        parser.lexer.text = "\"Monke\", _";
        queryObj = {};

        std::shared_ptr<RelRef> ptr = parser.parseModifiesOrUsesVariables(queryObj, TokenType::MODIFIES);
        REQUIRE(ptr.get()->getType() == RelRefType::MODIFIESP);

        std::shared_ptr<ModifiesP> sharedPtr = std::dynamic_pointer_cast<ModifiesP>(ptr);
        ModifiesP *mPtr = sharedPtr.get();
        REQUIRE(mPtr->modifiesProc.isVarName());
        REQUIRE(mPtr->modifiesProc.getVariableName() == "Monke");
        REQUIRE(mPtr->modified.isWildcard());
    }


    SECTION("Valid UsesS Rel Variables") {
        parser.lexer.text = "3, \"x\"";
        std::shared_ptr<RelRef> ptr = parser.parseModifiesOrUsesVariables(queryObj, TokenType::USES);
        REQUIRE(ptr.get()->getType() == RelRefType::USESS);

        std::shared_ptr<UsesS> sharedPtr = std::dynamic_pointer_cast<UsesS>(ptr);
        UsesS *uPtr = sharedPtr.get();
        REQUIRE(uPtr->useStmt.isLineNo());
        REQUIRE(uPtr->useStmt.getLineNo() == 3);
        REQUIRE(uPtr->used.isVarName());
        REQUIRE(uPtr->used.getVariableName() == "x");
    }

    SECTION("Invalid UsesS Rel Variables - Wildcard in first argument") {
        parser.lexer.text = "_, \"x\"";
        REQUIRE_THROWS_MATCHES(parser.parseModifiesOrUsesVariables(queryObj, TokenType::USES),
                               exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::cannotBeWildcardMessage));
    }

    SECTION("Invalid UsesS Rel Variables - synonym does not belong to variable") {
        parser.lexer.text = "3, x";
        queryObj = {};
        queryObj.addDeclaration("x", DesignEntity::PROCEDURE);
        REQUIRE_THROWS_MATCHES(parser.parseModifiesOrUsesVariables(queryObj, TokenType::USES),
                               exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::notVariableSynonymMessage));
    }

    SECTION("Valid UsesP Rel Variables - First argument is of design entity procedure") {
        parser.lexer.text = "p, _";
        queryObj = {};
        queryObj.addDeclaration("p", DesignEntity::PROCEDURE);

        std::shared_ptr<RelRef> ptr = parser.parseModifiesOrUsesVariables(queryObj, TokenType::USES);
        REQUIRE(ptr.get()->getType() == RelRefType::USESP);

        std::shared_ptr<UsesP> sharedPtr = std::dynamic_pointer_cast<UsesP>(ptr);
        UsesP *uPtr = sharedPtr.get();
        REQUIRE(uPtr->useProc.isDeclaration());
        REQUIRE(uPtr->useProc.getDeclaration() == "p");
        REQUIRE(uPtr->used.isWildcard());
    }

    SECTION("Valid UsesP Rel Variables - First argument is of string (Procedure name) ") {
        parser.lexer.text = "\"Monke\", _";
        queryObj = {};

        std::shared_ptr<RelRef> ptr = parser.parseModifiesOrUsesVariables(queryObj, TokenType::USES);
        REQUIRE(ptr.get()->getType() == RelRefType::USESP);

        std::shared_ptr<UsesP> sharedPtr = std::dynamic_pointer_cast<UsesP>(ptr);
        UsesP *uPtr = sharedPtr.get();
        REQUIRE(uPtr->useProc.isVarName());
        REQUIRE(uPtr->useProc.getVariableName() == "Monke");
        REQUIRE(uPtr->used.isWildcard());
    }
}

TEST_CASE("Parser parseRelRefHelper") {
    SECTION("UseS RelRef") {
        Query queryObj;
        queryObj.addDeclaration("s", DesignEntity::STMT);
        Parser parser;
        parser.lexer.text = "Uses(s,_)";
        std::shared_ptr<RelRef> ptr = parser.parseRelRefHelper(queryObj);
        REQUIRE(ptr.get()->getType() == RelRefType::USESS);

        std::shared_ptr<UsesS> sharedPtr = std::dynamic_pointer_cast<UsesS>(ptr);
        UsesS *uPtr = sharedPtr.get();
        REQUIRE(uPtr->useStmt.isDeclaration());
        REQUIRE(uPtr->used.isWildcard());
    }

    SECTION("UsesP RelRef") {
        Query queryObj;
        queryObj.addDeclaration("p", DesignEntity::PROCEDURE);
        Parser parser;
        parser.lexer.text = "Uses(p,_)";
        std::shared_ptr<RelRef> ptr = parser.parseRelRefHelper(queryObj);
        REQUIRE(ptr.get()->getType() == RelRefType::USESP);

        std::shared_ptr<UsesP> sharedPtr = std::dynamic_pointer_cast<UsesP>(ptr);
        UsesP *uPtr = sharedPtr.get();
        REQUIRE(uPtr->useProc.isDeclaration());
        REQUIRE(uPtr->used.isWildcard());
    }

    SECTION("ModifiesS RelRef") {
        Query queryObj;
        queryObj.addDeclaration("s", DesignEntity::STMT);
        Parser parser;
        parser.lexer.text = "Modifies(s,_)";
        std::shared_ptr<RelRef> ptr = parser.parseRelRefHelper(queryObj);
        REQUIRE(ptr.get()->getType() == RelRefType::MODIFIESS);

        std::shared_ptr<ModifiesS> sharedPtr = std::dynamic_pointer_cast<ModifiesS>(ptr);
        ModifiesS *mPtr = sharedPtr.get();
        REQUIRE(mPtr->modifiesStmt.isDeclaration());
        REQUIRE(mPtr->modified.isWildcard());
    }

    SECTION("ModifiesP RelRef") {
        Query queryObj;
        queryObj.addDeclaration("p", DesignEntity::PROCEDURE);
        Parser parser;
        parser.lexer.text = "Modifies(p,_)";
        std::shared_ptr<RelRef> ptr = parser.parseRelRefHelper(queryObj);
        REQUIRE(ptr.get()->getType() == RelRefType::MODIFIESP);

        std::shared_ptr<ModifiesP> sharedPtr = std::dynamic_pointer_cast<ModifiesP>(ptr);
        ModifiesP *mPtr = sharedPtr.get();
        REQUIRE(mPtr->modifiesProc.isDeclaration());
        REQUIRE(mPtr->modified.isWildcard());
    }
}

TEST_CASE("Parser parseAttrRef") {
    Query query;
    query.addDeclaration("s", DesignEntity::STMT);
    query.addDeclaration("v", DesignEntity::VARIABLE);
    query.addDeclaration("p", DesignEntity::PROCEDURE);
    query.addDeclaration("c", DesignEntity::CONSTANT);

    AttrRef ar;
    Parser parser;

    SECTION("stmt#") {
        parser.lexer.text = "s.stmt#";
        ar = parser.parseAttrRef(query);
        REQUIRE(ar.declaration == "s");
        REQUIRE(ar.declarationType == DesignEntity::STMT);
        REQUIRE(ar.attrName == AttrName::STMTNUM);
    }

    SECTION ("varName") {
        parser.lexer.text = "v.varName";
        ar = parser.parseAttrRef(query);
        REQUIRE(ar.declaration == "v");
        REQUIRE(ar.declarationType == DesignEntity::VARIABLE);
        REQUIRE(ar.attrName == AttrName::VARNAME);
    }

    SECTION ("procName") {
        parser.lexer.text = "p.procName";
        ar = parser.parseAttrRef(query);
        REQUIRE(ar.declaration == "p");
        REQUIRE(ar.declarationType == DesignEntity::PROCEDURE);
        REQUIRE(ar.attrName == AttrName::PROCNAME);
    }

    SECTION ("value") {
        parser.lexer.text = "c.value";
        ar = parser.parseAttrRef(query);
        REQUIRE(ar.declaration == "c");
        REQUIRE(ar.declarationType == DesignEntity::CONSTANT);
        REQUIRE(ar.attrName == AttrName::VALUE);
    }

    SECTION ("Invalid - whitespace before and after .") {
        parser.lexer.text = "c .value";
        REQUIRE_THROWS_MATCHES(parser.parseAttrRef(query), exceptions::PqlSyntaxException,
                               Catch::Message(messages::qps::parser::unexpectedWhitespaceMessage));

        parser.lexer.text = "c. value";
        REQUIRE_THROWS_MATCHES(parser.parseAttrRef(query), exceptions::PqlSyntaxException,
                               Catch::Message(messages::qps::parser::unexpectedWhitespaceMessage));
    }

    SECTION ("Invalid - Design Entity does not have the specified AttrRef") {
        parser.lexer.text = "v.stmt#";
        REQUIRE_THROWS_MATCHES(parser.parseAttrRef(query), exceptions::PqlSyntaxException,
                               Catch::Message(messages::qps::parser::invalidAttrNameForDesignEntity));
    }
}

TEST_CASE("Parser parseAttrCompareRef") {
    SECTION ("AttrRef") {
        Query query;
        query.addDeclaration("s", DesignEntity::STMT);

        AttrCompareRef acr;
        Parser parser;

        parser.lexer.text = "s.stmt#";
        acr = parser.parseAttrCompareRef(query);
        REQUIRE(acr.isAttrRef());
        AttrRef ar = acr.getAttrRef();
        REQUIRE(ar.declaration == "s");
        REQUIRE(ar.declarationType == DesignEntity::STMT);
        REQUIRE(ar.attrName == AttrName::STMTNUM);
    }

    SECTION ("Number") {
        Query query;
        AttrCompareRef acr;
        Parser parser;


        parser.lexer.text = "3";
        acr = parser.parseAttrCompareRef(query);
        REQUIRE(acr.isNumber());
        REQUIRE(acr.getNumber() == 3);
    }

    SECTION ("String") {
        Query query;

        AttrCompareRef acr;
        Parser parser;

        parser.lexer.text = "\"Monke\"";
        acr = parser.parseAttrCompareRef(query);
        REQUIRE(acr.isString());
        REQUIRE(acr.getString() == "Monke");
    }
}

TEST_CASE("Parser parseAttrCompare") {
    SECTION ("AttrRef on LHS and String on RHS") {
        Parser parser;
        parser.lexer.text = "p.procName = \"Monke\"";

        Query query;
        query.addDeclaration("p", DesignEntity::PROCEDURE);

        parser.parseAttrCompare(query);

        REQUIRE(!query.getWith().empty());
        AttrCompare ac = query.getWith()[0];
        AttrCompareRef lhs = ac.getLhs();
        AttrCompareRef rhs = ac.getRhs();
        REQUIRE(lhs.isAttrRef());
        REQUIRE(rhs.isString());
    }

    SECTION ("Number on LHS and AttRef on Rhs") {
        Parser parser;
        parser.lexer.text = "3 = rd.stmt#";

        Query query;
        query.addDeclaration("rd", DesignEntity::READ);

        parser.parseAttrCompare(query);

        REQUIRE(!query.getWith().empty());
        AttrCompare ac = query.getWith()[0];
        AttrCompareRef lhs = ac.getLhs();
        AttrCompareRef rhs = ac.getRhs();
        REQUIRE(lhs.isNumber());
        REQUIRE(rhs.isAttrRef());
    }

    SECTION ("Number on LHS and AttRef on Rhs") {
        Parser parser;
        parser.lexer.text = "& = rd.stmt#";

        Query query;
        query.addDeclaration("rd", DesignEntity::READ);

        REQUIRE_THROWS_MATCHES(parser.parseAttrCompare(query), exceptions::PqlSyntaxException,
                               Catch::Message(messages::qps::parser::invalidAttrCompRefMessage));
    }
}

TEST_CASE("Parser parseWithClause") {
    SECTION ("Single With clause") {
        Parser parser;
        parser.lexer.text = "with p.procName = \"Monke\"";

        Query query;
        query.addDeclaration("p", DesignEntity::PROCEDURE);

        parser.parseWithClause(query);

        REQUIRE(query.getWith().size() == 1);
        AttrCompare ac = query.getWith()[0];
        AttrCompareRef lhs = ac.getLhs();
        AttrCompareRef rhs = ac.getRhs();
        REQUIRE(lhs.isAttrRef());
        REQUIRE(rhs.isString());
    }

    SECTION ("Multi With clause") {
        SECTION ("Valid Multi With clause") {
            Parser parser;
            parser.lexer.text = "with p.procName = \"Monke\" and 3 = s.stmt#";

            Query query;
            query.addDeclaration("p", DesignEntity::PROCEDURE);
            query.addDeclaration("s", DesignEntity::STMT);

            parser.parseWithClause(query);

            REQUIRE(query.getWith().size() == 2);
            AttrCompare ac = query.getWith()[0];
            AttrCompareRef lhs = ac.getLhs();
            AttrCompareRef rhs = ac.getRhs();
            REQUIRE(lhs.isAttrRef());
            REQUIRE(rhs.isString());

            AttrCompare ac2 = query.getWith()[1];
            AttrCompareRef lhs2 = ac2.getLhs();
            AttrCompareRef rhs2 = ac2.getRhs();
            REQUIRE(lhs2.isNumber());
            REQUIRE(rhs2.isAttrRef());
        }

        SECTION ("Invalid Multi With clause") {
            Parser parser;
            parser.lexer.text = "with p.procName = \"Monke\" and with 3 = s.stmt#";

            Query query;
            query.addDeclaration("p", DesignEntity::PROCEDURE);
            query.addDeclaration("s", DesignEntity::STMT);

            REQUIRE_THROWS_MATCHES(parser.parseWithClause(query), exceptions::PqlSyntaxException,
                                   Catch::Message(messages::qps::parser::declarationDoesNotExistMessage));
        }
    }
}

TEST_CASE("Parser parseQuery") {
    Parser parser;
    parser.lexer.text = "Select c";
    Query query;
    query.addDeclaration("c", DesignEntity::CONSTANT);
    parser.parseQuery(query);

    REQUIRE(query.hasVariable("c"));

    parser.lexer.text = "Select a such that Modifies (a, v1) pattern a1 (v, _\"x\"_) with a.stmt# = 3";
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

    std::shared_ptr<ModifiesS> modifiesPtr = std::dynamic_pointer_cast<ModifiesS>(relRefPtr);
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

    // Check with
    std::vector<AttrCompare> withLst = query.getWith();
    REQUIRE(!withLst.empty());
    AttrCompare with = withLst[0];

    REQUIRE(with.getLhs().isAttrRef());
    REQUIRE(with.getRhs().isNumber());
}

TEST_CASE("Parser parsePql") {
    SECTION ("Valid pql query") {
        std::string testQuery = "assign a, a1; variable v; Select a such that Modifies (a, _) pattern a (v, _\"x\"_) "
                                "with \"Monke\" = v.varName";

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

        std::shared_ptr<ModifiesS> modifiesPtr = std::dynamic_pointer_cast<ModifiesS>(relRefPtr);
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

        std::vector<AttrCompare> withLst = queryObj.getWith();
        REQUIRE(!withLst.empty());
        AttrCompare with = withLst[0];

        REQUIRE(with.getLhs().isString());
        REQUIRE(with.getRhs().isAttrRef());
    }

    SECTION ("Valid pql query with different order of clauses") {
        std::string testQuery = "assign a, a1; variable v; Select a pattern a (v, _\"x\"_) "
                                "with \"Monke\" = v.varName such that Modifies (a, _)";

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

        // Check pattern
        std::vector<Pattern> patterns = queryObj.getPattern();
        Pattern pattern = patterns[0];

        REQUIRE(pattern.getSynonym() == "a");
        bool validDeclaration = (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclaration() == "v");
        REQUIRE(validDeclaration);
        REQUIRE(pattern.getExpression() == ExpSpec::ofPartialMatch("x"));

        // Check with
        std::vector<AttrCompare> withLst = queryObj.getWith();
        REQUIRE(!withLst.empty());
        AttrCompare with = withLst[0];

        REQUIRE(with.getLhs().isString());
        REQUIRE(with.getRhs().isAttrRef());

        // Check such that
        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::MODIFIESS);

        std::shared_ptr<ModifiesS> modifiesPtr = std::dynamic_pointer_cast<ModifiesS>(relRefPtr);
        REQUIRE(modifiesPtr->modifiesStmt.getDeclaration() == "a");
        REQUIRE(modifiesPtr->modifiesStmt.isDeclaration());
        REQUIRE(modifiesPtr->modified.isWildcard());
    }
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
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("s", DesignEntity::STMT)));
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("a", DesignEntity::ASSIGN)));
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("pn", DesignEntity::PRINT)));
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("r", DesignEntity::READ)));
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("ifs", DesignEntity::IF)));
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("w", DesignEntity::WHILE)));
    REQUIRE(p.isValidStatementType(query, StmtRef::ofDeclaration("cl", DesignEntity::CALL)));

    // Invalid statement types
    REQUIRE(!(p.isValidStatementType(query, StmtRef::ofDeclaration("v", DesignEntity::VARIABLE))));
    REQUIRE(!(p.isValidStatementType(query, StmtRef::ofDeclaration("p", DesignEntity::PROCEDURE))));
    REQUIRE(!(p.isValidStatementType(query, StmtRef::ofDeclaration("c", DesignEntity::CONSTANT))));

    // Declarations which do not exist
    REQUIRE_THROWS_MATCHES(p.isValidStatementType(query, StmtRef::ofDeclaration("x", qps::query::DesignEntity::ASSIGN)),
                           exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::declarationDoesNotExistMessage));
}




