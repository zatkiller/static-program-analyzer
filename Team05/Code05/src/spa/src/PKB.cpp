#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "logging.h"
#include "PKB.h"
#include "DesignExtractor/EntityExtractor/EntityExtractor.h"

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

/**
* Class that extracts Affects relationships provided a CFG 
*/
class AffectsCacher {
public:
    /**
    * Extracts all Affects relationships from the provided CFG
    * 
    * @param cfgContainer The CFG from which to extract Affects relationships
    * @return A set of pairs of STMT_LOs, representing the result of the extraction
    * @see CFG
    */
    CacheResults evalAffects(const sp::cfg::CFG cfgContainer) {
        // Initialize visited set for main traversal
        CfgNodeSet visited;

        // Traverse and extract relationships from each procedure's CFG
        for (auto varCfgPair : cfgContainer.cfgs) {
            // Skip first dummy node
            auto root = varCfgPair.second;
            if (!root->stmt.has_value() && root->getChildren().size() != 0) {
                root = root->getChildren().at(0).lock();
            }

            extractAndCacheFrom(root, &visited);
        }
        return res;
    }

private:
    CacheResults res;

    void extractAndCacheFrom(NodePtr start, CfgNodeSet* visited) {
        auto curr = start.lock().get();

        // Case where node has already been visited
        if (visited->find(curr) != visited->end()) {
            return;
        }

        // Deal with case where node is dummy
        if (!curr->stmt.has_value()) {
            // Get children
            std::vector<NodePtr> children = curr->getChildren();
            for (auto child : children) {
                extractAndCacheFrom(child, visited);
            }
        } else {
            // Add to visited list
            visited->insert(curr);

            // Get statement at curr
            STMT_LO currStmt = curr->stmt.value();

            // Check type of stmt
            bool isAssignNode = currStmt.type.value() == StatementType::Assignment;

            // Do secondary traversal if node is assignment statement
            if (isAssignNode) {
                // Extract variable of interest (i.e. var being modified)
                auto voi = curr->modifies;

                // Get children
                std::vector<NodePtr> nextNodes = curr->getChildren();
                for (auto nextNode : nextNodes) {
                    CfgNodeSet secVisited;
                    walkAndExtract(nextNode, voi, start, &secVisited);
                }
            }

            // Continue traversal
            std::vector<NodePtr> children = curr->getChildren();
            for (auto child : children) {
                extractAndCacheFrom(child, visited);
            }
        }
    }

    void walkAndExtract(NodePtr curr, std::unordered_set<VAR_NAME> voi, NodePtr src,
        CfgNodeSet* visited) {
        // Get children
        auto children = curr.lock()->getChildren();

        // Check if dummy node
        if (!curr.lock()->stmt.has_value()) {
            // If children exist walk them
            if (children.empty()) {
                return;
            }
            for (auto child : children) {
                walkAndExtract(child, voi, src, visited);
            }
            return;
        }

        // Check for affects relationship
        STMT_LO currStmt = curr.lock()->stmt.value();
        bool isAssignNode = currStmt.type.value() == StatementType::Assignment;
        bool hasRs = false;
        auto usedVars = curr.lock()->uses;
        for (auto var : voi) {
            if (usedVars.find(var) != usedVars.end()) {
                hasRs = true;
                break;
            }
        }
        if (hasRs && isAssignNode) {
            STMT_LO srcStmt = src.lock()->stmt.value();
            std::pair stmtPair = std::make_pair(srcStmt, currStmt);
            res.insert(stmtPair);
        }

        // Check if node modifies voi
        if (!curr.lock()->modifies.empty()) {
            auto modVars = curr.lock().get()->modifies;
            for (auto var : voi) {
                if (modVars.find(var) != modVars.end()) {
                    visited->insert(curr.lock().get());
                    return;
                }
            }
        }

        // Check if node already visited
        if (visited->find(curr.lock().get()) != visited->end()) {
            return;
        }

        // Update visited list and continue traversal
        visited->insert(curr.lock().get());
        for (auto child : children) {
            walkAndExtract(child, voi, src, visited);
        }
    }

    bool isAssignment(PKBField field) {
        return field.getContent<STMT_LO>()->type.value() == StatementType::Assignment;
    }
};

