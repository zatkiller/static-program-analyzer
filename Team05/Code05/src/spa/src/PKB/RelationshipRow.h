#pragma once

#include <stdio.h>

#include "PKBField.h"

/**
* A data structure to store a program design abstraction (relationship) which has two fields.
*/
class RelationshipRow {
public:
    RelationshipRow(PKBField, PKBField);

    /**
    * Retrieves the first field in the relationship.
    */
    PKBField getFirst() const;
    
    /**
    * Retrieves the second field in the relationship.
    */
    PKBField getSecond() const;

    bool operator == (const RelationshipRow&) const;

private:
    PKBField field1;
    PKBField field2;
};

/** 
* Hash function for RelationshipRow.
*/
class RelationshipRowHash {
    public:
        size_t operator() (const RelationshipRow&) const;
};
