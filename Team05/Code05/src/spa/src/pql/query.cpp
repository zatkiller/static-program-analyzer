#include "query.h"

void Query::addDeclaration(std::string var, DESIGN_ENTITY de) {
    if (declarations.find(var) != declarations.end())
        throw "Declaration already exists!";

    declarations.insert({var, de});
}