#pragma once

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <variant>

/**
 * @namespace AST
 * @brief Namespace to encapsulate the AST.
 */
namespace AST {

struct ASTNodeVisitor;

class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual void accept(std::shared_ptr<ASTNodeVisitor> visitor) const = 0;
    virtual bool operator==(ASTNode const& o) const = 0;
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
 * @class Expr
 * @brief Class that represents an expression in the AST
 * and is the base class for Var, Const and BinExpr. 
 * @details Expr is identified by: expr | term | factor \n 
 * where:
 * - expr: expr '+' term | expr '-' term | term
 * - term: term '*' factor | term '/' factor | term '%' factor | factor
 * - factor: var_name | const_value | '(' expr ')'
 * @see Var, Const, BinExpr
 */
class Expr : public ASTNode {
public:
    virtual ~Expr() {}
};

/**
 * @class Var
 * @brief Class that represents a variable in the AST. 
 * @details Var is identified by: var_name \n 
 * where:
 * - var_name: NAME
 *     - NAME: LETTER (LETTER | DIGIT)*
 */
class Var : public Expr {
private:
    std::string varName;
public:
    explicit Var(const std::string& varName) : varName(varName) {}
    const std::string& getVarName() const { return varName; }
    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;
    
    virtual bool operator==(ASTNode const& o) const;
};


/**
 * @class Statement
 * @brief Class that represents a statement in the AST
 * and is the base class for Read, Print, While, If and Assign statements. 
 * @details Statement is identified by: read | print | while | if | assign \n 
 * where:
 * - read: 'read' var_name';'
 * - print: 'print' var_name';'
 * - while: 'while' '(' cond_expr ')' '{' stmtLst '}'
 * - if: 'if' '(' cond_expr ')' 'then' '{' stmtLst '}' 'else' '{' stmtLst '}'
 * - assign: var_name '=' expr ';'
 * @see Read, Print, While, If, Assign
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
 * @class CondExpr
 * @brief Class that represents a conditional expression in the AST
 * and is the base class for CondBinExpr, NotCondExpr and RelExpr. 
 * @details CondExpr is identified by: cond_expr | rel_expr | rel_factor \n 
 * where: 
 * - cond_expr: rel_expr | '!' '(' cond_expr ')' | 
 * '(' cond_expr ')' '&&' '(' cond_expr ')' |
 * '(' cond_expr ')' '||' '(' cond_expr ')'
 * -  rel_expr: rel_factor '>' rel_factor | rel_factor '>=' rel_factor |
 * rel_factor '<' rel_factor | rel_factor '<' rel_factor |
 * rel_factor '==' rel_factor | rel_factor '!=' rel_factor
 * - rel_factor: var_name | const_value | expr
 * @see Var, Const, Expr
 */
class CondExpr : public ASTNode {
public:
    virtual ~CondExpr() {}
};

/**
 * @class StmtLst
 * @brief Class that represents 0..* Statement in the AST.
 * @details StmtLst is identified by: stmt+
 * where: 
 * - stmt: read | print | while | if | assign
 * @see Read, Print, While, If, Assign
 */
class StmtLst : public ASTNode {
private:
    std::vector<std::unique_ptr<Statement>> list;
public:
    explicit StmtLst(std::vector<std::unique_ptr<Statement>>& list)
    : list(std::move(list)) {}

    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;

    virtual bool operator==(ASTNode const& o) const;
};

/**
 * @class Procedure
 * @brief Class that represents a Procedure in the AST.
 * @details Procedure is identified by: 'procedure' proc_name '{' stmtLst '}'
 * where: 
 * - stmtLst: stmt+
 * - proc_name: NAME
 *     - NAME: LETTER (LETTER | DIGIT)*
 * @see Statement
 */
class Procedure : public ASTNode {
private:
    std::string procName;
    StmtLst stmtLst;
public:
    Procedure(const std::string& procName,
        StmtLst stmtLst) :
        procName(procName), stmtLst(std::move(stmtLst)) {}

    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;

    virtual bool operator==(ASTNode const& o) const;
};

/**
 * @class Program
 * @brief Class that represents a Program in the AST.
 * @details Currently a Program contains a single Procedure. \n
 * It is therefore identified by: procedure \n
 * where:
 * - procedure: 'procedure' proc_name '{' stmtLst '}'
 * @see Procedure
 */
class Program : public ASTNode {
private:
    std::unique_ptr<Procedure> procedure;
public:
    explicit Program(std::unique_ptr<Procedure> procedure) :
        procedure(std::move(procedure)) {}
    
    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;