PKB::PKB() {
    relationshipTables.emplace(PKBRelationship::FOLLOWS, std::make_shared<FollowsRelationshipTable>());
    relationshipTables.emplace(PKBRelationship::MODIFIES, std::make_shared<ModifiesRelationshipTable>());
    relationshipTables.emplace(PKBRelationship::CALLS, std::make_shared<CallsRelationshipTable>());
    relationshipTables.emplace(PKBRelationship::NEXT, std::make_shared<NextRelationshipTable>());
    relationshipTables.emplace(PKBRelationship::PARENT, std::make_shared<ParentRelationshipTable>());
    relationshipTables.emplace(PKBRelationship::USES, std::make_shared<UsesRelationshipTable>());
    relationshipTables.emplace(PKBRelationship::AFFECTS, std::make_shared<AffectsRelationshipTable>());

    statementTable = std::make_unique<StatementTable>();
    variableTable = std::make_unique<VariableTable>();
    constantTable = std::make_unique<ConstantTable>();
    procedureTable = std::make_unique<ProcedureTable>();
}

// INSERT API
void PKB::insertEntity(Content entity) {
    std::visit(overloaded{
        [&](VAR_NAME& item) { insertVariable(item); },
        [&](STMT_LO& item) { insertStatement(item); },
        [&](PROC_NAME& item) { insertProcedure(item); },
        [&](CONST& item) { insertConstant(item); },
        [](auto& item) { Logger(Level::ERROR) << "PKB.cpp " << "Unsupported entity type"; }
        }, entity);
}

void PKB::insertStatement(STMT_LO stmt) {
    statementTable->insert(stmt);
}


void PKB::insertVariable(VAR_NAME var) {
    variableTable->insert(var);
}

void PKB::insertProcedure(PROC_NAME proc) {
    procedureTable->insert(proc);
}

void PKB::insertConstant(CONST constant) {
    constantTable->insert(constant);
}

void PKB::insertAST(std::unique_ptr<sp::ast::Program> root) {
    this->root = std::move(root);
}

void PKB::insertCFG(const sp::cfg::CFG cfgContainer) {
    this->cfgContainer = cfgContainer;
}

bool PKB::validate(const PKBField field) const {
    switch (field.entityType) {
    case PKBEntityType::VARIABLE:
        return validateVariable(field);
    case PKBEntityType::PROCEDURE:
        return validateProcedure(field);
    case PKBEntityType::STATEMENT:
        return validateStatement(field);
    default:  // const
        return true;
    }
}

bool PKB::validateVariable(const PKBField field) const {
    if (field.fieldType == PKBFieldType::CONCRETE) {
        auto content = field.getContent<VAR_NAME>();
        auto varName = content->name;
        return variableTable->contains(VAR_NAME{ varName });
    }

    return true;
}

bool PKB::validateProcedure(const PKBField field) const {
    if (field.fieldType == PKBFieldType::CONCRETE) {
        auto content = field.getContent<PROC_NAME>();
        auto varName = content->name;
        return procedureTable->contains(PROC_NAME{ varName });
    }

    return true;
}

bool PKB::validateStatement(const PKBField field) const {
    if (field.fieldType == PKBFieldType::CONCRETE) {
        auto content = field.getContent<STMT_LO>();
        auto statementNum = content->statementNum;
        auto statementType = content->type;
        auto attribute = content->attribute;
        auto stmtQuery = statementTable->getStmt(statementNum);

        if (!stmtQuery.has_value()) {
            return false;
        }

        auto stmt = stmtQuery.value();

        // If provided STMT_LO has a type, check if it is equivalent to the one in the table
        // StatementTypes of All or None will be updated to the correct one in the table by appendStatementInformation
        if (statementType.has_value() && statementType.value() != StatementType::All &&
            statementType.value() != StatementType::None) {
            return statementType == stmt.type;
        }
    }

    return true;
}

void PKB::appendStatementInformation(PKBField* field) {
    if (field->fieldType == PKBFieldType::CONCRETE && field->entityType == PKBEntityType::STATEMENT) {
        auto content = field->getContent<STMT_LO>();
        auto statementNum = content->statementNum;

        // This function is only called when validate is true, i.e. there exists a STMT_LO matching the one provided 
        // Hence, there is no need to check whether the returned optional has a value
        field->content = statementTable->getStmt(statementNum).value();
    }
}

