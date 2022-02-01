#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <stack>
#include <queue>
#include <vector>
#include <variant>

#include "Lexer.h"
#include "Parser.h"
#include "logging.h"
#include "Parser/AST.h"

using std::make_unique;
using std::move;
using std::unique_ptr;
using std::deque;
using std::get;
using std::get_if;
using std::string;
using std::invalid_argument;
using std::cout;
using std::endl;
using std::vector;

int lineCount = 0;
AST::StmtLst parseStmtLst(deque<Token>& tokens);
unique_ptr<AST::Statement> parseWhileStmt(deque<Token>& tokens);
unique_ptr<AST::Statement> parseIfStmt(deque<Token>& tokens);
unique_ptr<AST::Statement> parseReadStmt(deque<Token>& tokens);
unique_ptr<AST::Statement> parsePrintStmt(deque<Token>& tokens);
unique_ptr<AST::Statement> parseAssignStmt(unique_ptr<AST::Var> varName, deque<Token>& tokens);

Token getNextToken(deque<Token>& tokens) {
    Token token = tokens.front();
    tokens.pop_front();
    return token;
}

/**
 * Parses constants
 */
unique_ptr<AST::Expr> parseConstExpr(deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);  // consume token
    return make_unique<AST::Const>(get<int>(currToken.value));
}

/**
 * Parses variable names
 */
unique_ptr<AST::Expr> parseVariableExpr(deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);  // consume token
    return make_unique<AST::Var>(get<string>(currToken.value));
}

AST::RelOp parseRelOp(deque<Token>& tokens) {
    Token relOpToken = getNextToken(tokens);
    char* relOp = get_if<char>(&relOpToken.value);
    if (!relOp) {
        Logger(Level::ERROR) << "RelOp Expected";
        throw invalid_argument("RelOp expected!");
    }

    Token nextToken = tokens.front();
    char* nextOp = get_if<char>(&nextToken.value);

    switch (*relOp) {
    case '>':
        // can be '>' or ">="
        if (nextOp && *nextOp == '=') {
            // we got ">=", return
            getNextToken(tokens);  // consume '='
            return AST::RelOp::GTE;
        }
        // we got '>', return
        return AST::RelOp::GT;
    case '<':
        // can be '<' or "<="
        if (nextOp && *nextOp == '=') {
            // we got "<=", return
            getNextToken(tokens);  // consume '='
            return AST::RelOp::LTE;
        }
        // we got '>', return
        return AST::RelOp::LT;
    case '=':
        // can be "=="
        if (!nextOp || *nextOp != '=') {
            Logger(Level::ERROR) << "Symbol not recognised";
            throw invalid_argument("Symbol not recognised!");
        }
        // we got "==", return
        getNextToken(tokens);  // consume '='
        return AST::RelOp::EQ;
    case '!':
        // can be "!="
        if (!nextOp || *nextOp != '=') {
            Logger(Level::ERROR) << "Symbol not recognised";
            throw invalid_argument("Symbol not recognised!");
        }
        // we got "!=", return
        getNextToken(tokens);  // consume '='
        return AST::RelOp::NE;
    default:
        // special not recognised
        Logger(Level::ERROR) << "Symbol Not Recognised";
        throw invalid_argument("Symbol not recognised!");
    }
}

AST::CondOp parseCondOp(deque<Token>& tokens) {
    // either "&&" or "||"
    Token condOpToken = getNextToken(tokens);
    char* currOp = get_if<char>(&condOpToken.value);
    if (!currOp) {
        Logger(Level::ERROR) << "CondOp Expected";
        throw invalid_argument("CondOp expected!");
    }

    condOpToken = tokens.front();
    char* nextOp = get_if<char>(&condOpToken.value);
    switch (*currOp) {
    case '&':
        if (!nextOp || *nextOp != '&') {
            Logger(Level::ERROR) << "Symbol not recognised";
            throw invalid_argument("Symbol not recognised!");
        }
        // we have "&&"
        getNextToken(tokens);  // consume the next '&'
        return AST::CondOp::AND;
    case '|':
        if (!nextOp || *nextOp != '|') {
            Logger(Level::ERROR) << "Symbol not recognised";
            throw invalid_argument("Symbol not recognised!");
        }
        // we have "||"
        getNextToken(tokens);  // consume the next '|'
        return AST::CondOp::OR;
    default:
        Logger(Level::ERROR) << "Symbol not recognised";
        throw invalid_argument("Symbol not recognised!");
    }
}

