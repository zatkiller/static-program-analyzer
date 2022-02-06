#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <set>
#include <algorithm>
#include <iterator>
#include <memory>
#include <variant>
#include <map>

#include "pql/query.h"
#include "PKB/PKBField.h"
#include "PKB/PKBDataTypes.h"
#include "PKB/PKBType.h"
#include "PKB/StatementType.h"
#include "PKB/PKBResponse.h"
#include "PKB.h"
#include "PKB/PKBRelationship.h"


PKBResponse getAll(DesignEntity);

std::string PKBFieldToString(PKBField);

std::string processResult(PKBResponse);

std::string evaluate(Query);
