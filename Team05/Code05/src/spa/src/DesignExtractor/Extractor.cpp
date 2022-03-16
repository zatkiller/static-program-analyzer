#include <functional>
#include <variant>

#include "Extractor.h"


template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace sp {
namespace design_extractor {

void PKBInserter::insert(Entry entry) {
    std::visit(overloaded {
        [&](Entity &item) { pkb->insertEntity(item); },
        [&](Relationship &item) {
            std::apply([&](auto& rs, auto& a1, auto& a2){ pkb->insertRelationship(rs, a1, a2); }, item);
        },
        [](auto &item) {}
    }, entry);

}

}  // namespace design_extractor
}  // namespace sp
