#include <stdio.h>
#include <iostream>
#include <string>
#include <map>
#include <stack>
#include <queue>
#include <vector>
#include <variant>
#include <utility>
#include <unordered_map>
#include <set>

#include "Lexer.h"
#include "Parser.h"
#include "logging.h"
#include "AST.h"

namespace sp {
namespace parser {

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

int lineCount = 1;
std::unordered_map<int, std::string> callStmts;
std::set<std::string> procedures;
/** =============================== HELPER METHODS ================================ */

void throwInvalidArgError(string msg) {
    Logger(Level::ERROR) << msg;
    throw invalid_argument(msg);
}

void throwUnexpectedToken(char c) {
    std::ostringstream oss;
    oss << "'" << c << "'" << " expected!";
    throwInvalidArgError(oss.str());
}

void throwUnexpectedToken(string s) {
    std::ostringstream oss;
    oss << "\"" << s << "\"" << " expected!";
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

/** ================================ ATOMIC PARSER ================================ */
namespace AtomicParser {
/**
 * Parses constants
 */
unique_ptr<ast::Const> parseConst(deque<Token>& tokens) {
    Token currToken = getNextToken(tokens);
    if (currToken.type != TokenType::number) {
        throwInvalidArgError("Number expected!");
    }
    return make_unique<ast::Const>(get<int>(currToken.value));
}

/**
 * Parses variable names
 */
unique_ptr<ast::Var> parseVariable(deque<Token>& tokens) {
    Token varToken = getNextToken(tokens);
    string* varName = get_if<string>(&varToken.value);
    if (!varName) {
        throwInvalidArgError("Name expected!");
    }
    return make_unique<ast::Var>(*varName); 
}

ast::RelOp parseRelOp(deque<Token>& tokens) {
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
            return ast::RelOp::GTE;
        }
        // we got '>', return
        return ast::RelOp::GT;
    case '<':
        // can be '<' or "<="
        if (nextOp && *nextOp == '=') {
            // we got "<=", return
            getNextToken(tokens);  // consume '='
            return ast::RelOp::LTE;
        }
        // we got '>', return
        return ast::RelOp::LT;
    case '=':
        // can be "=="
        if (!nextOp || *nextOp != '=') {
            throwUnexpectedToken('=');
        }
        // we got "==", return
        getNextToken(tokens);  // consume '='
        return ast::RelOp::EQ;
    case '!':
        // can be "!="
        if (!nextOp || *nextOp != '=') {
            throwUnexpectedToken('=');
        }
        // we got "!=", return
        getNextToken(tokens);  // consume '='
        return ast::RelOp::NE;
    default:
        // special not recognised
        throwInvalidArgError("RelOp not recognised!");
    }
}

ast::CondOp parseCondOp(deque<Token>& tokens) {
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
        return ast::CondOp::AND;
    case '|':
        if (!nextOp || *nextOp != '|') {
            throwUnexpectedToken('|');
        }
        // we have "||"
        getNextToken(tokens);  // consume the next '|'
        return ast::CondOp::OR;
    default:
        throwInvalidArgError("CondOp not recognised!");
    }
}

/**
 * @brief a map that maps each char symbol to the equivalent enum ::BinOp
 */
std::unordered_map<char, ast::BinOp> binOpMap = {
    {'+', ast::BinOp::PLUS},
    {'-', ast::BinOp::MINUS},
    {'*', ast::BinOp::MULT},
    {'/', ast::BinOp::DIVIDE},
    {'%', ast::BinOp::MOD}
};

}  // namespace AtomicParser

