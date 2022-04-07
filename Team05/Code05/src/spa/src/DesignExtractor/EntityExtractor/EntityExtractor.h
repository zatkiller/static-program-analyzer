#pragma once

#include <string>

#include "DesignExtractor/Extractor.h"

namespace sp {
namespace design_extractor {

struct StatementExtractorModule;
struct VariableExtractorModule;
struct ConstExtractorModule;
struct ProcedureExtractorModule;

/**
 * @brief Extracts entities using collectors of type T.
 * 
 * @tparam T the type of collector to be used for extracting entities.
 */
template<typename T>
class EntityExtractor : public Extractor<const ast::ASTNode*> {
public:
    using Extractor::Extractor;
    std::set<Entry> extract(const ast::ASTNode* node) override {
        T collector;
        node->accept(&collector);
        return collector.getEntries();
    }    
};

template<typename T>
class EntityExtractorModule : public ExtractorModule<const ast::ASTNode*> {
public:
    explicit EntityExtractorModule(PKBStrategy *pkb) :
        ExtractorModule(std::make_unique<EntityExtractor<T>>(), pkb) {}
};

struct VariableCollector : public Collector {
    void visit(const ast::Var& node) {
        entries.insert(VAR_NAME{node.getVarName()});
    }
};

struct VariableExtractorModule : public EntityExtractorModule<VariableCollector> {
    explicit VariableExtractorModule(PKBStrategy *pkb) : 
        EntityExtractorModule(pkb) {}
};

struct ConstCollector : public Collector {
    void visit(const ast::Const& node) {
        entries.insert(CONST{node.getConstValue()});
    }
};

struct ConstExtractorModule : public EntityExtractorModule<ConstCollector> {
    explicit ConstExtractorModule(PKBStrategy *pkb) :
        EntityExtractorModule(pkb) {}
};


struct ProcedureCollector : public Collector {
    void visit(const ast::Procedure& node) {
        entries.insert(PROC_NAME{node.getName()});
    }
};

struct ProcedureExtractorModule : public EntityExtractorModule<ProcedureCollector> {
    explicit ProcedureExtractorModule(PKBStrategy *pkb) :
        EntityExtractorModule(pkb) {}
};

struct StatementCollector : public Collector {
    void visit(const ast::Read& node) {
        entries.insert(STMT_LO{node.getStmtNo(), StatementType::Read, node.getVar().getVarName()});
    }

    void visit(const ast::Print& node) {
        entries.insert(STMT_LO{node.getStmtNo(), StatementType::Print, node.getVar().getVarName()});
    }

    void visit(const ast::While& node) {
        entries.insert(STMT_LO{node.getStmtNo(), StatementType::While});
    }

    void visit(const ast::If& node) {
        entries.insert(STMT_LO{node.getStmtNo(), StatementType::If});
    }

    void visit(const ast::Assign& node) {
        entries.insert(STMT_LO{node.getStmtNo(), StatementType::Assignment});
    }

    void visit(const ast::Call& node) {
        entries.insert(STMT_LO(node.getStmtNo(), StatementType::Call, node.getName()));
    }
};

struct StatementExtractorModule : public EntityExtractorModule<StatementCollector> {
    explicit StatementExtractorModule(PKBStrategy *pkb) :
        EntityExtractorModule(pkb) {}
};


}  // namespace design_extractor
}  // namespace sp
