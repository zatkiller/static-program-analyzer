#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <stack>
#include <queue>
#include <vector>
#include <variant>
#include <utility>

#include "Lexer.h"
#include "Parser.h"
#include "logging.h"
#include "AST.h"


namespace SimpleParser {

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

void throwInvalidArgError(string msg) {
    Logger(Level::ERROR) << msg;
    throw invalid_argument(msg);
}

void throwUnexpectedToken(char c) {
    std::ostringstream oss;
    oss << "'" << c << "'" << " Expected";
    throwInvalidArgError(oss.str());
}

void throwUnexpectedToken(string s) {
    std::ostringstream oss;
    oss << "\"" << s << "\"" << " Expected";
    throwInvalidArgError(oss.str());
}

Token getNextToken(deque<Token>& tokens) {
    Token token = tokens.front();
    tokens.pop_front();
    return token;
}

void checkAndConsume(char c, deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);  // consume char c
    char* v = get_if<char>(&currToken.value);
    if (!v || *v != c) {
        throwUnexpectedToken(c);
    }
}

void checkAndConsume(string s, deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);  // consume string s
    string* v = get_if<string>(&currToken.value);
    if (!v || *v != s) {
        throwUnexpectedToken(s);
    }
}

/**
 * Parses constants
 */
unique_ptr<AST::Const> Parser::parseConst(deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);
    if (currToken.type != TokenType::number) {
        throwInvalidArgError("Number expected!");
    }
    return make_unique<AST::Const>(get<int>(currToken.value));
}

/**
 * Parses variable names
 */
unique_ptr<AST::Var> Parser::parseVariable(deque<Token>& tokens) {
    Token varToken = getNextToken(tokens);
    string* varName = get_if<string>(&varToken.value);
    if (!varName) {
        throwInvalidArgError("Name expected!");
    }
    return make_unique<AST::Var>(*varName); 
}

AST::RelOp Parser::parseRelOp(deque<Token>& tokens) {
    Token relOpToken = getNextToken(tokens);
    char* relOp = get_if<char>(&relOpToken.value);
    if (!relOp) {
        throwInvalidArgError("RelOp Expected!");
    }
    if (tokens.size() < 1) {
        throwInvalidArgError("Missing Tokens for RelOp parsing!");
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
            throwUnexpectedToken('=');
        }
        // we got "==", return
        getNextToken(tokens);  // consume '='
        return AST::RelOp::EQ;
    case '!':
        // can be "!="
        if (!nextOp || *nextOp != '=') {
            throwUnexpectedToken('=');
        }
        // we got "!=", return
        getNextToken(tokens);  // consume '='
        return AST::RelOp::NE;
    default:
        // special not recognised
        throwInvalidArgError("RelOp not recognised!");
    }
}

AST::CondOp Parser::parseCondOp(deque<Token>& tokens) {
    // either "&&" or "||"
    Token condOpToken = getNextToken(tokens);
    if (condOpToken.type != TokenType::special) {
        throwInvalidArgError("CondOp expected!");
    }
    char currOp = get<char>(condOpToken.value);

    if (tokens.size() < 1) {
        throwInvalidArgError("Missing Tokens for CondOp Parsing!");
    }
    condOpToken = tokens.front();
    char* nextOp = get_if<char>(&condOpToken.value);
    switch (currOp) {
    case '&':
        if (!nextOp || *nextOp != '&') {
            throwUnexpectedToken('&');
        }
        // we have "&&"
        getNextToken(tokens);  // consume the next '&'
        return AST::CondOp::AND;
    case '|':
        if (!nextOp || *nextOp != '|') {
            throwUnexpectedToken('|');
        }
        // we have "||"
        getNextToken(tokens);  // consume the next '|'
        return AST::CondOp::OR;
    default:
        throwInvalidArgError("CondOp not recognised!");
    }
}

AST::BinOp Parser::parseBinOp(deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);
    char* binOp = get_if<char>(&currToken.value);
    if (!binOp) {
        throwInvalidArgError("BinOp expected!");
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
        throwInvalidArgError("BinOp not recognised!");
    }
}

AST::BinOp charToBinOp(char c) {
    switch (c) {
    case '+': return AST::BinOp::PLUS;
    case '-': return AST::BinOp::MINUS;
    case '*': return AST::BinOp::MULT;
    case '/': return AST::BinOp::DIVIDE;
    case '%': return AST::BinOp::MOD;
    }
}

