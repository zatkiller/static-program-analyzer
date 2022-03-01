#include "PKBTables.h"
#include "logging.h"

/** =============================== CONSTANTROW METHODS ================================ */
bool ConstantRow::operator == (const ConstantRow& other) const {
    return constant == other.constant;
}

bool ConstantRow::operator < (const ConstantRow& other) const {
    return std::tie(constant) < std::tie(other.constant);
}

CONST ConstantRow::getConst() const {
    return constant;
}

size_t ConstantRowHash::operator() (const ConstantRow& other) const {
    return std::hash<CONST>()(other.getConst());
}

/** ============================== CONSTANTTABLE METHODS =============================== */

bool ConstantTable::contains(CONST constant) const {
    return rows.count(ConstantRow(constant)) == 1;
}

void ConstantTable::insert(CONST constant) {
    rows.insert(ConstantRow(constant));
}

int ConstantTable::getSize() const {
    return rows.size();
}

std::vector<CONST> ConstantTable::getAllConst() const {
    std::vector<CONST> res;

    for (const auto& row : rows) {
        res.push_back(row.getConst());
    }

    return res;
}

/** ============================= PROCEDUREROW METHODS ================================ */

ProcedureRow::ProcedureRow(PROC_NAME p) : procedureName(p) {}

bool ProcedureRow::operator == (const ProcedureRow& row) const {
    return procedureName == row.procedureName;
}

bool ProcedureRow::operator < (const ProcedureRow& other) const {
    return std::tie(procedureName) < std::tie(other.procedureName);
}

PROC_NAME ProcedureRow::getProcName() const {
    return procedureName;
}

size_t ProcedureRowHash::operator() (const ProcedureRow& other) const {
    return std::hash<std::string>()(other.getProcName().name);
}

/** ============================ PROCEDURETABLE METHODS =============================== */

bool ProcedureTable::contains(std::string name) {
    return rows.count(ProcedureRow(PROC_NAME{ name })) == 1;
}

void ProcedureTable::insert(std::string name) {
    rows.insert(ProcedureRow(PROC_NAME{ name }));
}

int ProcedureTable::getSize() {
    return rows.size();
}

std::vector<PROC_NAME> ProcedureTable::getAllProcs() {
    std::vector<PROC_NAME> res;

    for (const auto& row : rows) {
        res.push_back(row.getProcName());
    }

    return res;
}

/** ============================== STATEMENTROW METHODS =============================== */

StatementRow::StatementRow(StatementType t, int stmtNum) : stmt{ stmtNum, t } {}

bool StatementRow::operator == (const StatementRow& row) const {
    return stmt == row.stmt;
}

bool StatementRow::operator < (const StatementRow& other) const {
    return std::tie(stmt) < std::tie(other.stmt);
}

STMT_LO StatementRow::getStmt() const {
    return stmt;
}

size_t StatementRowHash::operator() (const StatementRow& other) const {
    return std::hash<int>()(other.getStmt().statementNum);
}

/** ============================= STATEMENTTABLE METHODS ============================== */

bool StatementTable::contains(StatementType type, int statementNumber) {
    return rows.count(StatementRow(type, statementNumber)) == 1;
}

bool StatementTable::contains(int statementNumber) {
    return contains(StatementType::Assignment, statementNumber) ||
        contains(StatementType::Call, statementNumber) ||
        contains(StatementType::If, statementNumber) ||
        contains(StatementType::Read, statementNumber) ||
        contains(StatementType::Print, statementNumber) ||
        contains(StatementType::While, statementNumber);
}

void StatementTable::insert(StatementType type, int statementNumber) {
    if (!contains(statementNumber)) {
        rows.insert(StatementRow(type, statementNumber));
    }
}

int StatementTable::getSize() {
    return rows.size();
}

std::vector<STMT_LO> StatementTable::getAllStmt() {
    std::vector<STMT_LO> res;

    for (const auto& row : rows) {
        res.push_back(row.getStmt());
    }

    return res;
}

std::vector<STMT_LO> StatementTable::getStmtOfType(StatementType type) {
    std::vector<STMT_LO> res;

    for (const auto& row : rows) {
        if (row.getStmt().type.value() == type) {
            res.push_back(row.getStmt());
        }
    }

    return res;
}

std::optional<StatementType> StatementTable::getStmtTypeOfLine(int statementNum) {
    std::vector<StatementRow> filtered;
    std::copy_if(begin(rows), end(rows), std::back_inserter(filtered),
        [statementNum](StatementRow row) { return row.getStmt().statementNum == statementNum; });

    if (filtered.size() == 0) {
        Logger(Level::ERROR) << "No statement exists with the provided statement number";
        return std::nullopt;
    } else if (filtered.size() > 1) {
        Logger(Level::ERROR) << "Statement table has rows with duplicate line numbers";
        return std::nullopt;
    } else {
        STMT_LO stmt = filtered.at(0).getStmt();

        if (stmt.type.has_value()) {
            return stmt.type.value();
        } else {
            Logger(Level::ERROR) << "Statement does not have a type";
            return std::nullopt;
        }
    }
}

