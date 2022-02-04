#include <algorithm>

#include "test_query.h"

std::unordered_map<std::string, DesignEntity> designEntityMap = {
        { "stmt", DesignEntity::STMT },
        { "read", DesignEntity::READ } ,
        { "print", DesignEntity::PRINT },
        { "while", DesignEntity::WHILE },
        { "if", DesignEntity::IF },
        { "assign", DesignEntity::ASSIGN },
        { "variable", DesignEntity::VARIABLE },
        { "constant", DesignEntity::CONSTANT },
        { "procedure", DesignEntity::PROCEDURE }
};

bool Query::hasDeclaration(std::string name) {
    return declarations.count(name) > 0;
}

bool Query::hasVariable(std::string var) {
    return std::find(variable.begin(), variable.end(), var) != variable.end();
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

std::vector<std::shared_ptr<RelRef>> Query::getSuchthat() {
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


void Query::addVariable(std::string var) {
    variable.push_back(var);
}

void Query::addSuchthat(std::shared_ptr<RelRef> rel) {
    suchthat.push_back(rel);
}

void Query::addPattern(Pattern p) {
    pattern.push_back(p);
}


EntRef EntRef::ofVarName(std::string name) {
    EntRef e;
    e.variable = name;
    e.type = EntRefType::VARIABLE_NAME;
    return e;
}

EntRef EntRef::ofDeclaration(std::string d) {
    EntRef e;
    e.declaration = d;
    e.type = EntRefType::DECLARATION;
    return e;
}

EntRef EntRef::ofWildcard() {
    return EntRef { EntRefType::WILDCARD };
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

StmtRef StmtRef::ofDeclaration(std::string d) {
    StmtRef s;
    s.type = StmtRefType::DECLARATION;
    s.declaration = d;
    return s;
}

StmtRef StmtRef::ofLineNo(int lineNo) {
    StmtRef s;
    s.type = StmtRefType::LINE_NO;
    s.lineNo = lineNo;
    return s;
}

StmtRef StmtRef::ofWildcard() {
    return  StmtRef { StmtRefType::WILDCARD };
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


std::string Pattern::getSynonym() {
    return synonym;
}

EntRef Pattern::getEntRef() {
    return lhs;
}

std::string Pattern::getExpression() {
    return expression;
}
