#pragma once

#include <set>

#include "DesignExtractor/PKBStrategy.h"
#include "DesignExtractor/CFG/CFG.h"
#include "Parser/AST.h"
#include "PKB.h"
#include "TreeWalker.h"

namespace sp {
namespace design_extractor {

// A basic entity is represented by a Content type in PKB.
using Entity = Content;
// A relationship is made up of relationship type and the 2 content parameters
using Relationship = std::tuple<PKBRelationship, Content, Content>;
// An entry to PKB is either an Entity or a Relationship
using Entry = std::variant<Entity, Relationship>;

/**
 * @brief Base class for all AST based relationship collectors
 * 
 * Not all Extractors need an AST based Collector. But most of them do. So a shared base class
 * is extracted here.
 */
class Collector : public TreeWalker {
protected:
    std::set<Entry> entries;
public:
    const std::set<Entry> getEntries() { return entries; }
};

/**
 * @brief The base class for all design extractors
 * The extractor only has 1 responsibility, take a generic info of T and return a set of Entry type to be inserted into the PKB.
 * How the extractor perform the extraction from T is up to the extractor module itself.
 */
template<typename T>
struct Extractor {
    virtual std::set<Entry> extract(T) { 
        return std::set<Entry>(); 
    }
    virtual inline ~Extractor() {}
};

/**
 * @brief Define the ways to insert a single entry into the PKB
 */
class PKBInserter {
private:
    PKBStrategy *pkb;
public:
    explicit PKBInserter(PKBStrategy *pkb) : pkb(pkb) {}
    void insert(Entry entry);
};

/**
 * @brief An Extractor module is in charge of extracting the entity / relationship and inserting it into the PKB.
 * An Extractor module is composed of the extractor and the pkb inserter. The extractor is in charge of extracting the entries
 * while the inserter is in charge of inserting entries into PKB. 
 */
template<typename T>
class ExtractorModule {
private:
    std::unique_ptr<Extractor<T>> extractor;
    PKBInserter inserter;
public:
    ExtractorModule(std::unique_ptr<Extractor<T>> extractor, PKBStrategy *pkb) : 
        extractor(std::move(extractor)), inserter(pkb) {}
    void extract(T info) {
        auto entries = extractor->extract(info);
        for (auto &entry : entries) {
            inserter.insert(entry);
        }
    }
};


}  // namespace design_extractor
}  // namespace sp
