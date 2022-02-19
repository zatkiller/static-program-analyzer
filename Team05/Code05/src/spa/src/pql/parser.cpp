#include <sstream>

#include "pql/parser.h"
#include "logging.h"

namespace qps::parser {
    using qps::query::designEntityMap;
    using qps::query::Uses;
    using qps::query::Modifies;
    using qps::query::Follows;
    using qps::query::FollowsT;
    using qps::query::Parent;
    using qps::query::ParentT;
    using qps::query::Pattern;
    using qps::query::ExpSpec;

    std::string Parser::getParsedText() {
        return lexer.text;
    }

    Token Parser::getNextToken() {
        return lexer.getNextToken();
    }

    Token Parser::peekNextToken() {
        return lexer.peekNextToken();
    }

    Token Parser::getNextReservedToken() {
        return lexer.getNextReservedToken();
    }

    Token Parser::peekNextReservedToken() {
        return lexer.peekNextReservedToken();
    }

    void Parser::checkType(Token token, TokenType tokenType) {
        if (token.getTokenType() != tokenType)
            throw exceptions::PqlSyntaxException(messages::qps::parser::notExpectingTokenMessage);
    }

    Token Parser::getAndCheckNextToken(TokenType tt) {
        Token token = getNextToken();
        checkType(token, tt);
        return token;
    }

    Token Parser::peekAndCheckNextToken(TokenType tt) {
        Token token = peekNextToken();
        checkType(token, tt);
        return token;
    }

    Token Parser::getAndCheckNextReservedToken(TokenType tokenType) {
        Token token = getNextReservedToken();
        checkType(token, tokenType);
        return token;
    }

    Token Parser::peekAndCheckNextReservedToken(TokenType tokenType) {
        Token token = peekNextReservedToken();
        checkType(token, tokenType);
        return token;
    }

    void Parser::parseDeclaration(Query &queryObj, DesignEntity de) {
        Token variable = getAndCheckNextToken(TokenType::IDENTIFIER);
        queryObj.addDeclaration(variable.getText(), de);
    }

    void Parser::parseDeclarations(Query &queryObj) {
        Token token = getAndCheckNextToken(TokenType::IDENTIFIER);
        auto iterator = designEntityMap.find(token.getText());
        if (iterator == designEntityMap.end())
            throw exceptions::PqlSyntaxException(messages::qps::parser::noSuchDesignEntityMessage);

        DesignEntity designEntity = iterator->second;
        parseDeclaration(queryObj, designEntity);
        // Parse and add single declaration to Query Object

        while (peekNextToken().getTokenType() == TokenType::COMMA) {
            getAndCheckNextToken(TokenType::COMMA);
            parseDeclaration(queryObj, designEntity);
        }

        getAndCheckNextToken(TokenType::SEMICOLON);
    }

    void Parser::addPql(std::string query) {
        lexer = Lexer(query);
    }

    void Parser::parseSelectFields(Query &queryObj) {
        getAndCheckNextReservedToken(TokenType::SELECT);

        Token t = getAndCheckNextToken(TokenType::IDENTIFIER);
        std::string name = t.getText();

        if (!queryObj.hasDeclaration(name))
            throw exceptions::PqlSyntaxException(messages::qps::parser::declarationDoesNotExistMessage);

        queryObj.addVariable(name);
    }

    bool Parser::isStmtRef(Token token, Query &queryObj) {
        TokenType type = token.getTokenType();
        if (type == TokenType::NUMBER || type == TokenType::UNDERSCORE)
            return true;

        if (type == TokenType::IDENTIFIER && queryObj.hasDeclaration(token.getText()))
            return true;

        return false;
    }

    bool Parser::isEntRef(Token token, Query &queryObj) {
        TokenType type = token.getTokenType();
        if (type == TokenType::STRING || type == TokenType::UNDERSCORE)
            return true;

        if (type == TokenType::IDENTIFIER && queryObj.hasDeclaration(token.getText()))
            return true;

        return false;
    }

    StmtRef Parser::parseStmtRef(Query &queryObj) {
        Token token = getNextToken();
        TokenType type = token.getTokenType();
        StmtRef stmtRef;
        if (type == TokenType::NUMBER) {
            int lineNo;
            std::stringstream ss(token.getText());
            ss >> lineNo;
            stmtRef = StmtRef::ofLineNo(lineNo);
        } else if (type == TokenType::UNDERSCORE) {
            stmtRef = StmtRef::ofWildcard();
        } else if (type == TokenType::IDENTIFIER) {
            stmtRef = StmtRef::ofDeclaration(token.getText());
        }

        return stmtRef;
    }

    EntRef Parser::parseEntRef(Query &queryObj) {
        Token token = getNextToken();
        TokenType type = token.getTokenType();
        EntRef entRef;
        if (type == TokenType::STRING) {
            entRef = EntRef::ofVarName(token.getText());
        } else if (type == TokenType::UNDERSCORE) {
            entRef = EntRef::ofWildcard();
        } else if (type == TokenType::IDENTIFIER) {
            entRef = EntRef::ofDeclaration(token.getText());
        }

        return entRef;
    }