/** =============================== VARIABLEROW METHODS ================================ */

VariableRow::VariableRow(VAR_NAME v) : variableName(v) {}

bool VariableRow::operator == (const VariableRow& row) const {
    return variableName == row.variableName;
}

bool VariableRow::operator < (const VariableRow& other) const {
    return std::tie(variableName) < std::tie(other.variableName);
}

VAR_NAME VariableRow::getVarName() const {
    return variableName;
}

size_t VariableRowHash::operator()(const VariableRow& varRow) const {
    std::string varName = varRow.getVarName().name;
    return std::hash<std::string>()(varName);
}

/** ============================== VARIABLETABLE METHODS =============================== */

bool VariableTable::contains(std::string name) {
    return rows.count(VariableRow(VAR_NAME{ name })) == 1;
}

void VariableTable::insert(std::string name) {
    rows.insert(VariableRow(VAR_NAME{ name }));
}

int VariableTable::getSize() {
    return rows.size();
}

std::vector<VAR_NAME> VariableTable::getAllVars() {
    std::vector<VAR_NAME> res;

    for (const auto& row : rows) {
        res.push_back(row.getVarName());
    }

    return res;
}

/** ============================= RELATIONSHIPROW METHODS ============================== */

RelationshipRow::RelationshipRow(PKBField e1, PKBField e2) : field1(e1), field2(e2) {}

bool RelationshipRow::operator == (const RelationshipRow& row) const {
    return field1 == row.field1 && field2 == row.field2;
}

PKBField RelationshipRow::getFirst() const {
    return field1;
}

PKBField RelationshipRow::getSecond() const {
    return field2;
}

size_t RelationshipRowHash::operator() (const RelationshipRow& other) const {
    PKBField ent1 = other.getFirst();
    PKBField ent2 = other.getSecond();

    PKBEntityType entityType1 = ent1.entityType;
    PKBEntityType entityType2 = ent2.entityType;

    return (std::hash<PKBEntityType>()(entityType1) ^ std::hash<PKBEntityType>()(entityType2));
}

/** ============================ RELATIONSHIPTABLE METHODS ============================= */

RelationshipTable::RelationshipTable(PKBRelationship rsType) : type(rsType) {}

PKBRelationship RelationshipTable::getType() {
    return type;
}

/** ===================== NONTRANSITIVERELATIONSHIPTABLE METHODS ======================= */

NonTransitiveRelationshipTable::NonTransitiveRelationshipTable(PKBRelationship rsType) : RelationshipTable(rsType) {}

bool NonTransitiveRelationshipTable::isRetrieveValid(PKBField field1, PKBField field2) {
    // both Modifies and Uses cannot accept a wildcard as its first parameter
    return field1.fieldType != PKBFieldType::WILDCARD &&
        (field1.entityType == PKBEntityType::PROCEDURE ||
            field1.entityType == PKBEntityType::STATEMENT) &&
        (field2.entityType == PKBEntityType::VARIABLE);
}

bool NonTransitiveRelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
    return (field1.isValidConcrete(PKBEntityType::STATEMENT) || field1.isValidConcrete(PKBEntityType::PROCEDURE))
        && field2.isValidConcrete(PKBEntityType::VARIABLE);
}

bool NonTransitiveRelationshipTable::contains(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "RelationshipTable can only contain concrete fields and STATEMENT or PROCEDURE entity types.";
        return false;
    };

    return rows.count(RelationshipRow(field1, field2)) == 1;
}

void NonTransitiveRelationshipTable::insert(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "RelationshipTable only allow inserts of concrete fields and STATEMENT or PROCEDURE entity types.";
        return;
    }

    rows.insert(RelationshipRow(field1, field2));
}