/** ================================= EXPR PARSER ================================= */
namespace expr_parser {
/**
 * @brief a map that maps each char operator to the equivalent precedence for Shunting-Yard.
 */
std::unordered_map<char, int> binOpPrecedence = {
    {'+', 10},
    {'-', 10},
    {'*', 20},
    {'/', 20},
    {'%', 20}
};

/**
 * @brief Helper function for Shunting-Yard that pop 2 operands and make BinExpr with an operator.
 * 
 * @param operands  a reference to a stack of operands
 * @param operators a reference to a stack of operators
 */
void popAndPush(std::stack<std::unique_ptr<ast::Expr>>& operands, std::stack<char>& operators) {
    char top = operators.top();
    auto expr1 = move(operands.top());
    operands.pop();
    auto expr2 = move(operands.top());
    operands.pop();
    operands.push(make_unique<ast::BinExpr>(AtomicParser::binOpMap[top], move(expr2), move(expr1)));
    operators.pop();
}

/**
 * @brief Helper function for Shunting-Yard that handles a valid ::BinOp operand
 * 
 * @param operands  a reference to a stack of operands
 * @param operators a reference to a stack of operators
 * @param operand    an operand that is being parsed
 */
void handleOperand(
    std::stack<std::unique_ptr<ast::Expr>>& operands, 
    std::stack<char>& operators, 
    char operand) {
    while (!operators.empty()) {
        char top = operators.top();
        if (top == '(' || binOpPrecedence[top] < binOpPrecedence[operand]) {
            break;
        }
        popAndPush(operands, operators);   // make BinExpr with two operands and one operator
    }
    operators.push(operand);
}

/**
 * @brief Helper function for Shunting-Yard that handles the closing parenthesis ')'
 * @details Helper function will keep popping and forming expressions until a matching '(' is found.
 * If it is found, the expression can continue to be parsed.
 * Otherwise, it is the end of the expression. 
 * 
 * @param operands  a reference to a stack of operands
 * @param operators a reference to a stack of operators
 * @return a bool indicating whether it is the end of an expression or not  
 */
bool handleClosingParen(std::stack<std::unique_ptr<ast::Expr>>& operands, std::stack<char>& operators) {
    while (!operators.empty()) {
        char top = operators.top();
        if (top == '(') {
            return false;  // not the end of expr
        }
        popAndPush(operands, operators);  // make BinExpr with two operands and one operator
    }
    return true;
}

unique_ptr<ast::Expr> shuntingYardParser(deque<Token>& tokens) {
    // converting from infix to postfix expr
    std::stack<std::unique_ptr<ast::Expr>> operands;
    deque<Token> queue;
    std::stack<char> operators;

    bool isEnd = false;
    do {
        Token currToken = tokens.front();
        switch (currToken.type) {
        case TokenType::eof:
            throwInvalidArgError("Unexpected Termination!");
            break;
        case TokenType::number:
            operands.push(AtomicParser::parseConst(tokens));
            break;
        case TokenType::name:
            operands.push(AtomicParser::parseVariable(tokens));
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
            } else if (symbol == '*' || symbol == '/' || symbol == '%' || symbol == '+' || symbol == '-') {
                handleOperand(operands, operators, symbol);
                tokens.pop_front();
            } else if (symbol == ')') {
                isEnd = handleClosingParen(operands, operators);
                if (!isEnd) {
                    // if it is not the end of parsing the expr, we found the matching parenthesis
                    operators.pop();  // consume '('
                    tokens.pop_front();  // consume ')'
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
        popAndPush(operands, operators);
    }

    if (operands.size() != 1) {
        throwInvalidArgError("Expr Parsing Error!");
    }

    return move(operands.top());
}

unique_ptr<ast::Expr> parse(deque<Token>& tokens) {
    // can be a name | const | binExpr | '(' expr ')'
    return shuntingYardParser(tokens);
}

}  // namespace expr_parser

/** =============================== CONDEXPR PARSER =============================== */
namespace cond_expr_parser {

unique_ptr<ast::CondExpr> parseRelExpr(deque<Token>& tokens) {
    auto lhsExpr = expr_parser::parse(tokens);
    // the symbols '>', '>=', '<', '<=', '==', '!='
    auto relOp = AtomicParser::parseRelOp(tokens);
    auto rhsExpr = expr_parser::parse(tokens);
    return make_unique<ast::RelExpr>(relOp, move(lhsExpr), move(rhsExpr));
}

/**
 * To identify if bracketted expression is part of a condExpr or Expr.
 * E.g. "while ((x + 1) > 1)" vs "while ((x > 1) && (y < x))"
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
                return sym == '&' || sym == '|';
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

unique_ptr<ast::CondExpr> parse(deque<Token>& tokens) {
    Token currToken = tokens.front();
    unique_ptr<ast::CondExpr> condExprResult;
    if (currToken.type == TokenType::special) {
        // we have to "unwrap" the "(...)" or "!(...)"
        auto tokenVal = get<char>(currToken.value);
        if (tokenVal == '!') {
            // This means: "!(condExpr)"
            tokens.pop_front();  // consume the '!'
            checkAndConsume('(', tokens);
            condExprResult = make_unique<ast::NotCondExpr>(move(parse(tokens)));
            checkAndConsume(')', tokens);
        } else if (tokenVal == '(') {
            if (isCondExpr(tokens)) {
                checkAndConsume('(', tokens);  // consume the '('
                condExprResult = move(parse(tokens));  // consume condExpr
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
    auto condOp = move(AtomicParser::parseCondOp(tokens));
    auto rhsCondExprResult = move(parse(tokens));
    return make_unique<ast::CondBinExpr>(condOp, move(condExprResult), move(rhsCondExprResult));
}
}  // namespace cond_expr_parser

/** ================================= STMT PARSER ================================= */
namespace statement_list_parser {

unique_ptr<ast::Statement> parseReadStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    checkAndConsume("read", tokens);
    auto var = AtomicParser::parseVariable(tokens);
    checkAndConsume(';', tokens);
    return make_unique<ast::Read>(lineNo, move(var));
}

unique_ptr<ast::Statement> parsePrintStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    checkAndConsume("print", tokens);
    auto var = AtomicParser::parseVariable(tokens);
    checkAndConsume(';', tokens);
    return make_unique<ast::Print>(lineNo, move(var));
}

unique_ptr<ast::Statement> parseAssignStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    // get the var being assigned
    auto var = AtomicParser::parseVariable(tokens);
    checkAndConsume('=', tokens);
    // parse the expr on the right hand side
    auto rhsExpr = expr_parser::parse(tokens);
    checkAndConsume(';', tokens);
    return make_unique<ast::Assign>(lineNo, move(var), move(rhsExpr));
}

unique_ptr<ast::Statement> parseWhileStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    // check for "while"
    checkAndConsume("while", tokens);
    checkAndConsume('(', tokens);
    auto condExprResult = cond_expr_parser::parse(tokens);
    checkAndConsume(')', tokens);
    checkAndConsume('{', tokens);
    auto stmtLstResult = parse(tokens);
    checkAndConsume('}', tokens);

