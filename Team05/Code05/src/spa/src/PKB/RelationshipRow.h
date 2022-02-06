#pragma once

#include <stdio.h>

#include "PKBField.h"

class RelationshipRow {
public:
    RelationshipRow(PKBField, PKBField);

    bool operator == (const RelationshipRow&) const;
    PKBField getFirst() const;
    PKBField getSecond() const;

private:
    PKBField entity1;
    PKBField entity2;
};

class RelationshipRowHash {
    public:
        size_t operator() (const RelationshipRow&) const;
};
