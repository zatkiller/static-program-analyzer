#include <sstream>

#include "pql/parser.h"
#include "logging.h"

namespace qps::parser {
    using qps::query::designEntityMap;
    using qps::query::attrNameToDesignEntityMap;
    using qps::query::UsesS;
    using qps::query::UsesP;
    using qps::query::ModifiesS;
    using qps::query::ModifiesP;
    using qps::query::Follows;
    using qps::query::FollowsT;
    using qps::query::Parent;
    using qps::query::ParentT;
    using qps::query::Next;
    using qps::query::NextT;
    using qps::query::Calls;
    using qps::query::CallsT;
    using qps::query::Pattern;
    using qps::query::ExpSpec;
    using qps::query::AttrName;
    using qps::query::AttrRef;
    using qps::query::AttrCompareRef;
    using qps::query::AttrCompare;

    std::unordered_map<TokenType, AttrName> tokenTypeToAttrName = {
            { TokenType::PROCNAME, AttrName::PROCNAME },
            { TokenType::VARNAME, AttrName::VARNAME },
            { TokenType::VALUE, AttrName::VALUE },
            { TokenType::STMTNUM, AttrName::STMTNUM }
    };

    bool Parser::hasLeadingWhitespace() {
        return lexer.hasLeadingWhitespace();
    }

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

    void Parser::checkDesignEntityAndAttrNameMatch(DesignEntity de, AttrName an) {
        auto designEntitySet = attrNameToDesignEntityMap.find(an)->second;
        if (designEntitySet.count(de) == 0)
            throw exceptions::PqlSyntaxException(messages::qps::parser::invalidAttrNameForDesignEntity);
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

    void Parser::addInput(std::string query) {
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
            stmtRef = StmtRef::ofDeclaration(token.getText(), queryObj.getDeclarationDesignEntity(token.getText()));

            if (!isValidStatementType(queryObj, stmtRef))
                throw exceptions::PqlSemanticException(messages::qps::parser::synonymNotStatementTypeMessage);

        } else {
            throw exceptions::PqlSyntaxException(messages::qps::parser::invalidStmtRefMessage);
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
            entRef = EntRef::ofDeclaration(token.getText(), queryObj.getDeclarationDesignEntity(token.getText()));

            if (!isValidEntityType(queryObj, entRef))
                throw exceptions::PqlSemanticException(messages::qps::parser::synonymNotEntityTypeMessage);

        } else {
            throw exceptions::PqlSyntaxException(messages::qps::parser::invalidEntRefMessage);
        }

        return entRef;
    }

    bool usePVariant(Token t, Query &query) {
        TokenType tt = t.getTokenType();
        if (tt == TokenType::STRING) {
            return true;
        } else if (tt == TokenType::NUMBER) {
            return false;
        } else if (tt == TokenType::IDENTIFIER) {
            auto de = query.getDeclarationDesignEntity(t.getText());
            return de == DesignEntity::PROCEDURE;
        }

        return false;
    }

    std::shared_ptr<RelRef> Parser::parseModifiesOrUsesVariables(Query &query, TokenType tt) {
        Token t1 = peekNextToken();
        bool isPVariant = usePVariant(t1, query);

        if (isPVariant && tt == TokenType::MODIFIES) {
            return parseRelRefVariables<ModifiesP>(query, &ModifiesP::modifiesProc, &ModifiesP::modified);
        } else if (isPVariant && tt == TokenType::USES) {
            return parseRelRefVariables<UsesP>(query, &UsesP::useProc, &UsesP::used);
        } else if (!isPVariant && tt == TokenType::MODIFIES) {
            return parseRelRefVariables<ModifiesS>(query, &ModifiesS::modifiesStmt, &ModifiesS::modified);
        } else if (!isPVariant && tt == TokenType::USES) {
            return parseRelRefVariables<UsesS>(query, &UsesS::useStmt, &UsesS::used);
        } else {
            throw exceptions::PqlSyntaxException("Satisfy none of the modifies and uses relref variants");
        }
    }

    std::shared_ptr<RelRef> Parser::parseRelRef(Query &queryObj) {
        TokenType tokenType = getNextReservedToken().getTokenType();
        getAndCheckNextToken(TokenType::OPENING_PARAN);

        std::shared_ptr<RelRef> ptr;

        if ((tokenType == TokenType::USES) || (tokenType == TokenType::MODIFIES)) {
            ptr = parseModifiesOrUsesVariables(queryObj, tokenType);
        } else if (tokenType == TokenType::FOLLOWS) {
            ptr =  parseRelRefVariables<Follows>(queryObj, &Follows::follower, &Follows::followed);
        } else if (tokenType == TokenType::FOLLOWS_T) {
            ptr =  parseRelRefVariables<FollowsT>(queryObj, &FollowsT::follower, &FollowsT::transitiveFollowed);
        } else if (tokenType == TokenType::PARENT) {
            ptr =  parseRelRefVariables<Parent>(queryObj, &Parent::parent, &Parent::child);
        } else if (tokenType == TokenType::PARENT_T) {
            ptr =  parseRelRefVariables<ParentT>(queryObj, &ParentT::parent, &ParentT::transitiveChild);
        } else if (tokenType == TokenType::NEXT) {
            ptr =  parseRelRefVariables<Next>(queryObj, &Next::before, &Next::after);
        } else if (tokenType == TokenType::NEXT_T) {
            ptr =  parseRelRefVariables<NextT>(queryObj, &NextT::before, &NextT::transitiveAfter);
        } else if (tokenType == TokenType::CALLS) {
            ptr =  parseRelRefVariables<Calls>(queryObj, &Calls::caller, &Calls::callee);
        } else if (tokenType == TokenType::CALLS_T) {
            ptr =  parseRelRefVariables<CallsT>(queryObj, &CallsT::caller, &CallsT::transitiveCallee);
        } else {
            throw exceptions::PqlSyntaxException(messages::qps::parser::invalidRelRefMessage);
        }

        getAndCheckNextToken(TokenType::CLOSING_PARAN);
        return ptr;
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
        return false;
    }