FieldRowResponse NonTransitiveRelationshipTable::retrieve(PKBField field1, PKBField field2) {
    PKBFieldType fieldType1 = field1.fieldType;
    PKBFieldType fieldType2 = field2.fieldType;

    FieldRowResponse res;

    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "Only fields of STATEMENT or PROCEDURE entity types can be retrieved from RelationshipTable.";
        return res;
    }

    if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
        if (this->contains(field1, field2)) res.insert({ field1, field2 });

    } else if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 != PKBFieldType::CONCRETE) {
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            if (first == field1) {
                res.insert({ first,second });
            }
        }
        // either statement declaration or wildcard, or procedure wildcard/declaration
    } else if (fieldType1 != PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
        // field1 cannot be both statement and wildcard, invalid syntax
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            // If first declaration looking for Procedures
            if (field1.entityType == PKBEntityType::PROCEDURE && first.entityType == PKBEntityType::PROCEDURE) {
                if (second == field2) {
                    res.insert({ first,second });
                }
            }

            if (field1.entityType == PKBEntityType::STATEMENT && first.entityType == PKBEntityType::STATEMENT) {
                if (second == field2) {
                    if (field1.statementType.value() == StatementType::All || field1.statementType == first.getContent<STMT_LO>()->type.value()) {
                        res.insert({ first,second });
                    }
                }
            }
        }
    } else { // both declaration/wild
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            // If first declaration looking for Procedures
            if (field1.entityType == PKBEntityType::PROCEDURE && first.entityType == PKBEntityType::PROCEDURE) {
                res.insert({ first,second });
            }

            if (field1.entityType == PKBEntityType::STATEMENT && first.entityType == PKBEntityType::STATEMENT) {
                if (field1.statementType.value() == StatementType::All || field1.statementType == first.getContent<STMT_LO>()->type.value()) {
                    res.insert({ first,second });
                }
            }
        }
    }

    return res;
}

int NonTransitiveRelationshipTable::getSize() {
    return rows.size();
}

/** ============================== FOLLOWSGRAPH METHODS =============================== */

void FollowsGraph::addEdge(STMT_LO u, STMT_LO v) {
    FollowsNode* uNode;
    FollowsNode* vNode;

    if (u.statementNum == v.statementNum) return;
    if (u.statementNum > v.statementNum) return;

    // filters nodes with matching statement numbers as the inputs
    std::map<STMT_LO, FollowsNode*> filtered;
    std::copy_if(nodes.begin(), nodes.end(), std::inserter(filtered, filtered.end()),
        [u, v](decltype(nodes)::value_type const& pair) {
            return pair.first.statementNum == u.statementNum || pair.first.statementNum == v.statementNum;
        });

    bool hasExistingStatementNumber = false;
    for (auto [stmt, node] : filtered) {
        if (stmt == u) {
            uNode = nodes.at(u);
            hasExistingStatementNumber = true;
            break;
        }

        if (stmt.statementNum == u.statementNum) {
            if (stmt.type.value() != u.type.value()) {
                return;
            }
        }
    }

    // if there exists a node with the same statement number as u but different type,
    // it is an invalid insert. 
    if (!hasExistingStatementNumber) {
        uNode = new FollowsNode(u, nullptr, nullptr);
        nodes.emplace(u, uNode);
    }

    hasExistingStatementNumber = false;
    for (auto [stmt, node] : filtered) {
        if (stmt == v) {
            vNode = nodes.at(v);
            hasExistingStatementNumber = true;
            break;
        }

        if (stmt.statementNum == v.statementNum) {
            if (stmt.type.value() != v.type.value()) {
                return;
            }
        }
    }

    // if there exists a node with the same statement number as u but different type,
    // it is an invalid insert. 
    if (!hasExistingStatementNumber) {
        vNode = new FollowsNode(v, nullptr, nullptr);
        nodes.emplace(v, vNode);
    }

    if (!uNode->next) uNode->next = vNode;
    if (!vNode->next) vNode->prev = uNode;
}

bool FollowsGraph::getContains(PKBField field1, PKBField field2) {
    STMT_LO stmt = *(field1.getContent<STMT_LO>());
    STMT_LO target = *(field2.getContent<STMT_LO>());
    if (nodes.count(stmt) != 0) {
        FollowsNode* curr = nodes.at(stmt);
        return curr->next && curr->next->stmt == target;
    }
    return false;
}

bool FollowsGraph::getContainsT(PKBField field1, PKBField field2) {
    STMT_LO stmt = *(field1.getContent<STMT_LO>());
    STMT_LO target = *(field2.getContent<STMT_LO>());
    if (nodes.count(stmt) != 0) {
        FollowsNode* curr = nodes.at(stmt);
        while (curr->next != nullptr) {
            if (curr->next->stmt == target) {
                return true;
            }
            curr = curr->next;
        }
    }
    return false;
}

