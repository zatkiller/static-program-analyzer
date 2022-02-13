#pragma once

#include <unordered_map>

#include "DesignExtractor/Extractor.h"
#include "PKB/PKBDataTypes.h"


using Depth = int;
using Bucket = std::unordered_map<Depth, STMT_LO>;

/**
 * @brief A template method abstract base class for hierarchy based relationship extractors.
 * 
 * It contains a bucket which contains the current state of statement traversal where the last statement
 * of each nested depth is stored. At each statement, the virtual insert method is called to insert into
 * pkb based on the derived class' implementation.
 */
class StatementBucketTemplate : public Extractor {
private:
    Depth currentDepth = 0;
    Bucket bucket;

    void enterBucket(STMT_LO stmt) {
        bucket.insert_or_assign(currentDepth, stmt);
    }

    virtual void insert(const Bucket&, Depth, Content) const = 0;
public:
    using Extractor::Extractor;
    void visit(const AST::If& node) override {
        auto stmt = STMT_LO{node.getStmtNo(), StatementType::If};
        insert(bucket, currentDepth, stmt);
        enterBucket(stmt);
    };
    void visit(const AST::While& node) override {
        auto stmt = STMT_LO{node.getStmtNo(), StatementType::While};
        insert(bucket, currentDepth, stmt);
        enterBucket(stmt);
    };
    void visit(const AST::Read& node) override {
        auto stmt = STMT_LO{node.getStmtNo(), StatementType::Read};
        insert(bucket, currentDepth, stmt);
        enterBucket(stmt);
    };
    void visit(const AST::Print& node) override {
        auto stmt = STMT_LO{node.getStmtNo(), StatementType::Print};
        insert(bucket, currentDepth, stmt);
        enterBucket(stmt);
    };
    void visit(const AST::Assign& node) override {
        auto stmt = STMT_LO{node.getStmtNo(), StatementType::Assignment};
        insert(bucket, currentDepth, stmt);
        enterBucket(stmt);
    };

    void enterContainer(std::variant<int, std::string> containerId) override {
        currentDepth++;
    };
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
    };
};