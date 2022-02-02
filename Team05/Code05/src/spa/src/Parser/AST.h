#pragma once

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <variant>

namespace AST {

struct ASTNodeVisitor;

class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual void accept(ASTNodeVisitor& visitor) const = 0;
};

// Abstract classes
class ASTNode;
class Expr;
class Statement;
class IO;
class CondExpr;

// Concrete Classes
class Program;
class Procedure;

// statements
class StmtLst;
class If;
class While;
class Read;
class Print;
class Assign;

// expressions
class Var;
class Const;
class BinExpr;

// conditional expressions
class RelExpr;
class CondBinExpr;
class NotCondExpr;

struct ASTNodeVisitor {
    virtual void visit(const Program& node) = 0;
    virtual void visit(const Procedure& node) = 0;
    virtual void visit(const StmtLst& node) = 0;
    virtual void visit(const If& node) = 0;
    virtual void visit(const While& node) = 0;
    virtual void visit(const Read& node) = 0;
    virtual void visit(const Print& node) = 0;
    virtual void visit(const Assign& node) = 0;
    virtual void visit(const Var& node) = 0;
    virtual void visit(const Const& node) = 0;
    virtual void visit(const BinExpr& node) = 0;
    virtual void visit(const RelExpr& node) = 0;
    virtual void visit(const CondBinExpr& node) = 0;
    virtual void visit(const NotCondExpr& node) = 0;
    virtual void enterContainer(std::variant<int, std::string> number) = 0;
    virtual void exitContainer() = 0;
};



/**
    * Base class for Var/Const/BinExpr in the AST.
    * Identified by:
    *  expr | term | factor
    * where:
    *  expr: expr ‘+’ term | expr ‘-’ term | term
    *  term: term ‘*’ factor | term ‘/’ factor | term ‘%’ factor | factor
    *  factor: var_name | const_value | ‘(’ expr ‘)’
    */
class Expr : public ASTNode {
public:
    virtual ~Expr() {}
};

/**
* Represents a variable in the AST.
* Identified by:
*  var_name
*/
class Var : public Expr {
private:
    std::string varName;
public:
    explicit Var(const std::string& varName) : varName(varName) {}
    const std::string& getVarName() const { return varName; }
    void setVarName(std::string& name) { this->varName = name; }
    void accept(ASTNodeVisitor& visitor) const;
};


/**
    * Represents a statement in the AST.
    * Base class for: 'read', 'print', 'while', 'if' and 'assign' statements.
    * Identified by:
    *  read | print | while | if | assign
    */
class Statement : public ASTNode {
private:
    int stmtNo;
public:
    ~Statement() {}
    explicit Statement(int stmtNo) : stmtNo(stmtNo) {}

    const int getStmtNo() const {
        return stmtNo;
    }
};

/**
    * Represents a conditional expression in the AST
    * Identified by:
    *  cond_expr | rel_expr | rel_factor
    * where:
    *  cond_expr: rel_expr | '!' '(' cond_expr ')' |
    *    '(' cond_expr ')' '&&' '(' cond_expr ')' |
    *    '(' cond_expr ')' '||' '(' cond_expr ')'
    *  rel_expr: rel_factor '>' rel_factor | rel_factor '>=' rel_factor |
    *    rel_factor '<' rel_factor | rel_factor '<' rel_factor |
    *    rel_factor '==' rel_factor | rel_factor '!=' rel_factor
    *  rel_factor: var_name | const_value | expr
    */
class CondExpr : public ASTNode {
public:
    virtual ~CondExpr() {}
};

class StmtLst : public ASTNode {
private:
    std::vector<std::unique_ptr<Statement>> list;
public:
    explicit StmtLst(std::vector<std::unique_ptr<Statement>>& list)
    : list(std::move(list)) {}

    void accept(ASTNodeVisitor& visitor) const;
};

/**
    * Represents a procedure in the AST.
    * Identified by:
    *  'procedure' procName stmtlst
    */
class Procedure : public ASTNode {
private:
    std::string procName;
    StmtLst stmtLst;
public:
    Procedure(const std::string& procName,
        StmtLst stmtLst) :
        procName(procName), stmtLst(std::move(stmtLst)) {}

    void accept(ASTNodeVisitor& visitor) const;
};

/**
    * Represents the Program in the AST.
    * Program contains a single Procedure.
    * Identified by:
    *  procedure
    */
class Program : public ASTNode {
private:
    std::unique_ptr<Procedure> procedure;
public:
    explicit Program(std::unique_ptr<Procedure> procedure) :
        procedure(std::move(procedure)) {}

    void accept(ASTNodeVisitor& visitor) const;
};

/**
    * Represents an 'if' statement in the AST.
    * Identified by:
    *  'if' '(' cond_expr ')' 'then' '{' stmtLst '}' 'else' '{' stmtLst '}'
    */
class If : public Statement {
private:
    std::unique_ptr<CondExpr> condExpr;
    StmtLst thenBlk;
    StmtLst elseBlk;
public:
    If(
        int stmtNo,
        std::unique_ptr<CondExpr> condExpr,
        StmtLst thenBlk,
        StmtLst elseBlk
    ) :
        Statement(stmtNo),
        condExpr(std::move(condExpr)),
        thenBlk(std::move(thenBlk)),
        elseBlk(std::move(elseBlk)) {}

    void accept(ASTNodeVisitor& visitor) const;
};

/**
    * Represents an 'while' statement in the AST.
    * Identified by:
    *  'while' '(' cond_expr ')' '{' stmtLst '}'
    */