Result FollowsGraph::retrieve(PKBField field1, PKBField field2) {
    bool isConcreteFirst = field1.fieldType == PKBFieldType::CONCRETE;
    bool isConcreteSec = field2.fieldType == PKBFieldType::CONCRETE;

    if (isConcreteFirst && isConcreteSec) {
        return this->getContains(field1, field2) ? Result{ {{field1, field2}} } : Result{};

    } else if (isConcreteFirst && !isConcreteSec) {
        STMT_LO stmt = *(field1.getContent<STMT_LO>());
        if (nodes.count(stmt) != 0) {
            FollowsNode* curr = nodes.at(stmt);

            if (curr->next != nullptr) {
                STMT_LO nextStmt = curr->next->stmt;
                StatementType type = field2.statementType.value();

                return (nextStmt.type.value() == type || type == StatementType::All)
                    ? Result{ {{ field1, PKBField::createConcrete(Content{nextStmt}) }} }
                : Result{};
            }
        }

        return Result{};

    } else if (!isConcreteFirst && isConcreteSec) {
        STMT_LO stmt = *(field2.getContent<STMT_LO>());
        if (nodes.count(stmt) != 0) {
            FollowsNode* curr = nodes.at(stmt);

            if (curr->prev != nullptr) {
                STMT_LO prevStmt = curr->prev->stmt;
                StatementType type = field1.statementType.value();

                return (prevStmt.type.value() == type || type == StatementType::All)
                    ? Result{ {{ PKBField::createConcrete(Content{prevStmt}), field2 }} }
                : Result{};
            }
        }
        return Result{};

    } else {
        return traverseAll(field1.statementType.value(), field2.statementType.value());
    }
}

Result FollowsGraph::retrieveT(PKBField field1, PKBField field2) {
    if (field1.fieldType == PKBFieldType::CONCRETE && field2.fieldType == PKBFieldType::DECLARATION) {
        return traverseStartT(field1, field2);
    } else if (field1.fieldType == PKBFieldType::DECLARATION && field2.fieldType == PKBFieldType::CONCRETE) {
        return traverseEndT(field1, field2);
    } else if (field1.fieldType == PKBFieldType::DECLARATION && field2.fieldType == PKBFieldType::DECLARATION) {
        return traverseAllT(field1.statementType.value(), field2.statementType.value());
    } else {
        return Result{};
    }
}

Result FollowsGraph::traverseStartT(PKBField field1, PKBField field2) {
    std::set<STMT_LO> stmtSet;
    Result res;
    StatementType type = field2.statementType.value();
    STMT_LO s = *field1.getContent<STMT_LO>();

    if (nodes.count(s) != 0) {
        traverseStartT(&stmtSet, nodes.at(s));

        for (auto stmt : stmtSet) {
            if (stmt.type.value() == type || type == StatementType::All) {
                res.insert(std::vector<PKBField>{field1, PKBField::createConcrete(stmt)});
            }
        }
    }

    return res;
}

void FollowsGraph::traverseStartT(std::set<STMT_LO>* stmtSetPtr, FollowsNode* node) {
    while (node->next) {
        stmtSetPtr->insert(node->next->stmt);
        node = node->next;
    }
}

Result FollowsGraph::traverseEndT(PKBField field1, PKBField field2) {
    std::set<STMT_LO> stmtSet;
    Result res;
    StatementType type = field1.statementType.value();
    STMT_LO s = *field2.getContent<STMT_LO>();

    if (nodes.count(s) != 0) {
        traverseEndT(&stmtSet, nodes.at(s));

        for (auto stmt : stmtSet) {
            if (stmt.type.value() == type || type == StatementType::All) {
                res.insert(std::vector<PKBField>{PKBField::createConcrete(stmt), field2});
            }
        }
    }

    return res;
}

void FollowsGraph::traverseEndT(std::set<STMT_LO>* stmtSetPtr, FollowsNode* node) {
    while (node->prev) {
        stmtSetPtr->insert(node->prev->stmt);
        node = node->prev;
    }
}

Result FollowsGraph::traverseAll(StatementType type1, StatementType type2) {
    Result res;
    for (auto const& [stmt_lo, node] : nodes) {
        std::vector<PKBField> temp;
        FollowsNode* curr = node;
        if (curr->next != nullptr) {
            STMT_LO currStmt = curr->stmt;
            STMT_LO nextStmt = curr->next->stmt;
            bool typeMatch1 = currStmt.type.value() == type1 || type1 == StatementType::All;
            bool typeMatch2 = nextStmt.type.value() == type2 || type2 == StatementType::All;
            if (typeMatch1 && typeMatch2) {
                temp.push_back(PKBField::createConcrete(Content{ currStmt }));
                temp.push_back(PKBField::createConcrete(Content{ nextStmt }));
                res.insert(temp);
            }
        }
    }
    return res;
}