    return make_unique<ast::While>(
        lineNo, move(condExprResult), move(stmtLstResult)
    );
}

unique_ptr<ast::Statement> parseIfStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    
    // check for "if"
    checkAndConsume("if", tokens);
    checkAndConsume('(', tokens);
    auto condExprResult = cond_expr_parser::parse(tokens);
    checkAndConsume(')', tokens);
    
    // check for "then"
    checkAndConsume("then", tokens);
    checkAndConsume('{', tokens);
    auto thenStmtLst = parse(tokens);
    checkAndConsume('}', tokens);
    
    // check for 'else'
    checkAndConsume("else", tokens);
    checkAndConsume('{', tokens);
    auto elseStmtLst = parse(tokens);
    checkAndConsume('}', tokens);

    return make_unique<ast::If>(
        lineNo,
        move(condExprResult),
        move(thenStmtLst),
        move(elseStmtLst)
    );
}

unique_ptr<ast::Statement> parseCallStmt(deque<Token>& tokens) {
    int lineNo = lineCount++;
    checkAndConsume("call", tokens);
    // get procName
    auto currToken = getNextToken(tokens);
    if (currToken.type != TokenType::name) {
        throwUnexpectedToken("procName");
    }
    string procName = get<string>(currToken.value);
    checkAndConsume(';', tokens);
    // insert into callStmts tracker
    callStmts[lineNo] = procName;
    return make_unique<ast::Call>(
        lineNo,
        procName
    );
}

ast::StmtLst parse(deque<Token>& tokens) {
    vector<unique_ptr<ast::Statement>> list;
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
        } else if (*keyword == "call") {
            list.push_back(move(parseCallStmt(tokens)));
        } else {
            list.push_back(move(parseAssignStmt(tokens)));
        }
    } while (
        (tokens.front().type != TokenType::special ||
            get<char>(tokens.front().value) != '}') &&
        tokens.front().type != TokenType::eof);
    return ast::StmtLst(list);
}    

}  // namespace statement_list_parser

/** ============================== HIGH-LEVEL PARSERS =============================== */

unique_ptr<ast::Procedure> parseProcedure(deque<Token>& tokens) {
    // consume "procedure"
    checkAndConsume("procedure", tokens);
    // get procName
    auto currToken = getNextToken(tokens);
    if (currToken.type != TokenType::name) {
        throwUnexpectedToken("procName");
    }
    string procName = get<string>(currToken.value);
    // parse stmtLst in the container
    checkAndConsume('{', tokens);
    auto stmtLst = statement_list_parser::parse(tokens);
    checkAndConsume('}', tokens);
    if (procedures.count(procName)) {
        std::ostringstream os;
        os << "Repeated procedure name: " << procName << "!";
        throwInvalidArgError(os.str());
    }
    procedures.insert(procName);
    return make_unique<ast::Procedure>(move(procName), move(stmtLst));
}

unique_ptr<ast::Program> parseProgram(deque<Token>& tokens) {
    if (tokens.empty() || tokens.front().type == TokenType::eof) {
        throwInvalidArgError("Empty program received!");
    }
    std::vector<unique_ptr<ast::Procedure>> res;
    while (!tokens.empty() && tokens.front().type != TokenType::eof) {
        if (tokens.front().type != TokenType::name || get<string>(tokens.front().value) != "procedure") {
            throwUnexpectedToken("procedure");
        }
        res.push_back(parseProcedure(tokens));
    }

    Token currToken = getNextToken(tokens);  // consume eof
    // check if the call statements are calling existent procedures
    for (auto callStmt : callStmts) {
        if (!procedures.count(callStmt.second)) {
            std::ostringstream os;
            os << "Calling a non-existent procedure: " 
            << callStmt.second << " at " << callStmt.first << "!";
            throwInvalidArgError(os.str());
        }
    }
    return make_unique<ast::Program>(move(res));
}

unique_ptr<ast::Program> parse(const string& source) {
    // new program, reset state.
    lineCount = 1;
    callStmts.clear();
    procedures.clear();
    // we first tokenise the source code
    deque<Token> lexedTokens = Lexer(source).getTokens();
    try {
        return parseProgram(lexedTokens);        
    }
    catch (invalid_argument ex) {
        Logger(Level::ERROR) << "exception caught: " << ex.what();
        return unique_ptr<ast::Program>();
    }
}

}  // namespace parser
}  // namespace sp
