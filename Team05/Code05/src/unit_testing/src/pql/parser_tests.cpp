#include "QPS/Parser.h"
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
using qps::query::ResultCl;
using qps::query::StmtRef;
using qps::query::Elem;
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
using qps::query::Affects;
using qps::query::AffectsT;
using qps::query::Pattern;
using qps::query::ExpSpec;
using qps::query::AttrRef;
using qps::query::AttrName;
using qps::query::AttrCompareRef;
using qps::query::AttrCompare;
using qps::query::Declaration;

TEST_CASE("Parser checkType") {
    Parser parser;
    Token token = Token{ "", TokenType::INVALID };
    REQUIRE_NOTHROW(parser.checkType(token, TokenType::INVALID));

    Token token2 = Token{ "", TokenType::STRING };
    REQUIRE_THROWS_MATCHES(parser.checkType(token2, TokenType::INVALID), exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::notExpectingTokenMessage));
}

TEST_CASE("Parser checkDesignEntityAndAttrNameMatch") {
    Parser parser;
    REQUIRE_THROWS_MATCHES(parser.checkDesignEntityAndAttrNameMatch(DesignEntity::CONSTANT, AttrName::PROCNAME),
                           exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::invalidAttrNameForDesignEntity));
}

TEST_CASE("Parser addInput") {
    std::string testQuery1 = "assign a; \n Select a such that Uses (a, v) pattern a (v, _)";
    std::string testQuery2 = "variable v; \n Select v such that Uses (a, v) pattern a (v, _)";

    Parser parser;
    parser.addInput(testQuery1);

    Lexer lexer1 = parser.lexer;
    REQUIRE(parser.lexer.text == testQuery1);

    parser.addInput(testQuery2);

    REQUIRE(parser.lexer.text == testQuery2);
    REQUIRE(!(lexer1 == parser.lexer));
}

