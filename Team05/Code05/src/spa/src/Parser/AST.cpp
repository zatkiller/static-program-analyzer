#include "AST.h"

namespace AST {

void Var::accept(std::shared_ptr<ASTNodeVisitor> visitor) const {
    visitor->visit(*this);
}

void StmtLst::accept(std::shared_ptr<ASTNodeVisitor> visitor) const {
    visitor->visit(*this);
    for (auto& s : list) {
        s->accept(visitor);
    }
}

void Procedure::accept(std::shared_ptr<ASTNodeVisitor> visitor) const {
    visitor->enterContainer(procName);
    visitor->visit(*this);
    stmtLst.accept(visitor);
}

void Program::accept(std::shared_ptr<ASTNodeVisitor> visitor) const {
    visitor->visit(*this);
    procedure->accept(visitor);
}

void If::accept(std::shared_ptr<ASTNodeVisitor> visitor) const {
    visitor->visit(*this);
    condExpr->accept(visitor);

    visitor->enterContainer(getStmtNo());
    thenBlk.accept(visitor);
    visitor->exitContainer();

    visitor->enterContainer(getStmtNo());
    elseBlk.accept(visitor);
    visitor->exitContainer();
}

void While::accept(std::shared_ptr<ASTNodeVisitor> visitor) const {
    visitor->visit(*this);
    condExpr->accept(visitor);

    visitor->enterContainer(getStmtNo());
    stmtLst.accept(visitor);
    visitor->exitContainer();
}


void Assign::accept(std::shared_ptr<ASTNodeVisitor> visitor) const {
    visitor->visit(*this);
    var->accept(visitor);
    expr->accept(visitor);
}

void Read::accept(std::shared_ptr<ASTNodeVisitor> visitor) const {
    visitor->visit(*this);
    this->getVar().accept(visitor);
}

void Print::accept(std::shared_ptr<ASTNodeVisitor> visitor) const {
    visitor->visit(*this);
    this->getVar().accept(visitor);
}

void Const::accept(std::shared_ptr<ASTNodeVisitor> visitor) const {
    visitor->visit(*this);
}

void BinExpr::accept(std::shared_ptr <ASTNodeVisitor> visitor) const {
    visitor->visit(*this);
    LHS->accept(visitor);
    RHS->accept(visitor);
}

void RelExpr::accept(std::shared_ptr<ASTNodeVisitor> visitor) const {
    visitor->visit(*this);
    LHS->accept(visitor);
    RHS->accept(visitor);
}

void CondBinExpr::accept(std::shared_ptr<ASTNodeVisitor> visitor) const {
    visitor->visit(*this);
    LHS->accept(visitor);
    RHS->accept(visitor);
}

void NotCondExpr::accept(std::shared_ptr<ASTNodeVisitor> visitor) const {
    visitor->visit(*this);
    condExpr->accept(visitor);
}


}  // namespace AST
