#include "ConstantTable.h"

bool ConstantTable::contains(PKBField entry) {
    if (entry.entityType != PKBEntityType::CONST) {
        throw "Only constants are accepted!";
    }
    CONST constant = std::get<CONST>(entry.content);
    return rows.count(ConstantRow(constant)) == 1;
}

void ConstantTable::insert(PKBField entry) {
    if (entry.entityType != PKBEntityType::CONST) {
        throw "Only constants are accepted!";
    }
    CONST constant = std::get<CONST>(entry.content);
    rows.insert(ConstantRow(constant));
}

int ConstantTable::getSize() {
    return rows.size();
}

std::vector<CONST> ConstantTable::getAllConst() {
    std::vector<CONST> res;

    for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
        res.push_back(iter->getConst());
    }

    return res;
}
