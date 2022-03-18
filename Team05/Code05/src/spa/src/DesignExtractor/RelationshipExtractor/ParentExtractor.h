#pragma once

#include "StatementBucketTemplate.h"

namespace sp {
namespace design_extractor {
class ParentCollector : public StatementBucketTemplate {
public:
    using StatementBucketTemplate::StatementBucketTemplate;
private:
    void insert(const Bucket& bucket, Depth depth, Content stmt) override {
        // A saved statement at the previous depth means there is a container statement that contains our current stmt.
        if (bucket.find(depth-1) != bucket.end()) {
            // The saved statement at the previous depth should be parent of our current statement.
            relationships.insert(Relationship(PKBRelationship::PARENT, bucket.at(depth-1), stmt));
        }
    };
};

/**
 * Extracts all Parent relationship from the AST and return them as a set of entries
 */
class ParentExtractor : public Extractor<const ast::ASTNode*> {
public:
    using Extractor::Extractor;
    std::set<Entry> extract(const ast::ASTNode* node) override {
        ParentCollector collector;
        node->accept(&collector);
        return collector.relationships;
    }
};

/**
 * Extracts all Parent relationship from the AST and send them to the PKBStrategy
 */
class ParentExtractorModule : public ExtractorModule<const ast::ASTNode*> {
public:
    explicit ParentExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<ParentExtractor>(), pkb) {}
};

}  // namespace design_extractor
}  // namespace sp