    virtual bool operator==(ASTNode const& o) const;
};

/**
 * @class If
 * @brief Class that Represents an If statement in the AST.
 * @details If is identified by: 'if' '(' cond_expr ')' 
 * 'then' '{' stmtLst '}' 'else' '{' stmtLst '}' \n
 * where:
 * - cond_expr: rel_expr | '!' '(' cond_expr ')' |
 * '(' cond_expr ')' '&&' '(' cond_expr ')' |
 * '(' cond_expr ')' '||' '(' cond_expr ')'
 * - stmtLst: stmt+
 * @see Statement, StmtLst, CondExpr, RelExpr
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

    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;
    CondExpr* getCondExpr() const {
        return condExpr.get();
    }

    virtual bool operator==(ASTNode const& o) const;
};

/**
 * @class While
 * @brief Class that Represents a While statement in the AST.
 * @details While is identified by: 'while' '(' cond_expr ')' '{' stmtLst '}' \n 
 * where:
 * - cond_expr: rel_expr | '!' '(' cond_expr ')' |
 * '(' cond_expr ')' '&&' '(' cond_expr ')' |
 * '(' cond_expr ')' '||' '(' cond_expr ')'
 * - stmtLst: stmt+
 * @see Statement, StmtLst, CondExpr, RelExpr
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

    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;
    CondExpr* getCondExpr() const {
        return condExpr.get();
    }

    virtual bool operator==(ASTNode const& o) const;
};

/**
 * @class Assign
 * @brief Class that represents an Assign statement in the AST.
 * @details Assign is identified by: var_name '=' expr ';' \n 
 * where: 
 * - expr: expr '+' term | expr '-' term | term
 * - term '*' factor | term '/' factor | term '%' factor | factor
 * - factor: var_name | const_value | '(' expr ')'
 * @see Var, Const, Expr, BinExpr
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

    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;

    Var* getLHS() const {
        return var.get();
    }

    Expr* getRHS() const {
        return expr.get();
    }

    virtual bool operator==(ASTNode const& o) const;
};


/**
 * @class IO
 * @brief Class that represents an input/output statement in the AST
 * and is the base class for Read and Print statements.
 * @details IO is identified by: read | print \n
 * where: 
 * - read: 'read' var_name';'
 * - print: 'print' var_name';'
 * @see Var, Const, Expr, BinExpr
 */
class IO : public Statement {
protected:
    std::unique_ptr<Var> var;
public:
    IO(
        int stmtNo,
        std::unique_ptr<Var> var
    ) :
        Statement(stmtNo),
        var(std::move(var)) {}

    const Var getVar() const { return *var; }

    virtual bool operator==(ASTNode const& o) const;
};

/**
 * @class Read
 * @brief Class that represents a Read statement in the AST.
 * @details Read is identified by: 'read' var_name;
 * @see Var
 */
class Read : public IO {
public:
    using IO::IO;
    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;
};

/**
 * @class Print
 * @brief Class that represents a Print statement in the AST.
 * @details Print is identified by: 'print' var_name;
 * @see Var
 */
class Print : public IO {
public:
    using IO::IO;
    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;
};

/**
 * @class Const
 * @brief Class that represents a constant in the AST.
 * @details Const is identified by: const_value \n
 * where:
 * - const_value: INTEGER
 *     - INTEGER: DIGIT+
 *     - DIGIT: 0-9
 */
class Const : public Expr {
private:
    int constValue;
public:
    explicit Const(int constValue) : constValue(constValue) {}
    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;
    const int getConstValue() const { return constValue; }

