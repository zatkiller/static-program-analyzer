#pragma once

#include <unordered_map>
#include <variant>

#include "DesignExtractor/Extractor.h"
#include "PKB/PKBField.h"

namespace sp {
namespace design_extractor {
using Depth = int;
using Bucket = std::unordered_map<Depth, STMT_LO>;

/**
 * @brief A template method abstract base class for hierarchy based relationship extractors.
 * 
 * It contains a bucket which contains the current state of statement traversal where the last statement
 * of each nested depth is stored. At each statement, the virtual insert method is called to insert into
 * pkb based on the derived class' implementation.
 */
class StatementBucketTemplate : public TreeWalker {
private:
    Depth currentDepth = 0;
    Bucket bucket;

    void enterBucket(STMT_LO stmt) {
        bucket.insert_or_assign(currentDepth, stmt);
    }

    virtual void insert(const Bucket&, Depth, Content) = 0;

public:
    std::set<Entry> relationships;
    void visit(const ast::If& node) override {
        auto stmt = STMT_LO{node.getStmtNo(), StatementType::If};
        insert(bucket, currentDepth, stmt);
        enterBucket(stmt);
    }
    void visit(const ast::While& node) override {
        auto stmt = STMT_LO{node.getStmtNo(), StatementType::While};
        insert(bucket, currentDepth, stmt);
        enterBucket(stmt);
    }
    void visit(const ast::Read& node) override {
        auto stmt = STMT_LO{node.getStmtNo(), StatementType::Read};
        insert(bucket, currentDepth, stmt);
        enterBucket(stmt);
    }
    void visit(const ast::Print& node) override {
        auto stmt = STMT_LO{node.getStmtNo(), StatementType::Print};
        insert(bucket, currentDepth, stmt);
        enterBucket(stmt);
    }
    void visit(const ast::Assign& node) override {
        auto stmt = STMT_LO{node.getStmtNo(), StatementType::Assignment};
        insert(bucket, currentDepth, stmt);
        enterBucket(stmt);
    }
    void visit(const ast::Call& node) override {
        auto stmt = STMT_LO{node.getStmtNo(), StatementType::Call};
        insert(bucket, currentDepth, stmt);
        enterBucket(stmt);
    }

    void enterContainer(std::variant<int, std::string> containerId) override {
        if (std::get_if<int>(&containerId) != nullptr) {
            currentDepth++;
        } else if (std::get_if<std::string>(&containerId) != nullptr) {
            currentDepth = 0;
            bucket.clear();
        }
    }
    
    void exitContainer() override {
        /**
         * Clear the saved statement on exit. To prevent 2 container having a relationship.
         * if (cond) then {
         *     read x;
         * } else {
         *     print x;
         * }
         */
        bucket.erase(bucket.find(currentDepth));
        currentDepth--;
    }
};
}  // namespace design_extractor
}  // namespace sp