Result FollowsGraph::traverseAllT(StatementType type1, StatementType type2) {
    std::set<STMT_LO> stmtSet;
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> res;

    for (auto const& [stmt_lo, node] : nodes) {
        stmtSet.clear();

        if ((type1 == StatementType::All || node->stmt.type.value() == type1)) {
            traverseStartT(&stmtSet, node);

            for (auto stmt : stmtSet) {
                if (type2 == StatementType::All || type2 == stmt.type.value()) {
                    res.insert(std::vector<PKBField>{PKBField::createConcrete(node->stmt),
                        PKBField::createConcrete(stmt)});
                }
            }
        }
    }

    return res;
}

int FollowsGraph::getSize() {
    return nodes.size();
}

/** =============================== PARENTGRAPH METHODS ================================ */

void ParentGraph::addEdge(STMT_LO u, STMT_LO v) {
    ParentNode* uNode;
    ParentNode* vNode;

    // Statement's parent cannot come after the statement
    if (u.statementNum >= v.statementNum) {
        return;
    }

    // First stmt has to be a container statement
    if (u.type.value() != StatementType::If && u.type.value() != StatementType::While) {
        return;
    }

    // Filter nodes whose statement numbers are the same as inputs
    std::map<STMT_LO, ParentNode*> filtered;
    std::copy_if(nodes.begin(), nodes.end(), std::inserter(filtered, filtered.end()),
        [u, v](decltype(nodes)::value_type const& pair) {
            return pair.first.statementNum == u.statementNum ||
                pair.first.statementNum == v.statementNum;
        });

    bool hasExistingStatementNo = false;
    for (auto [stmt, node] : filtered) {
        // Case where a node for u already exists
        if (stmt == u) {
            uNode = nodes.at(u);
            hasExistingStatementNo = true;
            break;
        }

        // Invalid insert
        if (stmt.statementNum == u.statementNum && stmt.type.value() != u.type.value()) {
            return;
        }
    }

    if (!hasExistingStatementNo) {
        uNode = new ParentNode(u, nullptr, ParentNode::NodeSet{});
        nodes.emplace(u, uNode);
    }

    hasExistingStatementNo = false;
    for (auto [stmt, node] : filtered) {
        // Case where a node for v already exists
        if (stmt == v) {
            vNode = nodes.at(v);
            hasExistingStatementNo = true;
            break;
        }

        // Invalid insert
        if (stmt.statementNum == v.statementNum && stmt.type.value() != v.type.value()) {
            return;
        }
    }

    if (!hasExistingStatementNo) {
        vNode = new ParentNode(v, nullptr, ParentNode::NodeSet{});
        nodes.emplace(v, vNode);
    }

    // Add the edge between uNode and vNode
    uNode->next.push_back(vNode);
    if (!vNode->prev) {
        vNode->prev = uNode;
    }
}

bool ParentGraph::getContains(PKBField field1, PKBField field2) {
    STMT_LO parent = *field1.getContent<STMT_LO>();
    STMT_LO child = *field2.getContent<STMT_LO>();

    if (nodes.count(parent) != 0) {
        ParentNode* curr = nodes.at(parent);
        ParentNode::NodeSet nextNodes = curr->next;
        ParentNode::NodeSet filtered;
        std::copy_if(nextNodes.begin(), nextNodes.end(),
            std::back_inserter(filtered), [child](ParentNode* const& node) {
                return node->stmt == child;
            });
        return (filtered.size() == 1);
    }
    return false;
}

bool ParentGraph::getContainsT(PKBField field1, PKBField field2) {
    STMT_LO parent = *field1.getContent<STMT_LO>();
    STMT_LO child = *field2.getContent<STMT_LO>();

    // Base Case where Parent(field1, field2) holds
    if (this->getContains(field1, field2)) {
        return true;
    }

    if (nodes.count(parent) != 0) {
        ParentNode* curr = nodes.at(parent);
        ParentNode::NodeSet nextNodes = curr->next;

        // Recursive lookup for each node in the NodeSet
        for (auto node : nextNodes) {
            STMT_LO newStmt = node->stmt;
            PKBField newField1 = PKBField::createConcrete(Content{ newStmt });
            if (this->getContainsT(newField1, field2)) {
                return true;
            }
        }
    }
    return false;
}

