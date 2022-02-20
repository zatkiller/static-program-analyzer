#include "AST.h"

namespace sp {
namespace ast {

// Victims
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

// Equivalence Class Checks
bool Var::operator==(ASTNode const& o) const {
    if (typeid(*this) != typeid(o)) return false;
    auto that = static_cast<Var const&>(o);
    return this->varName == that.varName;
}

bool StmtLst::operator==(ASTNode const& o) const {
    if (typeid(*this) != typeid(o)) return false;
    auto that = static_cast<StmtLst const*>(&o);
    if (list.size() != that->list.size()) return false;
    for (int i = 0; i < list.size(); i++) {
        if (!(*list[i] == *that->list[i])) {
            return false;
        }
    }
    return true;
}

bool Procedure::operator==(ASTNode const& o) const {
    if (typeid(*this) != typeid(o)) return false;
    auto that = static_cast<Procedure const*>(&o);
    return (this->procName == that->procName) &&
        (this->stmtLst == that->stmtLst);
}

bool Program::operator==(ASTNode const& o) const {
    if (typeid(*this) != typeid(o)) return false;
    auto that = static_cast<const Program*>(&o);
    return *this->procedure == *that->procedure;
}

bool If::operator==(ASTNode const& o) const {
    if (typeid(*this) != typeid(o)) return false;
    auto that = static_cast<If const*>(&o);
    return (*this->condExpr == *that->condExpr) &&
        (this->thenBlk == that->thenBlk) &&
        (this->elseBlk == that->elseBlk) &&
        (this->getStmtNo() == that->getStmtNo());
}

bool While::operator==(ASTNode const& o) const {
    if (typeid(*this) != typeid(o)) return false;
    auto that = static_cast<While const*>(&o);
    return (*this->condExpr == *that->condExpr) &&
        (this->stmtLst == that->stmtLst) &&
        (this->getStmtNo() == that->getStmtNo());
}

bool Assign::operator==(ASTNode const& o) const {
    if (typeid(*this) != typeid(o)) return false;
    auto that = static_cast<Assign const*>(&o);
    return (*this->var == *that->var) &&
        (*this->expr == *that->expr) &&
        (this->getStmtNo() == that->getStmtNo());
}

bool IO::operator==(ASTNode const& o) const {
    if (typeid(*this) != typeid(o)) return false;
    auto that = static_cast<IO const*>(&o);
    return *this->var == *that->var &&
        (this->getStmtNo() == that->getStmtNo());
}

bool Const::operator==(ASTNode const& o) const {
    if (typeid(*this) != typeid(o)) return false;
    auto that = static_cast<Const const*>(&o);
    return this->constValue == that->constValue;
}

bool BinExpr::operator==(ASTNode const& o) const {
    if (typeid(*this) != typeid(o)) return false;
    auto that = static_cast<BinExpr const*>(&o);
    return (this->Op == that->Op) &&
        (*this->LHS == *that->LHS) &&
        (*this->RHS == *that->RHS);
}

bool RelExpr::operator==(ASTNode const& o) const {
    if (typeid(*this) != typeid(o)) return false;
    auto that = static_cast<RelExpr const*>(&o);
    return (this->Op == that->Op) &&
        (*this->LHS == *that->LHS) &&
        (*this->RHS == *that->RHS);
}

bool CondBinExpr::operator==(ASTNode const& o) const {
    if (typeid(*this) != typeid(o)) return false;
    auto that = static_cast<CondBinExpr const*>(&o);
    return (this->Op == that->Op) &&
        (*this->LHS == *that->LHS) &&
        (*this->RHS == *that->RHS);
}

bool NotCondExpr::operator==(ASTNode const& o) const {
    if (typeid(*this) != typeid(o)) return false;
    auto that = static_cast<NotCondExpr const*>(&o);
    return (this->Op == that->Op) &&
        (*this->condExpr == *that->condExpr);
}

}  // namespace ast
}  // namespace sp
