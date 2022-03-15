#pragma once

#include "StatementBucketTemplate.h"

namespace sp {
namespace design_extractor {
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
 * Extracts all follows relationship from the AST and send them to PKB Adaptor.
 */
class FollowsExtractor : public IExtractor {
public:
    using IExtractor::IExtractor;
    std::set<Entry> extract(const ast::ASTNode* node) override {
        FollowsCollector collector;
        node->accept(&collector);
        return collector.relationships;
    }
};

class FollowsExtractorModule : public ExtractorModule {
public:
    FollowsExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<FollowsExtractor>(), pkb) {};
};
}  // namespace design_extractor
}  // namespace sp