AST::BinOp parseBinOp(deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);
    char* binOp = get_if<char>(&currToken.value);
    if (!binOp) {
        Logger(Level::ERROR) << "BinOp Expected";
        throw invalid_argument("BinOp expected!");
    }

    switch (*binOp) {
    case '+':
        return AST::BinOp::PLUS;
    case '-':
        return AST::BinOp::MINUS;
    case '*':
        return AST::BinOp::MULT;
    case '/':
        return AST::BinOp::DIVIDE;
    case '%':
        return AST::BinOp::MOD;
    default:
        Logger(Level::ERROR) << "Symbol not recognised";
        throw invalid_argument("Symbol not recognised!");
    }
}

void checkOpenBracket(deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);  // consume '('
    char* openBracket = get_if<char>(&currToken.value);
    if (!openBracket || *openBracket != '(') {
        Logger(Level::ERROR) << "'(' Expected";
        throw invalid_argument("'(' expected!");
    }
}

void checkClosedBracket(deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);  // consume ')'
    char* openBracket = get_if<char>(&currToken.value);
    if (!openBracket || *openBracket != ')') {
        Logger(Level::ERROR) << "')' Expected";
        throw invalid_argument("')' expected!");
    }
}

void checkOpenBrace(deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);  // consume '('
    char* openBracket = get_if<char>(&currToken.value);
    if (!openBracket || *openBracket != '{') {
        Logger(Level::ERROR) << "'{' Expected";
        throw invalid_argument("'{' expected!");
    }
}

void checkClosedBrace(deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);  // consume ')'
    char* openBracket = get_if<char>(&currToken.value);
    if (!openBracket || *openBracket != '}') {
        Logger(Level::ERROR) << "'}' Expected";
        throw invalid_argument("'}' expected!");
    }
}

void checkSemiColon(deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);
    char* semicolon = get_if<char>(&currToken.value);
    if (!semicolon) {
        Logger(Level::ERROR) << "';' Expected";
        throw invalid_argument("';' expected!");
    }
}

unique_ptr<AST::Expr> PostFixExprParser(deque<Token> queue) {
    std::stack<unique_ptr<AST::Expr>> stack;
    while (!queue.empty()) {
        Token currToken = queue.front();
        if (currToken.type == TokenType::name) {
            unique_ptr<AST::Expr> var = parseVariableExpr(queue);
            stack.push(move(var));
        } else if (currToken.type == TokenType::number) {
            unique_ptr<AST::Expr> num = parseConstExpr(queue);
            stack.push(move(num));
        } else {
            // special char
            AST::BinOp binOp = parseBinOp(queue);
            auto lhsExpr = move(stack.top());
            stack.pop();
            auto rhsExpr = move(stack.top());
            stack.pop();
            stack.push(make_unique<AST::BinExpr>(binOp, move(lhsExpr), move(rhsExpr)));
        }
    }
    return move(stack.top());
}

unique_ptr<AST::Expr> shuntingYardParser(deque<Token>& tokens) {
    // converting from infix to postfix expr
    std::map<char, string> binOpPrecedence;
    binOpPrecedence['+'] = 10;
    binOpPrecedence['-'] = 10;
    binOpPrecedence['*'] = 20;
    binOpPrecedence['/'] = 20;
    binOpPrecedence['%'] = 20;
    deque<Token> queue;
    std::stack<Token> stack;
    bool flag = true;
    while (flag) {
        Token currToken = tokens.front();
        if (currToken.type == TokenType::number || currToken.type == TokenType::name) {
            queue.push_back(currToken);
            tokens.pop_front();
        } else if (currToken.type == TokenType::special) {
            char* specialChar = get_if<char>(&currToken.value);
            if (!specialChar) {
                Logger(Level::ERROR) << "Char Expected";
                throw invalid_argument("Char expected!");
            }

            if (*specialChar == '+' || *specialChar == '-') {
                // check other operators currently on the stack
                while (!stack.empty()) {
                    Token topToken = stack.top();
                    char* topChar = get_if<char>(&topToken.value);
                    if (!topChar) {
                        Logger(Level::ERROR) << "Char Expected";
                        throw invalid_argument("Char expected!");
                    }
                    if (*topChar == '(' || binOpPrecedence[*topChar] < binOpPrecedence[*specialChar]) {
                        break;
                    }
                    stack.pop();
                    queue.push_back(topToken);
                }
                stack.push(currToken);
                tokens.pop_front();
            } else if (*specialChar == '*' || *specialChar == '/' || *specialChar == '%' || *specialChar == '(') {
                // for '(' or the highest priority operators, we just push it onto the stack
                stack.push(currToken);
                tokens.pop_front();
            } else if (*specialChar == ')') {
                bool hasOpenParen = false;
                while (!stack.empty()) {
                    Token topToken = stack.top();
                    char* topChar = get_if<char>(&topToken.value);
                    if (!topChar) {
                        Logger(Level::ERROR) << "Char Expected";
                        throw invalid_argument("Char expected!");
                    }
                    if (*topChar == '(') {
                        hasOpenParen = true;
                        stack.pop();  // consume the '('
                        tokens.pop_front();  // consume the ')'
                        break;
                    }
                    stack.pop();
                    queue.push_back(topToken);
                }
                flag = hasOpenParen;
            } else {
                // ';' or other unrecognised symbols like condOp
                flag = false;
            }
        } else {
            Logger(Level::ERROR) << "Unexpected Termination";
            throw invalid_argument("Unexpected Termination!");
        }
    }

    while (!stack.empty()) {
        queue.push_back(stack.top());
        stack.pop();
    }

    return PostFixExprParser(queue);
}

