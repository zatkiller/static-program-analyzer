#include "QueryObject.h"

QueryObject::QueryObject {
    declarations = {};
    select = {};
    suchthat = {};
    pattern = {};
};

std::unordered_map<std::string, DesignEntity> QueryObject::getDeclarations() {
    return this->declarations;
}

std::vector<std::string> QueryObject::getSelect() {
    return this->select;
}

std::vector<RelRef> QueryObject::getSuchthat() {
    return this->suchthat;
}

std::vector<Pattern> QueryObject::getPattern() {
    return this->pattern;
}

void QueryObject::setDeclarations(std::unordered_map<std::string, DesignEntity> declarations) {
    this->declarations = declarations;
}

void QueryObject::setSelect(std::vector<std::string> select) {
    this->select = select;
}

void QueryObject::setSuchthat(std::vector<RelRef> suchthat) {
    this->suchthat = suchthat;
}

void QueryObject::setPattern(std::vector<Pattern> pattern) {
    this->pattern = pattern;
}