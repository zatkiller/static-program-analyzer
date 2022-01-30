#include "Query.h"

Query::Query {
    declarations = {};
    variable = {};
    suchthat = {};
    pattern = {};
};

std::unordered_map<std::string, DesignEntity> Query::getDeclarations() {
    return this->declarations;
}

std::vector<std::string> Query::getVariable() {
    return this->variable;
}

std::vector<RelRef> Query::getSuchthat() {
    return this->suchthat;
}

std::vector<Pattern> Query::getPattern() {
    return this->pattern;
}

void Query::setDeclarations(std::unordered_map<std::string, DesignEntity> declarations) {
    this->declarations = declarations;
}

void Query::setVariable(std::vector<std::string> variable) {
    this->variable = variable;
}

void Query::setSuchthat(std::vector<RelRef> suchthat) {
    this->suchthat = suchthat;
}

void Query::setPattern(std::vector<Pattern> pattern) {
    this->pattern = pattern;
}