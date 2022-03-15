#pragma once

#include "Parser/AST.h"
#include "PKB.h"
#include "DesignExtractor/PKBStrategy.h"

namespace sp {
namespace design_extractor {

// A basic entity is represented by a Content type in PKB.
using Entity = Content;
// A relationship is made up of relationship type and the 2 content parameters
using Relationship = std::tuple<PKBRelationship, Content, Content>;
// An entry to PKB is either an Entity or a Relationship
using Entry = std::variant<Entity, Relationship>;

/**
 * @brief The base class for all design extractors
 * The extractor only has 1 responsibility, take an AST and return a set of Entry type to be inserted into the PKB.
 * How the extractor perform the extraction from the AST is up to the extractor module itself.
 */
struct Extractor {
    virtual std::set<Entry> extract(const ast::ASTNode*) { 
        return std::set<Entry>(); 
    };
    virtual inline ~Extractor() {};
};

/**
 * @brief Define the ways to insert a single entry into the PKB
 */
class PKBInserter {
private:
    PKBStrategy *pkb;
public:
    PKBInserter(PKBStrategy *pkb) : pkb(pkb) {};
    void insert(Entry entry);
};

/**
 * @brief An Extractor module is in charge of extracting the entity / relationship and inserting it into the PKB.
 * An Extractor module is composed of the extractor and the pkb inserter. The extractor is in charge of extracting the entries
 * while the inserter is in charge of inserting entries into PKB. 
 */
class ExtractorModule {
private:
    std::unique_ptr<Extractor> extractor;
    PKBInserter inserter;
public:
    ExtractorModule(std::unique_ptr<Extractor> extractor, PKBStrategy *pkb) : extractor(std::move(extractor)), inserter(pkb) {};
    void extract(const ast::ASTNode *node) {
        auto entries = extractor->extract(node);
        for (auto &entry : entries) {
            inserter.insert(entry);
        }
    }
};


/**
 * A foundation for all design extractor. Performs depth-first traversal on the AST and do nothing.
 * Children classes can inherit this class and override the specific methods that they want to perform
 * actions at.
 */
struct TreeWalker : public ast::ASTNodeVisitor {
    // one day I may be brave enough to use templates like https://www.foonathan.net/2017/12/visitors/
    // classic visitor design pattern will suffice for now.
    void visit(const ast::Program& node) override {};
    void visit(const ast::Procedure& node) override {};
    void visit(const ast::StmtLst& node) override {};
    void visit(const ast::If& node) override {};
    void visit(const ast::While& node) override {};
    void visit(const ast::Read& node) override {};
    void visit(const ast::Print& node) override {};
    void visit(const ast::Assign& node) override {};
    void visit(const ast::Call&) override {};
    void visit(const ast::Var& node) override {};
    void visit(const ast::Const& node) override {};
    void visit(const ast::BinExpr& node) override {};
    void visit(const ast::RelExpr& node) override {};
    void visit(const ast::CondBinExpr& node) override {};
    void visit(const ast::NotCondExpr& node) override {};
    void enterContainer(std::variant<int, std::string> containerId) override {};
    void exitContainer() override {};
};

}  // namespace design_extractor
}  // namespace sp