TEST_CASE("Parser parseDeclarationStmts") {
    Parser parser;
    parser.lexer.text = "assign a;\n";

    Query queryObj;
    parser.parseDeclarationStmts(queryObj);
    REQUIRE(queryObj.hasDeclaration("a"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a") == DesignEntity::ASSIGN);

    // Multiple declarations
    queryObj = Query{};
    parser.lexer.text = "assign a, a1;\n";
    parser.parseDeclarationStmts(queryObj);

    REQUIRE(queryObj.hasDeclaration("a"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a") == DesignEntity::ASSIGN);

    REQUIRE(queryObj.hasDeclaration("a1"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a1") == DesignEntity::ASSIGN);

    // Multiple declarations of different Design Entities
    queryObj = Query{};
    parser.lexer.text = "assign a, a1; variable v; call cl;";
    parser.parseDeclarationStmts(queryObj);

    REQUIRE(queryObj.hasDeclaration("a"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a") == DesignEntity::ASSIGN);

    REQUIRE(queryObj.hasDeclaration("a1"));
    REQUIRE(queryObj.getDeclarationDesignEntity("a1") == DesignEntity::ASSIGN);

    parser.parseDeclarationStmts(queryObj);
    REQUIRE(queryObj.hasDeclaration("v"));
    REQUIRE(queryObj.getDeclarationDesignEntity("v") == DesignEntity::VARIABLE);

    parser.parseDeclarationStmts(queryObj);
    REQUIRE(queryObj.hasDeclaration("cl"));
    REQUIRE(queryObj.getDeclarationDesignEntity("cl") == DesignEntity::CALL);
}

TEST_CASE("Parser parseAttrName") {
    Parser parser;
    Query query;

    parser.addInput("stmt#");
    query.addDeclaration("a", DesignEntity::ASSIGN);
    Declaration d = Declaration { "a", DesignEntity::ASSIGN };
    AttrName attrName = parser.parseAttrName(query, d);

    REQUIRE(attrName == AttrName::STMTNUM);
}

TEST_CASE("Parser parseElem") {
    SECTION("Elem is synonym") {
        Parser parser;
        Query query;

        parser.addInput("a");
        query.addDeclaration("a", DesignEntity::ASSIGN);
        Elem e = parser.parseElem(query);

        REQUIRE(e.isDeclaration());
        REQUIRE(e.getDeclaration() == Declaration { "a", DesignEntity::ASSIGN });
    }

    SECTION("Elem is AttrRef") {
        Parser parser;
        Query query;

        parser.addInput("a.stmt#");
        query.addDeclaration("a", DesignEntity::ASSIGN);
        Elem e = parser.parseElem(query);

        REQUIRE(e.isAttrRef());
        REQUIRE(e.getAttrRef() == AttrRef { AttrName::STMTNUM, Declaration { "a", DesignEntity::ASSIGN }});
    }
}

TEST_CASE("Parser parseTuple") {
    SECTION("select field is single elem") {
        Parser parser;
        Query query;

        parser.addInput("a");
        query.addDeclaration("a", DesignEntity::ASSIGN);
        std::vector<Elem> selectFields = parser.parseTuple(query);

        REQUIRE(selectFields.size() == 1);
        Elem e = selectFields[0];

        REQUIRE(e.isDeclaration());
        REQUIRE(e.getDeclaration() ==  Declaration { "a", DesignEntity::ASSIGN });
    }

    SECTION("select field is single elem with brackets") {
        Parser parser;
        Query query;

        parser.addInput("<a.stmt#>");
        query.addDeclaration("a", DesignEntity::ASSIGN);
        std::vector<Elem> selectFields = parser.parseTuple(query);

        REQUIRE(selectFields.size() == 1);
        Elem e = selectFields[0];

        REQUIRE(e.isAttrRef());
        REQUIRE(e.getAttrRef() == AttrRef { AttrName::STMTNUM, Declaration { "a", DesignEntity::ASSIGN }});
    }

    SECTION("select field is multiple elem with brackets") {
        Parser parser;
        Query query;

        parser.addInput("<a.stmt#, a, cl, cl.procName>");
        query.addDeclaration("a", DesignEntity::ASSIGN);
        query.addDeclaration("cl", DesignEntity::CALL);
        std::vector<Elem> selectFields = parser.parseTuple(query);

        REQUIRE(selectFields.size() == 4);

        REQUIRE(selectFields[0].isAttrRef());
        REQUIRE(selectFields[0].getAttrRef() == AttrRef { AttrName::STMTNUM,
                                                          Declaration { "a", DesignEntity::ASSIGN }});

        REQUIRE(selectFields[1].isDeclaration());
        REQUIRE(selectFields[1].getDeclaration() ==  Declaration { "a", DesignEntity::ASSIGN });

        REQUIRE(selectFields[2].isDeclaration());
        REQUIRE(selectFields[2].getDeclaration() ==  Declaration { "cl", DesignEntity::CALL });

        REQUIRE(selectFields[3].isAttrRef());
        REQUIRE(selectFields[3].getAttrRef() == AttrRef { AttrName::PROCNAME,
                                                          Declaration { "cl", DesignEntity::CALL }});
    }
}

TEST_CASE("Parser parseSelectFields") {
    SECTION ("parse Boolean") {
        Parser parser;
        parser.addInput("Select BOOLEAN");

        Query query;
        parser.parseSelectFields(query);

        ResultCl r = query.getResultCl();
        REQUIRE(r.isBoolean());
    }

    SECTION ("parse Tuple - Single synonym") {
        Parser parser;
        parser.addInput("Select a");

        Query query;
        query.addDeclaration("a", DesignEntity::ASSIGN);
        parser.parseSelectFields(query);

        ResultCl r = query.getResultCl();
        std::vector<Elem> tuple = r.getTuple();
        REQUIRE(tuple.size() == 1);

        Elem e = tuple[0];
        REQUIRE(e.isDeclaration());
    }

    SECTION ("parse Tuple") {
        Parser parser;
        parser.addInput("Select <a, a.stmt#, rd>");

        Query query;
        query.addDeclaration("a", DesignEntity::ASSIGN);
        query.addDeclaration("rd", DesignEntity::READ);
        parser.parseSelectFields(query);

        ResultCl r = query.getResultCl();
        std::vector<Elem> tuple = r.getTuple();
        REQUIRE(tuple.size() == 3);

        REQUIRE(tuple[0].isDeclaration());
        REQUIRE(tuple[1].isAttrRef());
        REQUIRE(tuple[2].isDeclaration());
    }
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
        REQUIRE(usesPtr->useStmt.getDeclarationSynonym() == "a");
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
        REQUIRE(modifiesPtr->modifiesStmt.getDeclarationSynonym() == "a");
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
        REQUIRE(followsPtr->follower.getDeclarationSynonym() == "s1");
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
        REQUIRE(followsPtr->follower.getDeclarationSynonym() == "s2");
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
        REQUIRE(parentPtr->child.getDeclarationSynonym() == "s2");
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
        REQUIRE(parentPtr->transitiveChild.getDeclarationSynonym() == "s2");
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
        REQUIRE(nPtr->before.getDeclarationSynonym() == "s1");
        REQUIRE(nPtr->after.isDeclaration());
        REQUIRE(nPtr->after.getDeclarationSynonym() == "s2");
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
        REQUIRE(nPtr->before.getDeclarationSynonym() == "s1");
        REQUIRE(nPtr->transitiveAfter.isDeclaration());
        REQUIRE(nPtr->transitiveAfter.getDeclarationSynonym() == "s2");
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
        REQUIRE(cPtr->caller.getDeclarationSynonym() == "p1");
        REQUIRE(cPtr->callee.isDeclaration());
        REQUIRE(cPtr->callee.getDeclarationSynonym() == "p2");
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
        REQUIRE(cPtr->caller.getDeclarationSynonym() == "p1");
        REQUIRE(cPtr->transitiveCallee.isDeclaration());
        REQUIRE(cPtr->transitiveCallee.getDeclarationSynonym() == "p2");
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

TEST_CASE("Parser parseRelRef - Affects") {
    SECTION ("Valid query with Affects relationship") {
        std::string testQuery = "Affects(a1, a2)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("a1", DesignEntity::ASSIGN);
        queryObj.addDeclaration("a2", DesignEntity::ASSIGN);

        parser.parseRelRef(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::AFFECTS);

        std::shared_ptr<Affects> aPtr = std::dynamic_pointer_cast<Affects>(relRefPtr);
        REQUIRE(aPtr->affectingStmt.isDeclaration());
        REQUIRE(aPtr->affectingStmt.getDeclarationSynonym() == "a1");
        REQUIRE(aPtr->affected.isDeclaration());
        REQUIRE(aPtr->affected.getDeclarationSynonym() == "a2");
    }

    SECTION ("Invalid query with Affects relationship - synonym is not assign") {
        std::string testQuery = "Affects(a, s)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("a", DesignEntity::ASSIGN);
        queryObj.addDeclaration("s", DesignEntity::STMT);

        REQUIRE_THROWS_MATCHES(parser.parseRelRef(queryObj), exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::notAssignSynonymMessage));
    }
}

TEST_CASE("Parser parseRelRef - Affects*") {
    SECTION ("Valid query with Affects* relationship") {
        std::string testQuery = "Affects*(a1, a2)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("a1", DesignEntity::ASSIGN);
        queryObj.addDeclaration("a2", DesignEntity::ASSIGN);

        parser.parseRelRef(queryObj);

        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::AFFECTST);

        std::shared_ptr<AffectsT> aPtr = std::dynamic_pointer_cast<AffectsT>(relRefPtr);
        REQUIRE(aPtr->affectingStmt.isDeclaration());
        REQUIRE(aPtr->affectingStmt.getDeclarationSynonym() == "a1");
        REQUIRE(aPtr->transitiveAffected.isDeclaration());
        REQUIRE(aPtr->transitiveAffected.getDeclarationSynonym() == "a2");
    }

    SECTION ("Invalid query with Affects* relationship - synonym is not assign") {
        std::string testQuery = "Affects*(a, cl)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("a", DesignEntity::ASSIGN);
        queryObj.addDeclaration("cl", DesignEntity::CALL);

        REQUIRE_THROWS_MATCHES(parser.parseRelRef(queryObj), exceptions::PqlSemanticException,
                               Catch::Message(messages::qps::parser::notAssignSynonymMessage));
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
            REQUIRE(cPtr->caller.getDeclarationSynonym() == "p1");
            REQUIRE(cPtr->transitiveCallee.isDeclaration());
            REQUIRE(cPtr->transitiveCallee.getDeclarationSynonym() == "p2");
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
            REQUIRE(cPtr->caller.getDeclarationSynonym() == "p1");
            REQUIRE(cPtr->transitiveCallee.isDeclaration());
            REQUIRE(cPtr->transitiveCallee.getDeclarationSynonym() == "p2");

            std::shared_ptr<RelRef> relRefPtr2 = suchThat[1];
            REQUIRE(relRefPtr2->getType() == RelRefType::NEXTT);

            std::shared_ptr<NextT> nPtr = std::dynamic_pointer_cast<NextT>(relRefPtr2);
            REQUIRE(nPtr->before.isDeclaration());
            REQUIRE(nPtr->before.getDeclarationSynonym() == "s1");
            REQUIRE(nPtr->transitiveAfter.isDeclaration());
            REQUIRE(nPtr->transitiveAfter.getDeclarationSynonym() == "s2");
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
    ExpSpec e = parser.parseExpSpec();

    REQUIRE(e.isWildcard());
    REQUIRE(!e.isPartialMatch());
    REQUIRE(!e.isFullMatch());

    testQuery = "_\"x\"_";
    parser.addInput(testQuery);

    e = parser.parseExpSpec();
    REQUIRE(e.isPartialMatch());
    REQUIRE(!e.isWildcard());
    REQUIRE(!e.isFullMatch());

    REQUIRE(e.getPattern() == "x");


    testQuery = "\"y\"";
    parser.addInput(testQuery);

    e = parser.parseExpSpec();
    REQUIRE(!e.isPartialMatch());
    REQUIRE(!e.isWildcard());
    REQUIRE(e.isFullMatch());

    REQUIRE(e.getPattern() == "y");

    testQuery = "_\"x\"";
    parser.addInput(testQuery);
    REQUIRE_THROWS_MATCHES(parser.parseExpSpec(), exceptions::PqlSyntaxException,
                           Catch::Message(messages::qps::parser::notExpectingTokenMessage));
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
                (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclarationSynonym() == "v");
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
                (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclarationSynonym() == "v");
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
                (pattern.getEntRef().isDeclaration()) && (pattern.getEntRef().getDeclarationSynonym() == "v");
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
        bool validDeclaration = (pattern.getEntRef().isDeclaration()) &&
                (pattern.getEntRef().getDeclarationSynonym() == "v");
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
        bool validDeclaration = (pattern.getEntRef().isDeclaration()) &&
                (pattern.getEntRef().getDeclarationSynonym() == "v");
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

    SECTION("Invalid pattern variables") {
        std::string testQuery = "w (v, _, _)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("w", DesignEntity::WHILE);
        queryObj.addDeclaration("v", DesignEntity::VARIABLE);

        REQUIRE_THROWS_MATCHES(parser.parsePattern(queryObj),
            exceptions::PqlSyntaxException,
            Catch::Message(messages::qps::parser::notExpectingTokenMessage));
    }

    SECTION("Invalid pattern variables") {
        std::string testQuery = "a (v, _, _)";

        Parser parser;
        parser.addInput(testQuery);

        Query queryObj;

        queryObj.addDeclaration("a", DesignEntity::ASSIGN);
        queryObj.addDeclaration("v", DesignEntity::VARIABLE);

        REQUIRE_THROWS_MATCHES(parser.parsePattern(queryObj),
            exceptions::PqlSyntaxException,
            Catch::Message(messages::qps::parser::notExpectingTokenMessage));
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
                (pattern1.getEntRef().getDeclarationSynonym() == "v");
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
                                    (pattern1.getEntRef().getDeclarationSynonym() == "v");
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
        REQUIRE(nPtr->before.getDeclarationSynonym() == "s1");
        REQUIRE(nPtr->transitiveAfter.isDeclaration());
        REQUIRE(nPtr->transitiveAfter.getDeclarationSynonym() == "s2");
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
        REQUIRE(cPtr->caller.getDeclarationSynonym() == "p1");
        REQUIRE(cPtr->callee.isDeclaration());
        REQUIRE(cPtr->callee.getDeclarationSynonym() == "p2");
    }

    SECTION("parseRelRefVariables - Calls*") {
        Query queryObj;
        Parser parser;
        parser.lexer.text = R"("first", "second")";

        std::shared_ptr<RelRef> ptr = parser.parseRelRefVariables<CallsT>(queryObj, &CallsT::caller,
                                                                         &CallsT::transitiveCallee);
        REQUIRE(ptr.get()->getType() == RelRefType::CALLST);

        CallsT *cPtr = std::dynamic_pointer_cast<CallsT>(ptr).get();
        REQUIRE(cPtr->caller.isVarName());
        REQUIRE(cPtr->caller.getVariableName() == "first");
        REQUIRE(cPtr->transitiveCallee.isVarName());
        REQUIRE(cPtr->transitiveCallee.getVariableName() == "second");
    }

    SECTION("parseRelRefVariables - Affects") {
        Query queryObj;
        Parser parser;
        parser.lexer.text = "a1, a2";
        queryObj.addDeclaration("a1", DesignEntity::ASSIGN);
        queryObj.addDeclaration("a2", DesignEntity::ASSIGN);

        std::shared_ptr<RelRef> ptr = parser.parseRelRefVariables<Affects>(queryObj, &Affects::affectingStmt,
            &Affects::affected);
        REQUIRE(ptr.get()->getType() == RelRefType::AFFECTS);

        Affects* cPtr = std::dynamic_pointer_cast<Affects>(ptr).get();
        REQUIRE(cPtr->affectingStmt.isDeclaration());
        REQUIRE(cPtr->affectingStmt.getDeclarationSynonym() == "a1");
        REQUIRE(cPtr->affected.isDeclaration());
        REQUIRE(cPtr->affected.getDeclarationSynonym() == "a2");
    }

    SECTION("parseRelRefVariables - Affects*") {
        Query queryObj;
        Parser parser;
        parser.lexer.text = "a1, a2";
        queryObj.addDeclaration("a1", DesignEntity::ASSIGN);
        queryObj.addDeclaration("a2", DesignEntity::ASSIGN);

        std::shared_ptr<RelRef> ptr = parser.parseRelRefVariables<AffectsT>(queryObj, &AffectsT::affectingStmt,
            &AffectsT::transitiveAffected);
        REQUIRE(ptr.get()->getType() == RelRefType::AFFECTST);

        AffectsT* cPtr = std::dynamic_pointer_cast<AffectsT>(ptr).get();
        REQUIRE(cPtr->affectingStmt.isDeclaration());
        REQUIRE(cPtr->affectingStmt.getDeclarationSynonym() == "a1");
        REQUIRE(cPtr->transitiveAffected.isDeclaration());
        REQUIRE(cPtr->transitiveAffected.getDeclarationSynonym() == "a2");
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
        REQUIRE(mPtr->modifiesProc.getDeclarationSynonym() == "p");
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
        REQUIRE(uPtr->useProc.getDeclarationSynonym() == "p");
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
        REQUIRE(ar.getDeclaration()  == Declaration { "s", DesignEntity::STMT} );
        REQUIRE(ar.getAttrName() == AttrName::STMTNUM);
    }

    SECTION ("varName") {
        parser.lexer.text = "v.varName";
        ar = parser.parseAttrRef(query);
        REQUIRE(ar.getDeclaration() == Declaration { "v", DesignEntity::VARIABLE } );
        REQUIRE(ar.getAttrName() == AttrName::VARNAME);
    }

    SECTION ("procName") {
        parser.lexer.text = "p.procName";
        ar = parser.parseAttrRef(query);
        REQUIRE(ar.getDeclaration() == Declaration { "p", DesignEntity::PROCEDURE });
        REQUIRE(ar.getAttrName() == AttrName::PROCNAME);
    }

    SECTION ("value") {
        parser.lexer.text = "c.value";
        ar = parser.parseAttrRef(query);
        REQUIRE(ar.getDeclaration()  == Declaration { "c", DesignEntity::CONSTANT });
        REQUIRE(ar.getAttrName() == AttrName::VALUE);
    }

    SECTION ("Valid - whitespace before and after .") {
        parser.lexer.text = "c .value";
        REQUIRE_NOTHROW(parser.parseAttrRef(query));

        parser.lexer.text = "c. value";
        REQUIRE_NOTHROW(parser.parseAttrRef(query));
    }

    SECTION ("Invalid - Design Entity does not have the specified AttrRef") {
        parser.lexer.text = "v.stmt#";
        REQUIRE_THROWS_MATCHES(parser.parseAttrRef(query), exceptions::PqlSyntaxException,
                               Catch::Message(messages::qps::parser::invalidAttrNameForDesignEntity));
    }
}

TEST_CASE("validateComparingTypes") {
    SECTION("Incompatible type matches") {
        Parser p;
        REQUIRE_THROWS_MATCHES(
            p.validateComparingTypes(
                AttrCompareRef::ofAttrRef(
                    AttrRef{ AttrName::STMTNUM, Declaration {"rd", DesignEntity::READ } }), 
                    AttrCompareRef::ofString("v")
            ), 
            exceptions::PqlSemanticException,
            Catch::Message(messages::qps::parser::incompatibleComparisonMessage)
        );

        REQUIRE_THROWS_MATCHES(
            p.validateComparingTypes(
                AttrCompareRef::ofNumber(1), 
                AttrCompareRef::ofAttrRef(
                    AttrRef{ AttrName::VARNAME, Declaration { "v", DesignEntity::VARIABLE } }
                )
            ), 
            exceptions::PqlSemanticException,
            Catch::Message(messages::qps::parser::incompatibleComparisonMessage)
        );

        REQUIRE_THROWS_MATCHES(p.validateComparingTypes(AttrCompareRef::ofString("v"), AttrCompareRef::ofNumber(1)), 
            exceptions::PqlSemanticException, Catch::Message(messages::qps::parser::incompatibleComparisonMessage));
    }

    SECTION("Compatible type matches") {
        Parser p;

        REQUIRE_NOTHROW(p.validateComparingTypes(AttrCompareRef::ofAttrRef(AttrRef{ AttrName::PROCNAME, Declaration {
            "p", DesignEntity::PROCEDURE } }), AttrCompareRef::ofString("v")));

        REQUIRE_NOTHROW(p.validateComparingTypes(AttrCompareRef::ofString("v"), AttrCompareRef::ofAttrRef(
            AttrRef{ AttrName::VARNAME, Declaration { "v", DesignEntity::VARIABLE, } })));

        REQUIRE_NOTHROW(p.validateComparingTypes(AttrCompareRef::ofAttrRef(AttrRef{ AttrName::STMTNUM,  Declaration {
            "cl", DesignEntity::CALL} }), AttrCompareRef::ofNumber(200)));

        REQUIRE_NOTHROW(p.validateComparingTypes(AttrCompareRef::ofNumber(99), AttrCompareRef::ofAttrRef(
            AttrRef{ AttrName::VALUE, Declaration { "c", DesignEntity::CONSTANT } })));

        REQUIRE_NOTHROW(p.validateComparingTypes(AttrCompareRef::ofAttrRef(AttrRef{ AttrName::PROCNAME, Declaration {
            "p", DesignEntity::PROCEDURE } }), AttrCompareRef::ofAttrRef(AttrRef{
                AttrName::VARNAME, Declaration { "v", DesignEntity::VARIABLE } })));

        REQUIRE_NOTHROW(p.validateComparingTypes(AttrCompareRef::ofAttrRef(AttrRef{ AttrName::STMTNUM, Declaration {
            "cl", DesignEntity::CALL } }), AttrCompareRef::ofAttrRef(AttrRef{
                AttrName::VALUE, Declaration { "c", DesignEntity::CONSTANT } })));
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
        REQUIRE(ar.getDeclaration() == Declaration { "s", DesignEntity::STMT });
        REQUIRE(ar.getAttrName() == AttrName::STMTNUM);
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

TEST_CASE("Parser parsePql") {
    SECTION ("Valid QPS query") {
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
        Elem e = Elem::ofDeclaration( Declaration { "a", DesignEntity::ASSIGN });
        REQUIRE(queryObj.hasSelectElem(e));

        // Check such that
        std::shared_ptr<RelRef> relRefPtr = (queryObj.getSuchthat())[0];
        REQUIRE(relRefPtr->getType() == RelRefType::MODIFIESS);

        std::shared_ptr<ModifiesS> modifiesPtr = std::dynamic_pointer_cast<ModifiesS>(relRefPtr);
        REQUIRE(modifiesPtr->modifiesStmt.getDeclarationSynonym() == "a");
        REQUIRE(modifiesPtr->modifiesStmt.isDeclaration());
        REQUIRE(modifiesPtr->modified.isWildcard());

        // Check pattern
        std::vector<Pattern> patterns = queryObj.getPattern();
        Pattern pattern = patterns[0];

        REQUIRE(pattern.getSynonym() == "a");
        bool validDeclaration = (pattern.getEntRef().isDeclaration()) &&
                (pattern.getEntRef().getDeclarationSynonym() == "v");
        REQUIRE(validDeclaration);
        REQUIRE(pattern.getExpression() == ExpSpec::ofPartialMatch("x"));

        std::vector<AttrCompare> withLst = queryObj.getWith();
        REQUIRE(!withLst.empty());
        AttrCompare with = withLst[0];

        REQUIRE(with.getLhs().isString());
        REQUIRE(with.getRhs().isAttrRef());
    }

    SECTION ("Valid QPS query with different order of clauses") {
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
        Elem e = Elem::ofDeclaration( Declaration { "a", DesignEntity::ASSIGN });
        REQUIRE(queryObj.hasSelectElem(e));

        // Check pattern
        std::vector<Pattern> patterns = queryObj.getPattern();
        Pattern pattern = patterns[0];

        REQUIRE(pattern.getSynonym() == "a");
        bool validDeclaration = (pattern.getEntRef().isDeclaration()) &&
                (pattern.getEntRef().getDeclarationSynonym() == "v");
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
        REQUIRE(modifiesPtr->modifiesStmt.getDeclarationSynonym() == "a");
        REQUIRE(modifiesPtr->modifiesStmt.isDeclaration());
        REQUIRE(modifiesPtr->modified.isWildcard());
    }
}