Result ParentGraph::retrieve(PKBField field1, PKBField field2) {
    bool isConcreteFirst = field1.fieldType == PKBFieldType::CONCRETE;
    bool isConcreteSec = field2.fieldType == PKBFieldType::CONCRETE;

    if (isConcreteFirst && isConcreteSec) {
        return this->getContains(field1, field2)
            ? Result{ {{field1, field2}} }
        : Result{};

    } else if (isConcreteFirst && !isConcreteSec) {
        STMT_LO target = *(field1.getContent<STMT_LO>());
        Result res{};

        if (!target.type.has_value() && !field1.statementType.has_value()) {
            return res;
        }

        StatementType targetType = field2.statementType.value();

        if (nodes.count(target) != 0) {
            ParentNode* curr = nodes.at(target);

            if (!curr->next.empty()) {
                ParentNode::NodeSet nextNodes = curr->next;
                ParentNode::NodeSet filtered;
                std::copy_if(nextNodes.begin(), nextNodes.end(), std::back_inserter(filtered),
                    [targetType](ParentNode* const& node) {
                        return (node->stmt.type.value() == targetType || targetType == StatementType::All);
                    });
                for (ParentNode* node : filtered) {
                    std::vector<PKBField> temp;
                    PKBField second = PKBField::createConcrete(Content{ node->stmt });
                    temp.push_back(field1);
                    temp.push_back(second);
                    res.insert(temp);
                }
            }
        }
        return res;

    } else if (!isConcreteFirst && isConcreteSec) {
        STMT_LO target = *(field2.getContent<STMT_LO>());
        Result res{};

        if (!target.type.has_value() && !field2.statementType.has_value()) {
            return res;
        }

        StatementType targetType = field1.statementType.value();

        if (nodes.count(target) != 0) {
            ParentNode* curr = nodes.at(target);

            if (curr->prev != nullptr) {
                STMT_LO prevStmt = curr->prev->stmt;
                if (prevStmt.type.value() == targetType || targetType == StatementType::All) {
                    PKBField first = PKBField::createConcrete(Content{ prevStmt });
                    res.insert(std::vector<PKBField>{first, field2});
                }
            }
        }
        return res;

    } else {
        return this->traverseAll(field1.statementType.value(), field2.statementType.value());
    }
}

Result ParentGraph::retrieveT(PKBField field1, PKBField field2) {
    bool isConcreteFirst = field1.fieldType == PKBFieldType::CONCRETE;
    bool isDeclarationFirst = field1.fieldType == PKBFieldType::DECLARATION;
    bool isConcreteSec = field2.fieldType == PKBFieldType::CONCRETE;
    bool isDeclarationSec = field2.fieldType == PKBFieldType::DECLARATION;

    if (isConcreteFirst && isDeclarationSec) {
        return traverseStartT(field1, field2);
    } else if (isDeclarationFirst && isConcreteSec) {
        return traverseEndT(field1, field2);
    } else if (isDeclarationFirst && isDeclarationSec) {
        return traverseAllT(field1.statementType.value(), field2.statementType.value());
    } else {
        return Result{};
    }
}

Result ParentGraph::traverseStartT(PKBField field1, PKBField field2) {
    std::set<STMT_LO> found;
    Result res{};
    StatementType targetType = field2.statementType.value();
    STMT_LO startStmt = *(field1.getContent<STMT_LO>());

    if (nodes.count(startStmt) != 0) {
        traverseStartT(&found, nodes.at(startStmt), targetType);
    }

    if (!found.empty()) {
        std::transform(found.begin(), found.end(), std::insert_iterator<Result>(res, res.end()),
            [field1](STMT_LO const& stmt) {
                PKBField second = PKBField::createConcrete(Content{ stmt });
                return std::vector<PKBField>{field1, second};
            });
    }

    return res;
}

void ParentGraph::traverseStartT(std::set<STMT_LO>* found, ParentNode* node, StatementType targetType) {
    ParentNode::NodeSet nextNodes = node->next;

    if (!nextNodes.empty()) {
        for (auto nextNode : nextNodes) {
            bool typeMatch = nextNode->stmt.type.value() == targetType || targetType == StatementType::All;
            if (typeMatch) {
                found->insert(nextNode->stmt);
            }
            traverseStartT(found, nextNode, targetType);
        }
    }
    return;
}

Result ParentGraph::traverseEndT(PKBField field1, PKBField field2) {
    std::set<STMT_LO> found;
    Result res{};
    StatementType targetType = field1.statementType.value();
    STMT_LO startStmt = *(field2.getContent<STMT_LO>());

    if (nodes.count(startStmt) != 0) {
        traverseEndT(&found, nodes.at(startStmt), targetType);
    }

    if (!found.empty()) {
        std::transform(found.begin(), found.end(), std::insert_iterator<Result>(res, res.end()),
            [field2](STMT_LO const& stmt) {
                PKBField first = PKBField::createConcrete(Content{ stmt });
                return std::vector<PKBField>{first, field2};
            });
    }

    return res;
}

void ParentGraph::traverseEndT(std::set<STMT_LO>* found, ParentNode* node, StatementType targetType) {
    while (node->prev) {
        bool typeMatch = node->prev->stmt.type.value() == targetType || targetType == StatementType::All;
        if (typeMatch) {
            found->insert(node->prev->stmt);
        }
        node = node->prev;
    }
}

