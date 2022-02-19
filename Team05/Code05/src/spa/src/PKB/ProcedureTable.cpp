#include <stdio.h>
#include <set>

#include "PKBField.h"
#include "ProcedureTable.h"

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
