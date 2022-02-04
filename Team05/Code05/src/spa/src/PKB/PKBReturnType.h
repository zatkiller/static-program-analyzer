#pragma once

#include <variant>
#include <vector>

#include "PKBType.h"

using ReturnType = std::variant<PKBType, std::vector<PKBType>>;

struct PKBReturnType {
	ReturnType ret;
};