Result ParentGraph::traverseAllT(StatementType type1, StatementType type2) {
    std::set<STMT_LO> found;
    Result res;

    for (auto const& [stmt_lo, node] : nodes) {
        ParentNode* curr = node;
        found.clear();

        bool typeMatch = type1 == StatementType::All || node->stmt.type.value() == type1;
        if (typeMatch) {
            traverseStartT(&found, node, type2);
        }

        std::transform(found.begin(), found.end(), std::insert_iterator<Result>(res, res.end()),
            [curr](STMT_LO const& stmt) {
                PKBField first = PKBField::createConcrete(Content{ curr->stmt });
                PKBField second = PKBField::createConcrete(Content{ stmt });
                return std::vector<PKBField>{first, second};
            });
    }

    return res;
}

Result ParentGraph::traverseAll(StatementType type1, StatementType type2) {
    Result res{};
    for (auto const& [stmt_lo, node] : nodes) {
        ParentNode* curr = node;
        bool typeMatch1 = curr->stmt.type.value() == type1 || type1 == StatementType::All;

        if (!curr->next.empty() && typeMatch1) {
            ParentNode::NodeSet nextNodes = curr->next;

            // Filter nodes that match second statement type
            ParentNode::NodeSet filtered;
            std::copy_if(nextNodes.begin(), nextNodes.end(), std::back_inserter(filtered),
                [type2](ParentNode* const& node) {
                    return node->stmt.type.value() == type2 || type2 == StatementType::All;
                });

            // Populate result
            std::transform(filtered.begin(), filtered.end(), std::insert_iterator<Result>(res, res.end()),
                [curr](ParentNode* const& node) {
                    PKBField first = PKBField::createConcrete(Content{ curr->stmt });
                    PKBField second = PKBField::createConcrete(Content{ node->stmt });
                    return std::vector<PKBField>{first, second};
                });
        }
    }
    return res;
}

int ParentGraph::getSize() {
    return nodes.size();
}

/** ======================= TRANSITIVERELATIONSHIPTABLE METHODS ======================== */

TransitiveRelationshipTable::TransitiveRelationshipTable(PKBRelationship rsType) : RelationshipTable(rsType) {};

/** ======================== MODIFIESRELATIONSHIPTABLE METHODS ========================= */

ModifiesRelationshipTable::ModifiesRelationshipTable() : NonTransitiveRelationshipTable{ PKBRelationship::MODIFIES } {};

/** ========================== USESRELATIONSHIPTABLE METHODS =========================== */

UsesRelationshipTable::UsesRelationshipTable() : NonTransitiveRelationshipTable{ PKBRelationship::USES } {};

/** ======================== FOLLOWSRELATIONSHIPTABLE METHODS ========================== */

FollowsRelationshipTable::FollowsRelationshipTable() : TransitiveRelationshipTable(PKBRelationship::FOLLOWS) {
    graph = std::make_unique<FollowsGraph>();
}

void FollowsRelationshipTable::insert(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "Only concrete statements can be inserted into the Follows table!";
        return;
    }

    graph->addEdge(*field1.getContent<STMT_LO>(), *field2.getContent<STMT_LO>());
}

bool FollowsRelationshipTable::contains(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "Only concrete statements can be inserted into the Follows table!";
        return false;
    }

    return graph->getContains(field1, field2);
}

bool FollowsRelationshipTable::containsT(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "FollowsRelationshipTable can only contain concrete fields and STATEMENT entity types.";
        return false;
    }

    return graph->getContainsT(field1, field2);
}

FieldRowResponse FollowsRelationshipTable::retrieve(PKBField field1, PKBField field2) {
    // Both fields have to be a statement type
    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "Only STATEMENT entity types can be retrieved from FollowsRelationshipTable.";
        return FieldRowResponse{ };
    }

    // for any fields that are wildcards, convert them into declarations of all types
    if (field1.fieldType == PKBFieldType::WILDCARD) {
        field1.fieldType = PKBFieldType::DECLARATION;
        field1.statementType = StatementType::All;
    }

    if (field2.fieldType == PKBFieldType::WILDCARD) {
        field2.fieldType = PKBFieldType::DECLARATION;
        field2.statementType = StatementType::All;
    }

    if (field1.fieldType == PKBFieldType::CONCRETE &&
        field2.fieldType == PKBFieldType::CONCRETE) {
        return this->contains(field1, field2)
            ? FieldRowResponse{ {{field1, field2}} }
        : FieldRowResponse{};
    }

    return graph->retrieve(field1, field2);
}

