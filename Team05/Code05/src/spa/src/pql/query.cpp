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

void EntRef::setType(EntRefType entType) {
    type = entType;
}

void EntRef::setDeclaration(std::string d) {
    declaration = d;
}

void EntRef::setVariableName(std::string name) {
    variable = name;
}

EntRefType EntRef::getType() {
    return type;
}

std::string EntRef::getDeclaration() {
    return declaration;
}

std::string EntRef::getVariableName() {
    return variable;
}

bool EntRef::isDeclaration() {
    return type == EntRefType::DECLARATION;
}

bool EntRef::isVarName() {
    return type == EntRefType::VARIABLE_NAME;
}

bool EntRef::isWildcard() {
    return type == EntRefType::WILDCARD;
}

void StmtRef::setType(StmtRefType type) {
    type = type;
}

void StmtRef::setDeclaration(std::string d) {
    declaration = d;
}

void StmtRef::setLineNo(int lineNo) {
    lineNo = lineNo;
}

StmtRefType StmtRef::getType() {
    return type;
}

std::string StmtRef::getDeclaration() {
    return declaration;
}

int StmtRef::getLineNo() {
    return lineNo;
}

bool StmtRef::isDeclaration() {
    return type == StmtRefType::DECLARATION;
}

bool StmtRef::isLineNo() {
    return type == StmtRefType::LINE_NO;
}

bool StmtRef::isWildcard() {
    return type == StmtRefType::WILDCARD;
}

void Pattern::setSynonym(std::string s) {
    synonym = s;
}

void Pattern::setLhs(EntRef entityRef) {
    lhs = entityRef;
}

void Pattern::setExpression(std::string expression) {
    expression = expression;
}

std::string Pattern::getSynonym() {
    return synonym;
}

EntRef Pattern::getEntRef() {
    return lhs;
}

std::string Pattern::getExpression() {
    return expression;
}