unique_ptr<AST::Expr> parseExpr(deque<Token>& tokens) {
    // can be a name | const | binExpr | '(' expr ')'
    return shuntingYardParser(tokens);
}

unique_ptr<AST::CondExpr> parseRelExpr(deque<Token>& tokens) {
    auto lhsExpr = parseExpr(tokens);
    // the symbols '>', '>=', '<', '<=', '==', '!='
    auto relOp = parseRelOp(tokens);
    auto rhsExpr = parseExpr(tokens);
    return make_unique<AST::RelExpr>(relOp, move(lhsExpr), move(rhsExpr));
}

unique_ptr<AST::CondExpr> parseCondExpr(deque<Token>& tokens) {
    // Token currToken = getNextToken(tokens);
    Token currToken = tokens.front();
    unique_ptr<AST::CondExpr> condExprResult;
    if (currToken.type == TokenType::special) {
        // we have to "unwrap" the "(...)" or "!(...)"
        auto tokenVal = get<char>(currToken.value);
        if (tokenVal == '!') {
            // This means: "!(condExpr)"
            tokens.pop_front();  // consume the '!'
            checkOpenBracket(tokens);
            condExprResult = make_unique<AST::NotCondExpr>(move(parseCondExpr(tokens)));
            checkClosedBracket(tokens);
        } else if (tokenVal == '(') {
            int openBrCount = 0;
            bool isCondExpr = false;
            for (auto& v : tokens) {
                if (v.type == TokenType::special) {
                    char sym = get<char>(v.value);
                    switch (sym) {
                    case '(':
                        openBrCount++;
                        break;
                    case ')':
                        openBrCount--;
                        break;
                    }
                    if (openBrCount == 0 && sym != ')') {
                        if (sym == '&' || sym == '|') {
                            isCondExpr = true;
                            break;
                        }
                        break;
                    } else if (openBrCount < 0) {
                        isCondExpr = true;
                        break;
                    }
                }
            }
            if (isCondExpr) {
                checkOpenBracket(tokens);  // consume the '('
                condExprResult = move(parseCondExpr(tokens));  // consume condExpr
                checkClosedBracket(tokens);
            } else {
                return move(parseRelExpr(tokens));
            }

        } else {
            Logger(Level::ERROR) << "Symbol not recognised";
            throw invalid_argument("Symbol not recognised!");
        }
    } else {
        // here, we parse rel_expr
        return move(parseRelExpr(tokens));
    }

    // Peek and check if we have another cond_expr,
    // i.e. ‘(’ cond_expr ‘)’ ‘&&’ ‘(’ cond_expr ‘)’ or ‘(’ cond_expr ‘)’ ‘||’ ‘(’ cond_expr ‘)’
    Token checkNextCond = tokens.front();
    char* condSymbol = get_if<char>(&checkNextCond.value);
    if (!condSymbol || (*condSymbol != '&' && *condSymbol != '|')) {
        // if so we are done.
        return condExprResult;
    }
    // otherwise, we still need to handle the other clause.
    auto condOp = move(parseCondOp(tokens));
    auto rhsCondExprResult = move(parseCondExpr(tokens));
    return make_unique<AST::CondBinExpr>(condOp, move(condExprResult), move(rhsCondExprResult));
}

AST::StmtLst parseStmtLst(deque<Token>& tokens) {
    vector<unique_ptr<AST::Statement>> list;
    do {
        Token currToken = getNextToken(tokens);
        string* keyword = get_if<string>(&currToken.value);
        if (!keyword) {
            Logger(Level::ERROR) << "String Expected";
            throw invalid_argument("String expected!");
        }
        if (*keyword == "read") {
            list.push_back(move(parseReadStmt(tokens)));
        } else if (*keyword == "print") {
            list.push_back(move(parsePrintStmt(tokens)));
        } else if (*keyword == "while") {
            list.push_back(move(parseWhileStmt(tokens)));
        } else if (*keyword == "if") {
            list.push_back(move(parseIfStmt(tokens)));
        } else {
            // assign, but currToken is the varName
            auto var = make_unique<AST::Var>(*keyword);
            list.push_back(move(parseAssignStmt(move(var), tokens)));
        }
    } while (tokens.front().type != TokenType::special ||
        get<char>(tokens.front().value) != '}');
    return AST::StmtLst(list);
}

