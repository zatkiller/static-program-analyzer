#pragma once

#include <set>
#include <deque>
#include <map>
#include <variant>
#include <utility>
#include <memory>
#include <string>

#include "Parser/AST.h"
#include "PKB.h"

using muTable = std::set<std::pair<std::variant<std::string, int>, std::string>>;  // modifies or uses table
using sTable = std::set<std::string>;  // string only table

/**
 * A foundation for all design extractor. Performs depth-first traversal on the AST and do nothing.
 * Children classes can inherit this class and override the specific methods that they want to perform
 * actions at.
 */
struct TreeWalker : public AST::ASTNodeVisitor {
    // one day I may be brave enough to use templates like https://www.foonathan.net/2017/12/visitors/
    // classic visitor design pattern will suffice for now.
    void visit(const AST::Program& node) override {};
    void visit(const AST::Procedure& node) override {};
    void visit(const AST::StmtLst& node) override {};
    void visit(const AST::If& node) override {};
    void visit(const AST::While& node) override {};
    void visit(const AST::Read& node) override {};
    void visit(const AST::Print& node) override {};
    void visit(const AST::Assign& node) override {};
    void visit(const AST::Var& node) override {};
    void visit(const AST::Const& node) override {};
    void visit(const AST::BinExpr& node) override {};
    void visit(const AST::RelExpr& node) override {};
    void visit(const AST::CondBinExpr& node) override {};
    void visit(const AST::NotCondExpr& node) override {};
    void enterContainer(std::variant<int, std::string> containerId) override {};
    void exitContainer() override {};
};

/**
 * Base class of all design extractors. Adds a PKB adaptor during construction
 */
class Extractor : public TreeWalker {
protected:
    PKB* pkb;
public:
    explicit Extractor(PKB* pkb) : pkb(pkb) {}
};
