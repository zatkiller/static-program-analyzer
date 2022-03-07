#include "TransitiveRelationshipTemplate.h"
#include "DesignExtractor/EntityExtractor/VariableExtractor.h"

namespace sp {
namespace design_extractor {
/**
 * Private strategy to only retrieve a local copy of variables.
 */
class VariablePKBStrategy : public NullPKBStrategy {
public:
    std::set<VAR_NAME> variables;
    void insertEntity(Content entity) override {
        try {
            variables.insert(std::get<VAR_NAME>(entity));
        } catch (std::bad_variant_access &ex) {
            Logger(Level::ERROR) << "Bad variant access in transitive relationship template extraction";
        }
    };
};


std::set<VAR_NAME> RelExtractorTemplate::extractVars(const ast::ASTNode *part) {
    VariablePKBStrategy vps;
    auto ve = std::make_unique<VariableExtractor>(&vps);
    part->accept(ve.get());

    return vps.variables;
}

void TransitiveRelationshipTemplate::visit(const ast::Call &node) {
    auto proc = PROC_NAME{node.getName()};
    try {
        auto vars = procVarMap.at(proc);
        for (auto var : vars) {
            insert(STMT_LO{node.getStmtNo(), StatementType::Call}, var);
        }
        cascadeToContainer(vars);
    } catch (std::out_of_range &ex) {
        // if out of range exception thrown by at, that means something wrong with topo ordering.
        Logger(Level::ERROR) << "TransitiveRelationshipTemplate.cpp " << 
        "empty proc at target " << proc.name << " something wrong with topo order";
    }

}


void TransitiveRelationshipTemplate::cascadeToContainer(const std::set<VAR_NAME> varNames) {
    for (auto stmt : container) {
        for (auto var : varNames) {
            insert(stmt, var);
        }
    }
    if (!currentProcedure.name.empty()) {
        for (auto var : varNames) {
            insert(currentProcedure, var);
        }
        procVarMap[currentProcedure].insert(varNames.begin(), varNames.end());
    }
}

void TransitiveRelationshipTemplate::extractAndInsert(Content a1, const ast::ASTNode *part) {
    auto vars = extractVars(part);
    for (auto var : vars) {
        insert(a1, var);
    }
    cascadeToContainer(vars);
}

void TransitiveRelationshipTemplate::enterContainer(std::variant<int, std::string> containerId) {
    int* pNum = std::get_if<int>(&containerId);
    if (pNum) {
        container.push_front(STMT_LO{ *pNum, stmtNumToType[*pNum] });
    } else {
        currentProcedure = PROC_NAME{ std::get<std::string>(containerId) };
    }
}
void TransitiveRelationshipTemplate::exitContainer() {
    container.pop_front();
}
}  // namespace design_extractor
}  // namespace sp
