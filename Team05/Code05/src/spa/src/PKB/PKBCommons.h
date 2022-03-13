#pragma once

enum class PKBRelationship {
    MODIFIES, USES, FOLLOWS, FOLLOWST, PARENT, PARENTT, CALLS, CALLST, NEXT, NEXTT
};

enum class StatementType {
    Assignment, While, If, Read, Print, Call, All, None
};
