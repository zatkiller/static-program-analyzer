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
 * @brief A base dataclass that can be used to collect entities by walking the AST.
 */
struct EntityCollector : public TreeWalker {
    std::set<Entry> entities;
};

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
        return collector.entities;
    }    
};

template<typename T>
class EntityExtractorModule : public ExtractorModule<const ast::ASTNode*> {
public:
    explicit EntityExtractorModule(PKBStrategy *pkb) :
        ExtractorModule(std::make_unique<EntityExtractor<T>>(), pkb) {}
};

struct VariableCollector : public EntityCollector {
    void visit(const ast::Var& node) {
        entities.insert(VAR_NAME{node.getVarName()});
    }
};

struct VariableExtractorModule : public EntityExtractorModule<VariableCollector> {
    explicit VariableExtractorModule(PKBStrategy *pkb) : 
        EntityExtractorModule(pkb) {}
};

struct ConstCollector : public EntityCollector {
    void visit(const ast::Const& node) {
        entities.insert(CONST{node.getConstValue()});
    }
};

struct ConstExtractorModule : public EntityExtractorModule<ConstCollector> {
    explicit ConstExtractorModule(PKBStrategy *pkb) :
        EntityExtractorModule(pkb) {}
};


struct ProcedureCollector : public EntityCollector {
    void visit(const ast::Procedure& node) {
        entities.insert(PROC_NAME{node.getName()});
    }
};

struct ProcedureExtractorModule : public EntityExtractorModule<ProcedureCollector> {
    explicit ProcedureExtractorModule(PKBStrategy *pkb) :
        EntityExtractorModule(pkb) {}
};

struct StatementCollector : public EntityCollector {
    void visit(const ast::Read& node) {
        entities.insert(STMT_LO{node.getStmtNo(), StatementType::Read, node.getVar().getVarName()});
    }

    void visit(const ast::Print& node) {
        entities.insert(STMT_LO{node.getStmtNo(), StatementType::Print, node.getVar().getVarName()});
    }

    void visit(const ast::While& node) {
        entities.insert(STMT_LO{node.getStmtNo(), StatementType::While});
    }

    void visit(const ast::If& node) {
        entities.insert(STMT_LO{node.getStmtNo(), StatementType::If});
    }

    void visit(const ast::Assign& node) {
        entities.insert(STMT_LO{node.getStmtNo(), StatementType::Assignment});
    }

    void visit(const ast::Call& node) {
        entities.insert(STMT_LO(node.getStmtNo(), StatementType::Call, node.getName()));
    }
};

struct StatementExtractorModule : public EntityExtractorModule<StatementCollector> {
    explicit StatementExtractorModule(PKBStrategy *pkb) :
        EntityExtractorModule(pkb) {}
};


}  // namespace design_extractor
}  // namespace sp
