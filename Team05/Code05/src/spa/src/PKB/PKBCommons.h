#pragma once

enum class PKBRelationship {
    MODIFIES, USES, FOLLOWS, FOLLOWST, PARENT, PARENTT, CALLS, CALLST
};

enum class StatementType {
    Assignment, While, If, Read, Print, Call, All, None
};