class While : public Statement {
private:
    std::unique_ptr<CondExpr> condExpr;
    StmtLst stmtLst;
public:
    While(
        int stmtNo,
        std::unique_ptr<CondExpr> condExpr,
        StmtLst stmtLst
    ) :
        Statement(stmtNo),
        condExpr(std::move(condExpr)),
        stmtLst(std::move(stmtLst)) {}

    void accept(ASTNodeVisitor& visitor) const;
};

/**
    * Represents an 'assign' statement in the AST.
    * Identified by:
    *  var_name '=' expr ';'
    */
class Assign : public Statement {
private:
    std::unique_ptr<Var> var;
    std::unique_ptr<Expr> expr;
public:
    Assign(
        int stmtNo,
        std::unique_ptr<Var> var, 
        std::unique_ptr<Expr> expr
    ) :
        Statement(stmtNo),
        var(std::move(var)), 
        expr(std::move(expr)) {}

    void accept(ASTNodeVisitor& visitor) const;

    const Var getLHS() const {
        return *var;
    }
};


/**
    * Represents an input/output statement in the AST.
    * Base class for 'Read and 'Print' statements.
    */
class IO : public Statement {
private:
    std::unique_ptr<Var> var;
public:
    IO(
        int stmtNo,
        std::unique_ptr<Var> var
    ) :
        Statement(stmtNo),
        var(std::move(var)) {}

    const Var getVar() const { return *var; }
};

/**
    * Represents a Read statement in the AST.
    * Identified by:
    *  'read' var_name;
    */
class Read : public IO {
public:
    using IO::IO;
    void accept(ASTNodeVisitor& visitor) const;

};

/**
    * Represents a Print statement in the AST.
    * Identified by:
    *  'print' var_name;
    */
class Print : public IO {
public:
    using IO::IO;
    void accept(ASTNodeVisitor& visitor) const;
};

/**
    * Represents a const in the AST.
    * Identified by:
    *  const_value
    */
class Const : public Expr {
private:
    int constValue;
public:
    Const(int constValue) : constValue(constValue) {}
    void accept(ASTNodeVisitor& visitor) const;
};

/**
    * Represents possible operators in BinExpr.
    */
enum class BinOp {
    PLUS   = '+',
    MINUS  = '-',
    DIVIDE = '/',
    MULT   = '*',
    MOD    = '%'
};

/**
    * Represents an expression with 2 Expr and an operator in the AST.
    * Identified by:
    *  from expr: expr '+' term | expr '-' term |
    *  from term: term '*' factor | term '/' factor | term '%' factor
    */
class BinExpr: public Expr {
private:
    BinOp Op;
    std::unique_ptr<Expr> LHS, RHS;
public:
    BinExpr(
        BinOp Op, 
        std::unique_ptr<Expr> LHS, 
        std::unique_ptr<Expr> RHS
    ) : 
        Op(Op), 
        LHS(std::move(LHS)), 
        RHS(std::move(RHS)) {}

    void accept(ASTNodeVisitor& visitor) const;
};

/**
    * Represents the possible operators in RelExpr
    */
enum class RelOp {
    LTE,  // <=
    GTE,  // >=
    LT,   // <
    GT,   // >
    EQ,   // ==
    NE    // !=
};

/**
    * Represents a RelExpr in the AST.
    * Identified by: 
    *  rel_expr: rel_factor '>' rel_factor | rel_factor '>=' rel_factor | 
    *    rel_factor '<' rel_factor | rel_factor '<' rel_factor | 
    *    rel_factor '==' rel_factor | rel_factor '!=' rel_factor
    */
class RelExpr : public CondExpr {
private:
    RelOp Op;
    std::unique_ptr<Expr> LHS, RHS;
public:
    RelExpr(
        RelOp Op, 
        std::unique_ptr<Expr> LHS, 
        std::unique_ptr<Expr> RHS
    ) : 
        Op(Op), 
        LHS(std::move(LHS)), 
        RHS(std::move(RHS)) {}

    void accept(ASTNodeVisitor& visitor) const;
};

/**
    * Represents the possible operators in CondBinExpr and NotCondExpr
    */
enum class CondOp {
    NOT,  // !
    AND,  // &&
    OR    // ||
};
/**
    * Represents CondBinExpr in the AST.
    * Identified by:
    *  from cont_expr: ‘(’ cond_expr ‘)’ ‘&&’ ‘(’ cond_expr ‘)’ |
    *    ‘(’ cond_expr ‘)’ ‘||’ ‘(’ cond_expr ‘)’
    */
class CondBinExpr : public CondExpr {
private:
    CondOp Op;
    std::unique_ptr<CondExpr> LHS, RHS;
public:
    CondBinExpr(
        CondOp Op, 
        std::unique_ptr<CondExpr> LHS, 
        std::unique_ptr<CondExpr> RHS
    ) : 
        Op(Op), 
        LHS(std::move(LHS)), 
        RHS(std::move(RHS)) {}

    void accept(ASTNodeVisitor& visitor) const;
};

/**
    * Represents the condition expression with '!' in the AST.
    * Identified by:
    *   from cont_expr: ‘!’ ‘(’ cond_expr ‘)’
    */
class NotCondExpr : public CondExpr {
private:
    CondOp Op = CondOp::NOT;
    std::unique_ptr<CondExpr> condExpr;
public:
    NotCondExpr(
        std::unique_ptr<CondExpr> condExpr
    ) : 
        condExpr(std::move(condExpr)) {}

    void accept(ASTNodeVisitor& visitor) const;
};

}  // namespace AST
