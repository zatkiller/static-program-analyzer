#pragma once

/**
* An enum for the program design abstractions as well as their transitive counterparts (if any).
*/
enum class PKBRelationship {
    MODIFIES, USES, FOLLOWS, FOLLOWST, PARENT, PARENTT, CALLS, CALLST, NEXT, NEXTT, AFFECTS, AFFECTST
};

/**
* An enum for the types of statements. Apart from the six basic types, All and None are added
* to support the PKB implementation.
*/
enum class StatementType {
    Assignment, While, If, Read, Print, Call, All, None
};
