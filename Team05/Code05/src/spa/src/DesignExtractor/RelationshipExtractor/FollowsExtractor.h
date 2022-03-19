#pragma once

#include "StatementBucketTemplate.h"

namespace sp {
namespace design_extractor {

/**
 * a ASTNodeVisitor that collects the Follows relationship as it walks through the AST.
 */
class FollowsCollector : public StatementBucketTemplate {
public:
    using StatementBucketTemplate::StatementBucketTemplate;
private:
    void insert(const Bucket& bucket, Depth depth, Content stmt) override {
        // A saved statement at the current depth means there is a previous statement to our current stmt.
        if (bucket.find(depth) != bucket.end()) {
            // The saved statement at the current depth should be followed by our current statement.
            relationships.insert(Relationship(PKBRelationship::FOLLOWS, bucket.at(depth), stmt));
        }
    };
};

/**
 * Extracts all Follows relationship from the AST and return them as a set of entries
 */
class FollowsExtractor : public Extractor<const ast::ASTNode*> {
public:
    using Extractor::Extractor;
    std::set<Entry> extract(const ast::ASTNode* node) override {
        FollowsCollector collector;
        node->accept(&collector);
        return collector.relationships;
    }
};

/**
 * Extracts all Follows relationship from the AST and send them to the PKBStrategy
 */
class FollowsExtractorModule : public ExtractorModule<const ast::ASTNode*> {
public:
    explicit FollowsExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<FollowsExtractor>(), pkb) {}
};
}  // namespace design_extractor
}  // namespace sp
