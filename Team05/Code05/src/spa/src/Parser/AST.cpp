#include "AST.h"

using namespace AST;

void Var::accept(ASTNodeVisitor& visitor) const {
    visitor.visit(*this);
}

void StmtLst::accept(ASTNodeVisitor& visitor) const {
    visitor.visit(*this);
    for (auto& s : list) {
        s->accept(visitor);
    }
}

void Procedure::accept(ASTNodeVisitor& visitor) const {
    visitor.enterContainer(procName);
    visitor.visit(*this);
    stmtLst.accept(visitor);
}

void Program::accept(ASTNodeVisitor& visitor) const {
    visitor.visit(*this);
    procedure->accept(visitor);
}

void If::accept(ASTNodeVisitor & visitor) const {
    visitor.visit(*this);
    condExpr->accept(visitor);

    visitor.enterContainer(getStmtNo());
    thenBlk.accept(visitor);
    visitor.exitContainer();

    visitor.enterContainer(getStmtNo());
    elseBlk.accept(visitor);
    visitor.exitContainer();
}

void While::accept(ASTNodeVisitor& visitor) const {
    visitor.visit(*this);
    condExpr->accept(visitor);

    visitor.enterContainer(getStmtNo());
    stmtLst.accept(visitor);
    visitor.exitContainer();
}