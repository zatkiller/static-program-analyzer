#pragma once

#include "Parser/AST.h"
#include "PKB.h"
#include "DesignExtractor/PKBStrategy.h"

namespace sp {

/**
 * A foundation for all design extractor. Performs depth-first traversal on the AST and do nothing.
 * Children classes can inherit this class and override the specific methods that they want to perform
 * actions at.
 */
struct TreeWalker : public sp::ast::ASTNodeVisitor {
    // one day I may be brave enough to use templates like https://www.foonathan.net/2017/12/visitors/
    // classic visitor design pattern will suffice for now.
    void visit(const sp::ast::Program& node) override {};
    void visit(const sp::ast::Procedure& node) override {};
    void visit(const sp::ast::StmtLst& node) override {};
    void visit(const sp::ast::If& node) override {};
    void visit(const sp::ast::While& node) override {};
    void visit(const sp::ast::Read& node) override {};
    void visit(const sp::ast::Print& node) override {};
    void visit(const sp::ast::Assign& node) override {};
    void visit(const sp::ast::Var& node) override {};
    void visit(const sp::ast::Const& node) override {};
    void visit(const sp::ast::BinExpr& node) override {};
    void visit(const sp::ast::RelExpr& node) override {};
    void visit(const sp::ast::CondBinExpr& node) override {};
    void visit(const sp::ast::NotCondExpr& node) override {};
    void enterContainer(std::variant<int, std::string> containerId) override {};
    void exitContainer() override {};
};

/**
 * Base class of all design extractors. Adds a PKB adaptor during construction
 */
class Extractor : public TreeWalker {
protected:
    PKBStrategy* pkb;
public:
    explicit Extractor(PKBStrategy* pkb) : pkb(pkb) {}
};
}  // namespace sp
