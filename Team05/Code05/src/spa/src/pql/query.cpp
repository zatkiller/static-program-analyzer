#include "query.h"

bool Query::hasDeclaration(std::string name) {
    return declarations.count(name) > 0;
}

DesignEntity Query::getDeclarationDesignEntity(std::string name) {
    return declarations.find(name)->second;
}


std::unordered_map<std::string, DesignEntity> Query::getDeclarations() {
    return declarations;
}

std::vector<std::string> Query::getVariable() {
    return variable;
}

std::vector<RelRef> Query::getSuchthat() {
    return suchthat;
}

std::vector<Pattern> Query::getPattern() {
    return pattern;
}

void Query::addDeclaration(std::string var, DesignEntity de) {
    if (declarations.find(var) != declarations.end())
        throw "Declaration already exists!";

    declarations.insert({var, de});
}

void Query::setDeclarations(std::unordered_map<std::string, DesignEntity> declarations) {
    declarations = declarations;
}

void Query::setVariable(std::vector<std::string> variable) {
    variable = variable;
}

void Query::setSuchthat(std::vector<RelRef> suchthat) {
    suchthat = suchthat;
}

void Query::setPattern(std::vector<Pattern> pattern) {
    pattern = pattern;
}