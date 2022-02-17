#include "ConstantTable.h"

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