    virtual bool operator==(ASTNode const& o) const;
};

/**
 * @enum BinOp
 * @brief Enum that represents all possible operators in BinExpr.
 * @see BinExpr
 */
enum class BinOp {
    PLUS   = '+',  /**< Addition operator '+' */
    MINUS  = '-',  /**< Subtraction operator '-' */
    DIVIDE = '/',  /**< Division operator '/' */
    MULT   = '*',  /**< Multiplication operator '*' */
    MOD    = '%'  /**< Modulo operator '%' */
};

/**
 * @class BinExpr
 * @brief Class that represents a binary expression (2 Expr and a ::BinOp operator) in the AST.
 * @details BinExpr is composed of a ::BinOp operator and two Expr LHS and RHS. \n
 * BinExpr is identified by:
 * - from expr: expr '+' term | expr '-' term |
 * - from term: term '*' factor | term '/' factor | term '%' factor
 * @see Expr, Var, Const, ::BinOp
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

    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;

    virtual bool operator==(ASTNode const& o) const;
};

/**
 * @enum RelOp
 * @brief Enum that represents all possible operators in RelExpr.
 * @see RelExpr
 */
enum class RelOp {
    LTE,  /**< Less than or equals to operator "<=" */
    GTE,  /**< Greater than or equals to operator ">=" */
    LT,   /**< Less than "<" */
    GT,   /**< Greater than ">" */
    EQ,   /**< Equals operator "==" */
    NE    /**< Not equals operator "!=" */
};

/**
 * @class RelExpr
 * @brief Class that represents a relational expression (2 Expr and a ::RelOp operator) in the AST. 
 * @details RelExpr is composed of a ::RelOp operator and two Expr LHS and RHS. \n 
 * RelExpr is identified by: \n
 * rel_expr: rel_factor '>' rel_factor | rel_factor '>=' rel_factor | 
 * rel_factor '<' rel_factor | rel_factor '<' rel_factor | 
 * rel_factor '==' rel_factor | rel_factor '!=' rel_factor \n 
 * where: 
 * - rel_factor: var_name | const_value | expr
 * @see Expr, Var, Const, ::RelOp
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

    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;

    virtual bool operator==(ASTNode const& o) const;
};

/**
 * @enum CondOp
 * @brief Enum that represents all the possible operators in CondBinExpr and NotCondExpr.
 * @see CondBinExpr, NotCondExpr
 */
enum class CondOp {
    NOT,  /**< NOT operator "!" */
    AND,  /**< AND operator "&&" */
    OR    /**< OR operator "||" */
};


/**
 * @class CondBinExpr
 * @brief Represents a binary conditional expression (2 CondExpr and a ::CondOp operator)
 *  in the AST.
 * @details CondBinExpr is composed of a ::CondOp operator and two CondExpr LHS and RHS. \n 
 * CondBinExpr is identified by:
 * - from cond_expr: '(' cond_expr ')' '&&' '(' cond_expr ')' |
 * '(' cond_expr ')' '||' '(' cond_expr ')'
 * @see CondExpr, ::RelOp
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

    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;

    virtual bool operator==(ASTNode const& o) const;
};

/**
 * @class NotCondExpr
 * @brief Represents the negated conditional expression (::CondOp::NOT operator and a CondExpr)
 *  in the AST.
 * @details NotCondExpr is composed of a ::CondOp::NOT operator and a CondExpr. \n 
 * NotCondExpr is identified by:
 * - from cont_expr: '!' '(' cond_expr ')'
 * @see CondExpr, ::CondOp::NOT
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

    void accept(std::shared_ptr<ASTNodeVisitor> visitor) const;

    virtual bool operator==(ASTNode const& o) const;
};

/**
 * @brief Factory method for ASTNode with 1 argument.
 * 
 * @tparam T returning ASTNode type.
 * @tparam K parameter type that is used to construct the T object.
 * @param arg parameter of type K used to construct the T object. 
 * @return std::unique_ptr<T> a unique pointer to the T object. 
 */
template <class T, typename K>
std::unique_ptr<T> make(K arg) {
    return std::make_unique<T>(arg);
}

/**
 * @brief Factory method for ASTNode with 2 arguments.
 * 
 * @tparam T returning ASTNode type.
 * @tparam K parameter type that is used to construct the T object.
 * @tparam H parameter type that is used to construct the T object.
 * @param arg1 parameter of type K used to construct the T object. 
 * @param arg2 parameter of type H used to construct the T object. 
 * @return std::unique_ptr<T> a unique pointer to the T object. 
 */
template <class T, typename K, typename H>
std::unique_ptr<T> make(K arg1, H arg2) {
    return std::make_unique<T>(arg1, std::move(arg2));
}

/**
 * @brief Factory method for ASTNode with 3 arguments.
 * 
 * @tparam T returning ASTNode type.
 * @tparam K parameter type that is used to construct the T object.
 * @tparam H parameter type that is used to construct the T object.
 * @tparam V parameter type that is used to construct the T object.
 * @param arg1 parameter of type K used to construct the T object. 
 * @param arg2 parameter of type H used to construct the T object. 
 * @param arg3 parameter of type V used to construct the T object. 
 * @return std::unique_ptr<T> a unique pointer to the T object. 
 */
template <class T, typename K, typename H, typename V>
std::unique_ptr<T> make(K arg1, H arg2, V arg3) {
    return std::make_unique<T>(arg1, std::move(arg2), std::move(arg3));
}

/**
 * @brief Factory method for ASTNode with 4 arguments.
 * 
 * @tparam T returning ASTNode type.
 * @tparam K parameter type that is used to construct the T object.
 * @tparam H parameter type that is used to construct the T object.
 * @tparam V parameter type that is used to construct the T object.
 * @tparam Z parameter type that is used to construct the T object.
 * @param arg1 parameter of type K used to construct the T object. 
 * @param arg2 parameter of type H used to construct the T object. 
 * @param arg3 parameter of type V used to construct the T object. 
 * @param arg4 parameter of type Z used to construct the t object. 
 * @return std::unique_ptr<T> a unique pointer to the T object. 
 */
template <class T, typename K, typename H, typename V, typename Z>
std::unique_ptr<T> make(K arg1, H arg2, V arg3, Z arg4) {
    return std::make_unique<T>(arg1, std::move(arg2), std::move(arg3), std::move(arg4));
}

/**
 * @brief Variadic method for StmtLst with any number of arguments.
 * 
 * @tparam Ts parameter type that is used to construct the StmtLst object.
 * @param ts parameter of type T used to construct the StmtLst object.
 * @return StmtLst a list of statements.
 * @see StmtLst
 */
template <typename ... Ts>
StmtLst makeStmts(Ts &&... ts) {
    std::unique_ptr<Statement> stmtArr[] = { std::move(ts)... };
    auto lst = std::vector<std::unique_ptr<Statement>> { 
        std::make_move_iterator(std::begin(stmtArr)), std::make_move_iterator(std::end(stmtArr))
    };

    return StmtLst(lst);
}

}  // namespace AST