    std::shared_ptr<RelRef> Parser::parseModifiesOrUsesVariables(Query &queryObj, TokenType tt) {
        getAndCheckNextToken(TokenType::OPENING_PARAN);

        if (!isStmtRef(peekNextToken(), queryObj))
            throw exceptions::PqlSyntaxException(messages::qps::parser::invalidStmtRefMessage);

        StmtRef sr = parseStmtRef(queryObj);

        if (sr.isWildcard())
            throw exceptions::PqlSemanticException(messages::qps::parser::cannotBeWildcardMessage);

        getAndCheckNextToken(TokenType::COMMA);

        if (!isEntRef(peekNextToken(), queryObj))
            throw exceptions::PqlSyntaxException(messages::qps::parser::invalidEntRefMessage);

        EntRef er = parseEntRef(queryObj);

        if (er.isDeclaration() && queryObj.getDeclarationDesignEntity(er.getDeclaration()) != DesignEntity::VARIABLE)
            throw exceptions::PqlSemanticException(messages::qps::parser::notVariableSynonymMessage);

        getAndCheckNextToken(TokenType::CLOSING_PARAN);

        if (tt == TokenType::USES) {
            std::shared_ptr<Uses> usesPtr = std::make_shared<Uses>();
            usesPtr->used = er;
            usesPtr->useStmt = sr;
            return usesPtr;
        } else {
            std::shared_ptr<Modifies> modifiesPtr = std::make_shared<Modifies>();
            modifiesPtr->modifiesStmt = sr;
            modifiesPtr->modified = er;
            return modifiesPtr;
        }
    }

    std::shared_ptr<RelRef> Parser::parseRelRef(Query &queryObj) {
        TokenType tokenType = getNextReservedToken().getTokenType();

        if ((tokenType == TokenType::USES) || (tokenType == TokenType::MODIFIES)) {
            return parseModifiesOrUsesVariables(queryObj, tokenType);
        } else if (tokenType == TokenType::FOLLOWS) {
            return parseRelRefVariables<Follows>(queryObj, &Follows::follower, &Follows::followed);
        } else if (tokenType == TokenType::FOLLOWS_T) {
            return parseRelRefVariables<FollowsT>(queryObj, &FollowsT::follower, &FollowsT::transitiveFollowed);
        } else if (tokenType == TokenType::PARENT) {
            return parseRelRefVariables<Parent>(queryObj, &Parent::parent, &Parent::child);
        } else if (tokenType == TokenType::PARENT_T) {
            return parseRelRefVariables<ParentT>(queryObj, &ParentT::parent, &ParentT::transitiveChild);
        }

        throw exceptions::PqlSyntaxException(messages::qps::parser::invalidRelRefMessage);
    }

    bool Parser::isValidStatementType(Query &query, StmtRef s) {
        std::unordered_set<DesignEntity> statementsType {
            DesignEntity::STMT, DesignEntity::ASSIGN,
            DesignEntity::WHILE, DesignEntity::IF,
            DesignEntity::PRINT, DesignEntity::READ, DesignEntity::CALL
        };
        if (s.isDeclaration()) {
            DesignEntity d = query.getDeclarationDesignEntity(s.getDeclaration());
            return statementsType.find(d) != statementsType.end();
        }
        return true;
    }

    ExpSpec Parser::parseExpSpec() {
        bool isPartialMatch = false;
        bool isWildcard = false;
        std::string value = "";

        if (peekNextToken().getTokenType() == TokenType::UNDERSCORE) {
            isWildcard = true;
            getNextToken();
        }

        if (peekNextToken().getTokenType() == TokenType::STRING) {
            if (isWildcard) {
                isPartialMatch = true;
                isWildcard = false;
            }
            Token token = getAndCheckNextToken(TokenType::STRING);
            value = token.getText();
        }

        if (isPartialMatch) {
            Token t = peekNextToken();
            getAndCheckNextToken(TokenType::UNDERSCORE);
            return ExpSpec::ofPartialMatch(value);
        }

        if (isWildcard)
            return ExpSpec::ofWildcard();

        return ExpSpec::ofFullMatch(value);
    }

    void Parser::parseSuchThat(Query &queryObj) {
        getAndCheckNextReservedToken(TokenType::SUCH_THAT);
        std::shared_ptr<RelRef> relRef = parseRelRef(queryObj);
        queryObj.addSuchthat(relRef);
    }

    void Parser::parsePattern(Query &queryObj) {
        getAndCheckNextReservedToken(TokenType::PATTERN);

        Token t = getAndCheckNextToken(TokenType::IDENTIFIER);
        std::string declarationName = t.getText();

        if (queryObj.getDeclarationDesignEntity(declarationName) != DesignEntity::ASSIGN)
            throw exceptions::PqlSyntaxException(messages::qps::parser::notAnAssignmentMessage);

        Pattern p;

        p.synonym = declarationName;
        getAndCheckNextToken(TokenType::OPENING_PARAN);
        EntRef e = parseEntRef(queryObj);

        if (queryObj.getDeclarationDesignEntity(e.getDeclaration()) != DesignEntity::VARIABLE)
            throw exceptions::PqlSemanticException(messages::qps::parser::notVariableSynonymMessage);

        p.lhs = e;
        getAndCheckNextToken(TokenType::COMMA);
        p.expression = parseExpSpec();
        getAndCheckNextToken(TokenType::CLOSING_PARAN);

        queryObj.addPattern(p);
    }


    void Parser::parseQuery(Query &queryObj) {
        parseSelectFields(queryObj);

        if (peekNextReservedToken().getTokenType() == TokenType::SUCH_THAT)
            parseSuchThat(queryObj);

        if (peekNextReservedToken().getTokenType() == TokenType::PATTERN)
            parsePattern(queryObj);
    }

    Query Parser::parsePql(std::string query) {
        addPql(query);
        Query queryObj;

        try {
            for (Token token = peekNextReservedToken(); token.getTokenType() != TokenType::END_OF_FILE;
                 token = peekNextReservedToken()) {
                if (token.getTokenType() != TokenType::SELECT) {
                    // Parse delcarations first
                    parseDeclarations(queryObj);
                } else {
                    // Start parsing the actual query
                    parseQuery(queryObj);
                }
            }
        } catch (exceptions::PqlException) {
            queryObj.setValid(false);
        }

        return queryObj;
    }
}  // namespace qps::parser