FieldRowResponse FollowsRelationshipTable::retrieveT(PKBField field1, PKBField field2) {
    // Both fields have to be a statement type
    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "Only STATEMENT entity types can be retrieved from FollowsRelationshipTable.";
        return FieldRowResponse{ };
    }

    // for any fields that are wildcards, convert them into declarations of all types
    if (field1.fieldType == PKBFieldType::WILDCARD) {
        field1.fieldType = PKBFieldType::DECLARATION;
        field1.statementType = StatementType::All;
    }

    if (field2.fieldType == PKBFieldType::WILDCARD) {
        field2.fieldType = PKBFieldType::DECLARATION;
        field2.statementType = StatementType::All;
    }

    if (field1.fieldType == PKBFieldType::CONCRETE &&
        field2.fieldType == PKBFieldType::CONCRETE) {
        return this->containsT(field1, field2)
            ? FieldRowResponse{ {{field1, field2}} }
        : FieldRowResponse{};
    }

    return graph->retrieveT(field1, field2);
}

bool FollowsRelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
    return field1.isValidConcrete(PKBEntityType::STATEMENT) && field2.isValidConcrete(PKBEntityType::STATEMENT);
}

bool FollowsRelationshipTable::isRetrieveValid(PKBField field1, PKBField field2) {
    return field1.entityType == PKBEntityType::STATEMENT &&
        field2.entityType == PKBEntityType::STATEMENT;
}

int FollowsRelationshipTable::getSize() {
    return graph->getSize();
}

/** ======================== PARENTRELATIONSHIPTABLE METHODS ========================== */

ParentRelationshipTable::ParentRelationshipTable() : TransitiveRelationshipTable(PKBRelationship::PARENT) {
    graph = std::make_unique<ParentGraph>();
}

bool ParentRelationshipTable::isRetrieveValid(PKBField field1, PKBField entity2) {
    return field1.entityType == PKBEntityType::STATEMENT &&
        entity2.entityType == PKBEntityType::STATEMENT;
}

bool ParentRelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
    return field1.isValidConcrete(PKBEntityType::STATEMENT) &&
        field2.isValidConcrete(PKBEntityType::STATEMENT);
}

bool ParentRelationshipTable::contains(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "Only concrete statements can be inserted into the Parent table!";
        return false;
    }

    return this->graph->getContains(field1, field2);
}

void ParentRelationshipTable::insert(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "Only concrete statements can be inserted into the Parent table!";
        return;
    }

    STMT_LO first = *(field1.getContent<STMT_LO>());
    STMT_LO second = *(field2.getContent<STMT_LO>());

    this->graph->addEdge(first, second);
}

FieldRowResponse ParentRelationshipTable::retrieve(PKBField field1, PKBField field2) {
    // Check both fields are statement types
    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "Only STATEMENT entity types can be retrieved from ParentRelationshipTable";

        return FieldRowResponse{};
    }

    // for any fields that are wildcards, convert them into declarations of all types
    if (field1.fieldType == PKBFieldType::WILDCARD) {
        field1.fieldType = PKBFieldType::DECLARATION;
        field1.statementType = StatementType::All;
    }

    if (field2.fieldType == PKBFieldType::WILDCARD) {
        field2.fieldType = PKBFieldType::DECLARATION;
        field2.statementType = StatementType::All;
    }

    if (field1.fieldType == PKBFieldType::CONCRETE &&
        field2.fieldType == PKBFieldType::CONCRETE) {
        return this->contains(field1, field2)
            ? FieldRowResponse{ {{field1, field2}} }
        : FieldRowResponse{};
    }

    return this->graph->retrieve(field1, field2);
}

bool ParentRelationshipTable::containsT(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "ParentRelationshipTable can only contain concrete fields and STATEMENT entity types.";
        return false;
    }
    return this->graph->getContainsT(field1, field2);
}

FieldRowResponse ParentRelationshipTable::retrieveT(PKBField field1, PKBField field2) {
    // Check both fields are statement types
    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "Only STATEMENT entity types can be retrieved from ParentRelationshipTable";

        return FieldRowResponse{};
    }

    // Treat all wildcards as declarations
    if (field1.fieldType == PKBFieldType::WILDCARD) {
        field1.fieldType = PKBFieldType::DECLARATION;
        field1.statementType = StatementType::All;
    }

    if (field2.fieldType == PKBFieldType::WILDCARD) {
        field2.fieldType = PKBFieldType::DECLARATION;
        field2.statementType = StatementType::All;
    }

    if (field1.fieldType == PKBFieldType::CONCRETE &&
        field2.fieldType == PKBFieldType::CONCRETE) {
        return this->containsT(field1, field2)
            ? FieldRowResponse{ {{field1, field2}} }
        : FieldRowResponse{};
    }

    return this->graph->retrieveT(field1, field2);
}

int ParentRelationshipTable::getSize() {
    return graph->getSize();
}