unique_ptr<AST::Expr> Parser::shuntingYardParser(deque<Token>& tokens) {
    // converting from infix to postfix expr
    std::map<char, string> binOpPrecedence;
    binOpPrecedence['+'] = 10;
    binOpPrecedence['-'] = 10;
    binOpPrecedence['*'] = 20;
    binOpPrecedence['/'] = 20;
    binOpPrecedence['%'] = 20;

    std::stack<std::unique_ptr<AST::Expr>> operands;
    deque<Token> queue;
    std::stack<char> operators;

    auto popAndPush = [&]() {
        // Pop 2 from operands, make BinExpr with symbol.
        char top = operators.top();
        auto expr1 = move(operands.top());
        operands.pop();
        auto expr2 = move(operands.top());
        operands.pop();
        operands.push(make_unique<AST::BinExpr>(charToBinOp(top), move(expr2), move(expr1)));
        operators.pop();
    };

    bool isEnd = false;
    do {
        Token currToken = tokens.front();
        switch (currToken.type) {
        case TokenType::eof:
            throwInvalidArgError("Unexpected Termination!");
            break;
        case TokenType::number:
            operands.push(parseConst(tokens));
            break;
        case TokenType::name:
            operands.push(parseVariable(tokens));
            break;
        case TokenType::special:
            char symbol = std::get<char>(currToken.value);
            // if the symbol is not in binOpPrecedence or brackets, terminate
            if (binOpPrecedence.find(symbol) == binOpPrecedence.end() && symbol != '(' && symbol != ')') {
                isEnd = true;
                break;
            }

            // If the symbol is (, we push it onto stack and continue
            if (symbol == '(') {
                operators.push(symbol);
                tokens.pop_front();
                // If it's operators, we resolve what we can
            } else if (symbol == '*' || symbol == '/' || symbol == '%') {
                while (!operators.empty()) {
                    char top = operators.top();
                    if (top == '(' || binOpPrecedence[top] < binOpPrecedence[symbol]) {
                        break;
                    }

                    // Pop 2 from operands, make BinExpr with symbol.
                    popAndPush();
                }
                operators.push(symbol);
                tokens.pop_front();
            } else if (symbol == '+' || symbol == '-') {
                while (!operators.empty()) {
                    char top = operators.top();
                    if (top == '(' || binOpPrecedence[top] < binOpPrecedence[symbol]) {
                        break;
                    }

                    // Pop 2 from operands, make BinExpr with symbol.
                    popAndPush();
                }
                operators.push(symbol);
                tokens.pop_front();
            } else if (symbol == ')') {
                // assume this is the closing bracket unless there is matching open bracket in the stack.
                isEnd = true;
                while (!operators.empty()) {
                    char top = operators.top();
                    if (top == '(') {
                        operators.pop();
                        tokens.pop_front();
                        isEnd = false;
                        break;
                    }
                    // Pop 2 from operands, make BinExpr with symbol.
                    popAndPush();
                }
            } else {
                // If it's not a symbol we are expecting, exit.
                isEnd = true;
                break;
            }
            break;
        }
    } while (!isEnd && tokens.front().type != TokenType::eof);

    while (!operators.empty()) {
        popAndPush();
    }

    if (operands.size() != 1) {
        Logger(Level::ERROR) << "Some parsing error";
        throw invalid_argument("Unexpected Termination!");
    }

    return move(operands.top());
}

unique_ptr<AST::Expr> Parser::parseExpr(deque<Token>& tokens) {
    // can be a name | const | binExpr | '(' expr ')'
    return shuntingYardParser(tokens);
}

unique_ptr<AST::CondExpr> Parser::parseRelExpr(deque<Token>& tokens) {
    auto lhsExpr = parseExpr(tokens);
    // the symbols '>', '>=', '<', '<=', '==', '!='
    auto relOp = parseRelOp(tokens);
    auto rhsExpr = parseExpr(tokens);
    return make_unique<AST::RelExpr>(relOp, move(lhsExpr), move(rhsExpr));
}

/**
 * To identify if bracketted expression is part of a condExpr or Expr.
 * E.g. while( (x+1) > 1) vs while ((x > 1) && (y < x))
 */
bool isCondExpr(deque<Token>& tokens) {
    int openBrCount = 0;
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
                    return true;
                }
                return false;
            } else if (openBrCount < 0) {
                return true;
            }
        } else if (v.type == TokenType::eof) {
            if (openBrCount <= 0) {
                return true;
            }
        }
    }
    return false;
}

