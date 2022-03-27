#include <sstream>

#include "pql/parser.h"

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
    using qps::query::Affects;
    using qps::query::AffectsT;
    using qps::query::Pattern;
    using qps::query::ExpSpec;
    using qps::query::AttrName;
    using qps::query::AttrRef;
    using qps::query::AttrCompareRef;
    using qps::query::AttrCompare;
    using qps::query::Declaration;

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
            stmtRef = StmtRef::ofDeclaration(Declaration {token.getText(), queryObj.getDeclarationDesignEntity(token.getText())});

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
            entRef = EntRef::ofDeclaration(Declaration {token.getText(), queryObj.getDeclarationDesignEntity(token.getText())});

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

    std::shared_ptr<RelRef> Parser::parseRelRefHelper(Query &queryObj) {
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
        } else if (tokenType == TokenType::AFFECTS) {
            ptr =  parseRelRefVariables<Affects>(queryObj, &Affects::affectingStmt, &Affects::affected);
        } else if (tokenType == TokenType::AFFECTS_T) {
            ptr =  parseRelRefVariables<AffectsT>(queryObj, &AffectsT::affectingStmt, &AffectsT::transitiveAffected);
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
            DesignEntity d = query.getDeclarationDesignEntity(s.getDeclarationSynonym());
            return statementsType.find(d) != statementsType.end();
        }
        return false;
    }

    bool Parser::isValidEntityType(Query &query, EntRef e) {
        std::unordered_set<DesignEntity> entityTypes {
                DesignEntity::PROCEDURE, DesignEntity::VARIABLE
        };
        if (e.isDeclaration()) {
            DesignEntity d = e.getDeclarationType();
            return entityTypes.find(d) != entityTypes.end();
        }
        return false;
    }

    ExpSpec Parser::parseExpSpec() {
        bool hasString = false;
        bool hasWildcard = false;
        std::string value = "";

        if (peekNextToken().getTokenType() == TokenType::UNDERSCORE) {
            hasWildcard = true;
            getNextToken();
        }

        if (peekNextToken().getTokenType() == TokenType::STRING) {
            hasString = true;
            Token token = getAndCheckNextToken(TokenType::STRING);
            value = token.getText();
            validateExpr(value);
        }

        if (hasString && hasWildcard) {
            getAndCheckNextToken(TokenType::UNDERSCORE);
            return ExpSpec::ofPartialMatch(value);
        }

        if (hasWildcard)
            return ExpSpec::ofWildcard();

        if (hasString)
            return ExpSpec::ofFullMatch(value);

        throw exceptions::PqlSyntaxException("Unable to parse pattern");
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

        if (tt == TokenType::END_OF_FILE) {
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

    void Parser::parseRelRef(Query &query) {
        std::shared_ptr<RelRef> relRef = parseRelRefHelper(query);
        query.addSuchthat(relRef);
    }

    void Parser::parseSuchThatClause(Query &queryObj) {
        getAndCheckNextReservedToken(TokenType::SUCH_THAT);
        parseRelRef(queryObj);

        while (peekNextReservedToken().getTokenType() == TokenType::AND) {
            getAndCheckNextReservedToken(TokenType::AND);
            parseRelRef(queryObj);
        }
    }

    EntRef Parser::parsePatternLhs(Query &query, std::string synonym) {
        EntRef e = parseEntRef(query);


        if (e.isDeclaration() && e.getDeclarationType() != DesignEntity::VARIABLE)
            throw exceptions::PqlSemanticException(messages::qps::parser::notVariableSynonymMessage);

        return e;
    }

    Pattern Parser::parsePatternVariables(Query &query, std::string synonym, DesignEntity de) {
        Pattern p;
        getAndCheckNextToken(TokenType::OPENING_PARAN);
        EntRef e = parsePatternLhs(query, synonym);

        if (de == DesignEntity::ASSIGN) {
            getAndCheckNextToken(TokenType::COMMA);
            ExpSpec expression = parseExpSpec();
            p = Pattern::ofAssignPattern(synonym, e, expression);
        } else {
            int wildcardCount = (de == DesignEntity::IF) ? 2 : 1;

            while (wildcardCount > 0) {
                getAndCheckNextToken(TokenType::COMMA);
                getAndCheckNextToken(TokenType::UNDERSCORE);
                wildcardCount--;
            }

            p = (de == DesignEntity::IF) ? Pattern::ofIfPattern(synonym, e) : Pattern::ofWhilePattern(synonym, e);
        }

        getAndCheckNextToken(TokenType::CLOSING_PARAN);
        return p;
    }

    void Parser::parsePattern(Query &query) {
        Token t = getAndCheckNextToken(TokenType::IDENTIFIER);
        std::string declarationName = t.getText();
        DesignEntity de = query.getDeclarationDesignEntity(declarationName);
        if ((de != DesignEntity::ASSIGN) && (de != DesignEntity::IF) && (de != DesignEntity::WHILE))
            throw exceptions::PqlSyntaxException(messages::qps::parser::notValidPatternType);

        query.addPattern(parsePatternVariables(query, declarationName, de));
    }

    void Parser::parsePatternClause(Query &queryObj) {
        getAndCheckNextReservedToken(TokenType::PATTERN);

        parsePattern(queryObj);

        while (peekNextReservedToken().getTokenType() == TokenType::AND) {
            getAndCheckNextReservedToken(TokenType::AND);
            parsePattern(queryObj);
        }
    }

    AttrRef Parser::parseAttrRef(Query &query) {
        Token identifier = getAndCheckNextToken(TokenType::IDENTIFIER);

        std::string synonym = identifier.getText();
        DesignEntity de = query.getDeclarationDesignEntity(synonym);

        getAndCheckNextToken(TokenType::PERIOD);

        Token attrRefToken = getNextReservedToken();
        auto pos = tokenTypeToAttrName.find(attrRefToken.getTokenType());
        AttrName attrName = pos->second;
        checkDesignEntityAndAttrNameMatch(de, attrName);

        Declaration declaration(synonym, de);

        return AttrRef { attrName, declaration };
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

    void Parser::parseAttrCompare(Query &query) {
        auto lhs = parseAttrCompareRef(query);
        getAndCheckNextToken(TokenType::EQUAL);
        auto rhs = parseAttrCompareRef(query);
        AttrCompare ac = AttrCompare(lhs, rhs);
        ac.validateComparingTypes();
        query.addWith(ac);
    }

    void Parser::parseWithClause(Query &query) {
        getAndCheckNextReservedToken(TokenType::WITH);
        parseAttrCompare(query);

        while (peekNextReservedToken().getTokenType() == TokenType::AND) {
            getAndCheckNextReservedToken(TokenType::AND);
            parseAttrCompare(query);
        }
    }

    void Parser::parseQuery(Query &queryObj) {
        parseSelectFields(queryObj);

        TokenType tt = peekNextReservedToken().getTokenType();
        while (tt != TokenType::END_OF_FILE) {
            if (tt == TokenType::SUCH_THAT) {
                parseSuchThatClause(queryObj);
            } else if (tt == TokenType::PATTERN) {
                parsePatternClause(queryObj);
            } else if (tt == TokenType::WITH) {
                parseWithClause(queryObj);
            } else {
                throw exceptions::PqlSyntaxException(messages::qps::parser::notExpectingTokenMessage);
            }

            tt = peekNextReservedToken().getTokenType();
        }
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
