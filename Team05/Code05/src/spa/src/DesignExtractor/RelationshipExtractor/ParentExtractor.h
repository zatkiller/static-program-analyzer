#pragma once

#include "StatementBucketTemplate.h"

namespace sp {
class ParentExtractor : public StatementBucketTemplate {
public:
    using StatementBucketTemplate::StatementBucketTemplate;
private:
    void insert(const Bucket& bucket, Depth depth, Content stmt) const override {
        // A saved statement at the previous depth means there is a container statement that contains our current stmt.
        if (bucket.find(depth-1) != bucket.end()) {
            // The saved statement at the previous depth should be parent of our current statement.
            pkb->insertRelationship(PKBRelationship::PARENT, bucket.at(depth-1), stmt);
        }
    };
};
}  // namespace sp
