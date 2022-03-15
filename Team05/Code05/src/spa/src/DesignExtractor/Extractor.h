#pragma once

#include "TreeWalker.h"
#include "Parser/AST.h"
#include "PKB.h"
#include "DesignExtractor/PKBStrategy.h"
#include "DesignExtractor/CFG/CFG.h"

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
    virtual std::set<Entry> extract(const std::map<std::string, std::shared_ptr<cfg::CFGNode>>&) { 
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
    void extract(const std::map<std::string, std::shared_ptr<cfg::CFGNode>>& cfgs) { 
        auto entries = extractor->extract(cfgs);
        for (auto &entry : entries) {
            inserter.insert(entry);
        }
    };
};


}  // namespace design_extractor
}  // namespace sp
