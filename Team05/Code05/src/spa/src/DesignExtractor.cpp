#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "PKB.h"
#include "DesignExtractor.h"
#include "logging.h"

int DesignExtractor () {
	return 0;
}


void VariableExtractor::visitRead(const AST::Read& node) {
	Logger() << "Read variable extraction";
}

void VariableExtractor::visitPrint(const AST::Print& node) {
	Logger() << "Print variable extraction";
}
