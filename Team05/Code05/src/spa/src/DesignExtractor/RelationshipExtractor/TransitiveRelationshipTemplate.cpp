#include "TransitiveRelationshipTemplate.h"
#include "DesignExtractor/EntityExtractor/VariableExtractor.h"

/**
 * Private strategy to only retrieve a local copy of variables.
 */
class VariablePKBStrategy : public NullPKBStrategy {
public:
    std::set<VAR_NAME> variables;
    void insertVariable(std::string name) override {
        variables.insert(VAR_NAME{name});
    };
};

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
    }
};

std::set<VAR_NAME> RelExtractorTemplate::extract(const AST::ASTNode *part) {
    VariablePKBStrategy vps;
    auto ve = std::make_shared<VariableExtractor>(&vps);
    part->accept(ve);

    return vps.variables;
};

void TransitiveRelationshipTemplate::extractAndInsert(Content a1, const AST::ASTNode *part) {
    auto vars = extract(part);
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
};
void TransitiveRelationshipTemplate::exitContainer() {
    container.pop_front();
};
