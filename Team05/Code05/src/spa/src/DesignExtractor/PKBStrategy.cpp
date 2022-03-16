#include "DesignExtractor/PKBStrategy.h"

namespace sp {
namespace design_extractor {

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void ActualPKBStrategy::insertEntity(Content entity) {
    pkb->insertEntity(entity);
}

void ActualPKBStrategy::insertRelationship(PKBRelationship type, Content arg1, Content arg2) {
    pkb->insertRelationship(
        type,
        PKBField::createConcrete(arg1),
        PKBField::createConcrete(arg2)
    );
}
}  // namespace design_extractor
}  // namespace sp