unique_ptr<AST::CondExpr> Parser::parseCondExpr(deque<Token>& tokens) {
    // Token currToken = getNextToken(tokens);
    Token currToken = tokens.front();
    unique_ptr<AST::CondExpr> condExprResult;
    if (currToken.type == TokenType::special) {
        // we have to "unwrap" the "(...)" or "!(...)"
        auto tokenVal = get<char>(currToken.value);
        if (tokenVal == '!') {
            // This means: "!(condExpr)"
            tokens.pop_front();  // consume the '!'
            checkAndConsume('(', tokens);
            condExprResult = make_unique<AST::NotCondExpr>(move(parseCondExpr(tokens)));
            checkAndConsume(')', tokens);
        } else if (tokenVal == '(') {
            if (isCondExpr(tokens)) {
                checkAndConsume('(', tokens);  // consume the '('
                condExprResult = move(parseCondExpr(tokens));  // consume condExpr
                checkAndConsume(')', tokens);
            } else {
                return move(parseRelExpr(tokens));
            }

        } else {
            throwInvalidArgError("Symbol not recognised!");
        }
    } else {
        // here, we parse rel_expr
        return move(parseRelExpr(tokens));
    }

    // Peek and check if we have another cond_expr,
    // i.e. '(' cond_expr ')' '&&' '(' cond_expr ')' or '(' cond_expr ')' '||' '(' cond_expr ')'
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

AST::StmtLst Parser::parseStmtLst(deque<Token>& tokens) {
    vector<unique_ptr<AST::Statement>> list;
    do {
        Token currToken = tokens.front();
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
            list.push_back(move(parseAssignStmt(tokens)));
        }
    } while (
        (tokens.front().type != TokenType::special ||
            get<char>(tokens.front().value) != '}') &&
        tokens.front().type != TokenType::eof);
    return AST::StmtLst(list);
}

unique_ptr<AST::Statement> Parser::parseReadStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    checkAndConsume("read", tokens);
    auto var = parseVariable(tokens);
    checkAndConsume(';', tokens);
    return make_unique<AST::Read>(lineNo, move(var));
}

unique_ptr<AST::Statement> Parser::parsePrintStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    checkAndConsume("print", tokens);
    auto var = parseVariable(tokens);
    checkAndConsume(';', tokens);
    return make_unique<AST::Print>(lineNo, move(var));
}

unique_ptr<AST::Statement> Parser::parseAssignStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    // get the var being assigned
    auto var = parseVariable(tokens);
    checkAndConsume('=', tokens);
    // parse the expr on the right hand side
    auto rhsExpr = parseExpr(tokens);
    checkAndConsume(';', tokens);

    return make_unique<AST::Assign>(lineNo, move(var), move(rhsExpr));
}

unique_ptr<AST::Statement> Parser::parseWhileStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    // check for "while"
    checkAndConsume("while", tokens);
    checkAndConsume('(', tokens);
    auto condExprResult = parseCondExpr(tokens);
    checkAndConsume(')', tokens);
    checkAndConsume('{', tokens);
    auto stmtLstResult = parseStmtLst(tokens);
    checkAndConsume('}', tokens);

    return make_unique<AST::While>(
        lineNo, move(condExprResult), move(stmtLstResult)
    );
}

unique_ptr<AST::Statement> Parser::parseIfStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    
    // check for "if"
    checkAndConsume("if", tokens);
    checkAndConsume('(', tokens);
    auto condExprResult = parseCondExpr(tokens);
    checkAndConsume(')', tokens);
    
    // check for "then"
    checkAndConsume("then", tokens);
    checkAndConsume('{', tokens);
    auto thenStmtLst = parseStmtLst(tokens);
    checkAndConsume('}', tokens);
    
    // check for 'else'
    checkAndConsume("else", tokens);
    checkAndConsume('{', tokens);
    auto elseStmtLst = parseStmtLst(tokens);
    checkAndConsume('}', tokens);

    return make_unique<AST::If>(
        lineNo,
        move(condExprResult),
        move(thenStmtLst),
        move(elseStmtLst)
    );
}

unique_ptr<AST::Procedure> Parser::parseProcedure(deque<Token>& tokens) {
    // consume "procedure"
    checkAndConsume("procedure", tokens);
    // get procName
    auto currToken = getNextToken(tokens);
    if (currToken.type != TokenType::name) {
        throwInvalidArgError("procName expected!");
    }
    string procName = get<string>(currToken.value);
    checkAndConsume('{', tokens);
    auto stmtLst = parseStmtLst(tokens);
    checkAndConsume('}', tokens);
    return make_unique<AST::Procedure>(move(procName), move(stmtLst));
}

unique_ptr<AST::Program> Parser::parseProgram(deque<Token>& tokens) {
    Token currToken = tokens.front();
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
        Logger(Level::ERROR) << "exception caught: " << ex.what();
        return unique_ptr<AST::Program>();
    }
}

}  // namespace SimpleParser
