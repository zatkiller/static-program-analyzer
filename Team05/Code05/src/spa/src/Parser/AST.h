#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace AST {

	class Procedure;
	class Statement;
	class Expr;
	class CondExpr;
	class BinExpr;
	class Var;
	class Const;
	class StmtLst;

	class Read;
	class Print;

	struct IOVisitor {
		virtual void visitRead(const Read& node) = 0;
		virtual void visitPrint(const Print& node) = 0;
	};


	class ASTNode {
	public:
		// TODO: visiter method to be added
		virtual ~ASTNode() {}
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
		Statement(int stmtNo) : stmtNo(stmtNo) {};
		int getStmtNo() {
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
		Program(std::unique_ptr<Procedure> procedure) :
			procedure(std::move(procedure)) {}
	};	

	class StmtLst : public ASTNode {
	private:
		std::vector<std::unique_ptr<Statement>> list;
	public:
		StmtLst(
			std::vector<std::unique_ptr<Statement>>& list
		) : list(std::move(list)) {};
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
		Var(const std::string& varName) : varName(varName) {};
		std::string& getVarName() { return varName; }
		void setVarName(std::string& name) { this->varName = name; };
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
			var(std::move(var)) {};

		virtual void accept(IOVisitor& visitor) const = 0;
	};

	/**
	 * Represents a Read statement in the AST.
	 * Identified by:
	 *  'read' var_name;
	 */
	class Read : public IO {
	public:
		using IO::IO;
		void accept(IOVisitor& visitor) const {
			visitor.visitRead(*this);
		}
	};

	/**
	 * Represents a Print statement in the AST.
	 * Identified by:
	 *  'print' var_name;
	 */
	class Print : public IO {
	public:
		using IO::IO;
		void accept(IOVisitor& visitor) const {
			visitor.visitPrint(*this);
		}
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
		explicit Const(int constValue) : constValue(constValue) {};
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
	};

	/**
	 * Represents the possible operators in CondBinExpr and NotCondExpr
	 */
	enum class CondOp {
		NOT, // !
		AND, // &&
		OR   // ||
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
	};

}