unique_ptr<AST::Statement> parseReadStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    Token currToken = getNextToken(tokens);
    string* varName = get_if<string>(&currToken.value);
    if (!varName) {
        Logger(Level::ERROR) << "Name Expected";
        throw invalid_argument("Name expected!");;
    }
    checkSemiColon(tokens);
    auto var = make_unique<AST::Var>(*varName);
    return make_unique<AST::Read>(lineNo, move(var));
}

unique_ptr<AST::Statement> parsePrintStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    Token currToken = getNextToken(tokens);
    string* varName = get_if<string>(&currToken.value);
    if (!varName) {
        Logger(Level::ERROR) << "Name Expected";
        throw invalid_argument("Name expected!");;
    }
    checkSemiColon(tokens);
    auto var = make_unique<AST::Var>(*varName);
    return make_unique<AST::Print>(lineNo, move(var));
}

unique_ptr<AST::Statement> parseAssignStmt(unique_ptr<AST::Var> var, deque<Token>& tokens) {
    int lineNo = lineCount++;
    Token currToken = getNextToken(tokens);
    char* equalSign = get_if<char>(&currToken.value);
    if (!equalSign || *equalSign != '=') {
        Logger(Level::ERROR) << "'=' Expected";
        throw invalid_argument("'=' expected!");;
    }
    auto rhsExpr = parseExpr(tokens);
    checkSemiColon(tokens);
    return make_unique<AST::Assign>(lineNo, move(var), move(rhsExpr));
}

unique_ptr<AST::Statement> parseWhileStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    checkOpenBracket(tokens);
    auto condExprResult = parseCondExpr(tokens);
    checkClosedBracket(tokens);
    checkOpenBrace(tokens);
    auto stmtLstResult = parseStmtLst(tokens);
    checkClosedBrace(tokens);
    return make_unique<AST::While>(
        lineNo, move(condExprResult), move(stmtLstResult)
        );
}

unique_ptr<AST::Statement> parseIfStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    checkOpenBracket(tokens);
    auto condExprResult = parseCondExpr(tokens);
    checkClosedBracket(tokens);
    // check for 'then'
    Token currToken = getNextToken(tokens);
    string* thenStr = get_if<string>(&currToken.value);
    if (currToken.type != TokenType::name) {
        Logger(Level::ERROR) << "\"then\" Expected";
        throw invalid_argument("\"then\" expected!");
    }
    checkOpenBrace(tokens);
    auto thenStmtLst = parseStmtLst(tokens);
    checkClosedBrace(tokens);
    // check for 'else'
    currToken = getNextToken(tokens);
    string* elseStr = get_if<string>(&currToken.value);
    if (!elseStr || *elseStr != "else") {
        Logger(Level::ERROR) << "\"else\" Expected";
        throw invalid_argument("\"else\" expected!");
    }
    checkOpenBrace(tokens);
    auto elseStmtLst = parseStmtLst(tokens);
    checkClosedBrace(tokens);
    return make_unique<AST::If>(
        lineNo,
        move(condExprResult),
        move(thenStmtLst),
        move(elseStmtLst)
        );
}

unique_ptr<AST::Procedure> parseProcedure(deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);  // consume name
    if (currToken.type != TokenType::name) {
        Logger(Level::ERROR) << "Name expected";
        throw invalid_argument("Name expected!");
    }
    string procName = get<string>(currToken.value);
    checkOpenBrace(tokens);
    auto stmtLst = parseStmtLst(tokens);
    checkClosedBrace(tokens);
    return make_unique<AST::Procedure>(move(procName), move(stmtLst));
}

unique_ptr<AST::Program> parseProgram(deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);  // consume "procedure"
    if (get<string>(currToken.value) != "procedure") {
        Logger(Level::ERROR) << "Procedure expected";
        throw invalid_argument("Procedure expected!");
    }
    auto resultProc = parseProcedure(tokens);
    currToken = getNextToken(tokens);  // consume eof
    return make_unique<AST::Program>(move(resultProc));
}

unique_ptr<AST::Program> Parser::parse(const string& source) {
    // we first tokenise the source code
    deque<Token> lexedTokens = Lexer(source).getTokens();
    try {
        return parseProgram(lexedTokens);
    }
    catch (invalid_argument ex) {
    }
}
