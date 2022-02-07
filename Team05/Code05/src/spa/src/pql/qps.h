#pragma once

#include <string>
#include <list>

#include "pql/parser.h"
#include "pql/evaluator.h"

struct QPS {
    Parser parser;
    void evaluate(std::string, std::list<std::string>&, PKB*);
};
