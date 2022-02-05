#pragma once

#include <set>
#include <stdio.h>
#include <vector>

#include "PKBField.h"

struct PKBResponse {
    union Response {
        std::set<PKBField> content;
        std::set<std::vector<PKBField>> contentList;
    };
} PKBResponse;