    bool Parser::isValidEntityType(Query &query, EntRef e) {
        std::unordered_set<DesignEntity> entityTypes {
                DesignEntity::PROCEDURE, DesignEntity::VARIABLE
        };
        if (e.isDeclaration()) {
            DesignEntity d = query.getDeclarationDesignEntity(e.getDeclaration());
            return entityTypes.find(d) != entityTypes.end();
        }
        return false;
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
            validateExpr(value);
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

    void Parser::validateExpr(std::string expr) {
        Parser expParser;
        expParser.addInput(expr);
        expParser.parseExpr();
    }

    void Parser::parseExpr() {
        parseCurrentExpr();
        parseNextExpr(0);
    }

    void Parser::parseCurrentExpr() {
        Token token = getNextToken();
        TokenType tt = token.getTokenType();

        if ((tt == TokenType::END_OF_FILE)) {
            throw exceptions::PqlSyntaxException(messages::qps::parser::expressionUnexpectedEndMessage);
        } else if ((tt == TokenType::NUMBER) || (tt == TokenType::IDENTIFIER)) {
            return;
        } else if (tt == TokenType::OPENING_PARAN) {
            parseExpr();
            getAndCheckNextToken(TokenType::CLOSING_PARAN);
        } else {
            throw exceptions::PqlSyntaxException(messages::qps::parser::expressionInvalidGrammarMessage);
        }
    }

    int Parser::getOperatorPriority(Token token) {
        TokenType type = token.getTokenType();
        if ((type == TokenType::MULTIPLY) || (type == TokenType::DIVIDE) || (type == TokenType::MODULO)) {
            return 20;
        } else if ((type == TokenType::PLUS) || (type == TokenType::MINUS)) {
            return 10;
        } else {
            return -10;
        }
    }

    void Parser::parseNextExpr(int priority) {
        if (priority == -10) {
            return;
        }

        while (true) {
            int currPriority = getOperatorPriority(peekNextToken());

            if (currPriority < priority) {
                break;
            }

            Token op = getNextToken();
            parseCurrentExpr();

            int nextPriority = getOperatorPriority(peekNextToken());
            if (nextPriority > currPriority) {
                parseNextExpr(priority + 10);
            }
        }
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

        if (e.isDeclaration() && queryObj.getDeclarationDesignEntity(e.getDeclaration()) != DesignEntity::VARIABLE)
            throw exceptions::PqlSemanticException(messages::qps::parser::notVariableSynonymMessage);

        p.lhs = e;
        getAndCheckNextToken(TokenType::COMMA);
        p.expression = parseExpSpec();
        getAndCheckNextToken(TokenType::CLOSING_PARAN);

        queryObj.addPattern(p);
    }


    AttrRef Parser::parseAttrRef(Query &query) {
        Token identifier = getAndCheckNextToken(TokenType::IDENTIFIER);

        std::string declaration = identifier.getText();
        DesignEntity de = query.getDeclarationDesignEntity(declaration);

        Parser* p = this;
        auto f = [p]() {
            p->getAndCheckNextToken(TokenType::PERIOD);
        };
        checkSurroundingWhitespace(f);

        Token attrRefToken = getNextReservedToken();
        auto pos = tokenTypeToAttrName.find(attrRefToken.getTokenType());
        AttrName attrName = pos->second;
        checkDesignEntityAndAttrNameMatch(de, attrName);

        return AttrRef { attrName, de, declaration };
    }

    AttrCompareRef Parser::parseAttrCompareRef(Query &query) {
        Token t = peekNextToken();
        TokenType tt = t.getTokenType();

        if (tt == TokenType::IDENTIFIER) {
            return AttrCompareRef::ofAttrRef(parseAttrRef(query));
        } else if (tt == TokenType::STRING) {
            getNextToken();
            return AttrCompareRef::ofString(t.getText());
        } else if (tt == TokenType::NUMBER) {
            getNextToken();
            int lineNo;
            std::stringstream ss(t.getText());
            ss >> lineNo;
            return AttrCompareRef::ofNumber(lineNo);
        } else {
            throw exceptions::PqlSyntaxException(messages::qps::parser::invalidAttrCompRefMessage);
        }
    }

    void Parser::parseWith(Query &query) {
        getAndCheckNextReservedToken(TokenType::WITH);
        auto lhs = parseAttrCompareRef(query);
        getAndCheckNextToken(TokenType::EQUAL);
        auto rhs = parseAttrCompareRef(query);
        query.addWith(AttrCompare(lhs, rhs));
    }

    void Parser::parseQuery(Query &queryObj) {
        parseSelectFields(queryObj);

        if (peekNextReservedToken().getTokenType() == TokenType::SUCH_THAT)
            parseSuchThat(queryObj);

        if (peekNextReservedToken().getTokenType() == TokenType::PATTERN)
            parsePattern(queryObj);

        if (peekNextReservedToken().getTokenType() == TokenType::WITH)
            parseWith(queryObj);
    }

    Query Parser::parsePql(std::string query) {
        addInput(query);
        Query queryObj;
        queryObj.setValid(true);

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
