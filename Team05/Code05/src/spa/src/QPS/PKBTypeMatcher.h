#include "PKB/PKBCommons.h"
#include "Query.h"

namespace qps::evaluator {
class PKBTypeMatcher {
public:
    /**
     * Maps PKBRelationships based on RelRef type.
     *
     * @param r relRefType
     * @return PKBRelationship
     */
    static PKBRelationship getPKBRelationship(query::RelRefType r);

    /**
     * Maps DesignEntity to corresponding PKB StatementType.
     *
     * @param d DesignEntity
     * @return StatementType
     */
    static StatementType getStatementType(query::DesignEntity d);
};
}  // namespace qps::evaluator
