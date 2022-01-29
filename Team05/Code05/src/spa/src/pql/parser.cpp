
#include "logging.h"
#include "parser.h"


void Parser::parsePQL(std::string query) {
    query = "//assign a; while w;\n //Select a such that Follows (w, a) pattern a (\"x\", _)";
    Logger() << "Parsing query: " << query;
}