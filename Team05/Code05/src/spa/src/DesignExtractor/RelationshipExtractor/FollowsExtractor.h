#pragma once

#include "StatementBucketTemplate.h"

namespace sp {
class FollowsExtractor : public StatementBucketTemplate {
public:
    using StatementBucketTemplate::StatementBucketTemplate;
private:
    void insert(const Bucket& bucket, Depth depth, Content stmt) const override {
        // A saved statement at the current depth means there is a previous statement to our current stmt.
        if (bucket.find(depth) != bucket.end()) {
            // The saved statement at the current depth should be followed by our current statement.
            pkb->insertRelationship(PKBRelationship::FOLLOWS, bucket.at(depth), stmt);
        }
    };
};
}  // namespace sp
