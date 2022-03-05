#pragma once
#include <string>

#include "PKB.h"

namespace sp {
namespace design_extractor {
/**
 * Interface for handling PKB insertions. There could be different strategy like NullPKBStrategy, TestPKBStrategy, ActualPKBStrategy
 */
struct PKBStrategy {

    /**
    * Inserts an entity into the PKB.
    *
    * @param Content the entity to be inserted.
    */
    virtual void insertEntity(Content) = 0;

    /**
    * Inserts a relationship into the PKB.
    *
    * @param type relationship type
    * @param entity1 the first program design entity in the relationship
    * @param entity2 the second program design entity in the relationship
    */
    virtual void insertRelationship(PKBRelationship, Content, Content) = 0;
};


/**
 * Insert into null space. No data will be saved.
 */
class NullPKBStrategy : public PKBStrategy {
public:
    void insertEntity(Content) override {};
    void insertRelationship(PKBRelationship, Content, Content) override {};
};

/**
 * Insert data into actual PKB.
 */
class ActualPKBStrategy : public PKBStrategy {
private:
    PKB *pkb;
public:
    explicit ActualPKBStrategy(PKB *pkb) : pkb(pkb) {}
    void insertEntity(Content) override;
    void insertRelationship(PKBRelationship, Content, Content) override;
};
}  // namespace design_extractor
}  // namespace sp
