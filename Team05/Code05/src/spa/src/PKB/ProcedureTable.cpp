#include <stdio.h>
#include <set>

#include "PKBField.h"
#include "ProcedureTable.h"

// count of an item in a set can only be 0 or 1
bool ProcedureTable::contains(PKBField field) {
    if (field.entityType != PKBEntityType::PROCEDURE) {
        throw "Only procedures are accepted!";
    }
    PROC_NAME proc = std::get<PROC_NAME>(field.content);
    return rows.count(ProcedureRow(proc)) == 1;
}

void ProcedureTable::insert(PKBField field) {
    if (field.entityType != PKBEntityType::PROCEDURE) {
        throw "Only procedures are accepted!";
    }
    PROC_NAME proc = std::get<PROC_NAME>(field.content);
    rows.insert(ProcedureRow(proc));
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