void PKB::insertRelationship(PKBRelationship type, PKBField field1, PKBField field2) {
    // if both fields are not concrete, no insert can be done
    if (field1.fieldType != PKBFieldType::CONCRETE || field2.fieldType != PKBFieldType::CONCRETE) {
        Logger(Level::INFO) << "Both fields have to be concrete.\n";
        return;
    }

    if (!validate(field1) || !validate(field2)) {
        return;
    }

    appendStatementInformation(&field1);
    appendStatementInformation(&field2);

    getRelationshipTable(type)->insert(field1, field2);
}

/**
* Helper method to check whether the given relationship is transitive.
*
* @param relationship the type of program design abstraction
* @return bool
*/
bool isTransitiveRelationship(PKBRelationship relationship) {
    return relationship == PKBRelationship::FOLLOWST ||
        relationship == PKBRelationship::PARENTT ||
        relationship == PKBRelationship::CALLST ||
        relationship == PKBRelationship::NEXTT ||
        relationship == PKBRelationship::AFFECTST;
}

/**
* Helper method to retrieve the non-transitive counterpart of a relationship, if any. For relationships without
* any transitive counterparts, return itself.
*
* @param relationship the type of program design abstraction
* @return PKBRelationship
*/
PKBRelationship getNonTransitiveRelationship(PKBRelationship relationship) {
    if (relationship == PKBRelationship::FOLLOWST) {
        return PKBRelationship::FOLLOWS;
    } else if (relationship == PKBRelationship::PARENTT) {
        return PKBRelationship::PARENT;
    } else if (relationship == PKBRelationship::CALLST) {
        return PKBRelationship::CALLS;
    } else if (relationship == PKBRelationship::NEXTT) {
        return PKBRelationship::NEXT;
    } else if (relationship == PKBRelationship::AFFECTST) {
        return PKBRelationship::AFFECTS;
    } else {
        return relationship;
    }
}

std::shared_ptr<RelationshipTable> PKB::getRelationshipTable(PKBRelationship relationship) const {
    auto search = relationshipTables.find(getNonTransitiveRelationship(relationship));
    if (search != relationshipTables.end()) {
        return search->second;
    } else {
        Logger(Level::ERROR) << "No RelationshipTable for the given PKBRelationship";
        throw std::invalid_argument("No RelationshipTable for the given PKBRelationship");
    }
}

bool PKB::isRelationshipPresent(PKBField field1, PKBField field2, PKBRelationship rs) {
    if (field1.fieldType != PKBFieldType::CONCRETE || field2.fieldType != PKBFieldType::CONCRETE) {
        return false;
    }

    if (!validate(field1) || !validate(field2)) {
        return false;
    }

    appendStatementInformation(&field1);
    appendStatementInformation(&field2);

    this->populateAffCache(rs);

    auto relationshipTablePtr = getRelationshipTable(rs);
    if (isTransitiveRelationship(rs)) {
        if (rs == PKBRelationship::CALLST) {
            return std::dynamic_pointer_cast<TransitiveRelationshipTable<PROC_NAME>>(relationshipTablePtr)->
                containsT(field1, field2);
        } else {
            return std::dynamic_pointer_cast<TransitiveRelationshipTable<STMT_LO>>(relationshipTablePtr)->
                containsT(field1, field2);
        }
    } else {
        return relationshipTablePtr->contains(field1, field2);
    }
}

// GET API

PKBResponse PKB::getRelationship(PKBField field1, PKBField field2, PKBRelationship rs) {
    if (!validate(field1) || !validate(field2)) {
        return PKBResponse{ false, FieldRowResponse{} };
    }

    appendStatementInformation(&field1);
    appendStatementInformation(&field2);

    FieldRowResponse extracted;

    this->populateAffCache(rs);

    auto relationshipTablePtr = getRelationshipTable(rs);
    if (isTransitiveRelationship(rs)) {
        if (rs == PKBRelationship::CALLST) {
            extracted = std::dynamic_pointer_cast<TransitiveRelationshipTable<PROC_NAME>>(relationshipTablePtr)->
                retrieveT(field1, field2);
        } else {
            extracted = std::dynamic_pointer_cast<TransitiveRelationshipTable<STMT_LO>>(relationshipTablePtr)->
                retrieveT(field1, field2);
        }
    } else {
        extracted = relationshipTablePtr->retrieve(field1, field2);
    }

    return extracted.size() != 0
        ? PKBResponse{ true, Response{extracted} }
    : PKBResponse{ false, Response{extracted} };
}

