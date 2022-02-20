#include "DesignExtractor/PKBStrategy.h"

namespace sp {
namespace design_extractor {

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void ActualPKBStrategy::insertStatement(STMT_LO stmt) {
    pkb->insertStatement(stmt.type.value_or(StatementType::All), stmt.statementNum);
}

void ActualPKBStrategy::insertVariable(std::string name) {
    pkb->insertVariable(name);
}

void ActualPKBStrategy::insertEntity(Content entity) {
    std::visit(overloaded {
        [&](VAR_NAME& item) { pkb->insertVariable(item.name); },
        [&](STMT_LO& item) { pkb->insertStatement(item.type.value_or(StatementType::All), item.statementNum); },
        [&](PROC_NAME& item) { pkb->insertProcedure(item.name); },
        [&](CONST& item) { pkb->insertConstant(item); },
        [](auto& item) { Logger(Level::ERROR) << "PKBStrategy.cpp " << "Unsupported entity type"; },
    }, entity);
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