/**
* Helper method to convert a vector of PKBDataTypes into a PKBResponse.
*
* @param extracted a vector of PKBDataTypes (STMT_LO, VAR_NAME, PROC_NAME, or CONST)
* @return PKBResponse
*/
template <typename T>
PKBResponse createResponseFromTable(std::vector<T> extracted) {
    std::unordered_set<PKBField, PKBFieldHash> res;
    for (auto row : extracted) {
        res.insert(PKBField::createConcrete(row));
    }

    return res.size() != 0 ? PKBResponse{ true, Response{res} } : PKBResponse{ false, Response{res} };
}

PKBResponse PKB::getStatements() {
    return createResponseFromTable<STMT_LO>(statementTable->getAllEntity());
}

PKBResponse PKB::getStatements(StatementType stmtType) {
    return createResponseFromTable<STMT_LO>(statementTable->getStmtOfType(stmtType));
}

PKBResponse PKB::getVariables() {
    return createResponseFromTable<VAR_NAME>(variableTable->getAllEntity());
}

PKBResponse PKB::getProcedures() {
    return createResponseFromTable<PROC_NAME>(procedureTable->getAllEntity());
}

PKBResponse PKB::getConstants() {
    return createResponseFromTable<CONST>(constantTable->getAllEntity());
}

PKBResponse PKB::match(StatementType type, sp::design_extractor::PatternParam lhs,
    sp::design_extractor::PatternParam rhs) const {
    
    switch (type) {
    case StatementType::Assignment:
        return match<sp::ast::Assign>(lhs, rhs);
    case StatementType::If:
        return match<sp::ast::If>(lhs, rhs);
    case StatementType::While:
        return match<sp::ast::While>(lhs, rhs);
    default:
        Logger(Level::ERROR) << "No pattern matching available for the provided statement type.";
        throw std::invalid_argument("No pattern matching available for the provided statement type.");
    }
}

template <typename T>
PKBResponse PKB::match(sp::design_extractor::PatternParam lhs, sp::design_extractor::PatternParam rhs) const {
    FieldRowResponse res;
    sp::design_extractor::MatchedNodes<T> nodes;

    if constexpr (std::is_same_v<T, sp::ast::Assign>) {
        nodes = sp::design_extractor::extractAssign(root.get(), lhs, rhs);
    } else if constexpr (std::is_same_v<T, sp::ast::If>) {
        nodes = sp::design_extractor::extractIf(root.get(), lhs);
    } else {
        nodes = sp::design_extractor::extractWhile(root.get(), lhs);
    }

    for (auto& node : nodes) {
        int statementNumber = node.get().getStmtNo();
        auto stmt = PKBField::createConcrete(STMT_LO{ statementNumber, 
            statementTable->getStmtTypeOfLine(statementNumber).value() });

        // Handles assignments
        if constexpr (std::is_same_v<T, sp::ast::Assign>) {
            auto varName = node.get().getLHS()->getVarName();
            std::vector<PKBField> stmtRes;
            stmtRes.emplace_back(stmt);
            stmtRes.emplace_back(PKBField::createConcrete(VAR_NAME{ varName }));
            res.insert(stmtRes);
        } else {
            // Handles container statements
            sp::design_extractor::VariableCollector vc;
            node.get().getCondExpr()->accept(&vc);
            auto vars = vc.getEntries();

            for (auto var : vars) {
                std::vector<PKBField> stmtRes;
                stmtRes.emplace_back(stmt);
                stmtRes.emplace_back(PKBField::createConcrete(std::get<sp::design_extractor::Entity>(var)));
                res.insert(stmtRes);
            }
        }
    }

    return PKBResponse{ nodes.size() > 0, Response{res} };
}

void PKB::clearCache() {
    relationshipTables.at(PKBRelationship::AFFECTS).reset(new AffectsRelationshipTable());
    this->isAffCacheActive = false;
}

void PKB::populateAffCache(PKBRelationship rs) {
    bool isAffectsRs = rs == PKBRelationship::AFFECTS || rs == PKBRelationship::AFFECTST;
    if (isAffectsRs && !isAffCacheActive) {
        AffectsCacher affCacher;
        CacheResults res = affCacher.evalAffects(cfgContainer);
        for (auto item : res) {
            this->insertRelationship(rs, PKBField::createConcrete(item.first), 
                PKBField::createConcrete(item.second));
        }
        this->isAffCacheActive = true;
    }
}
