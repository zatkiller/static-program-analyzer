#pragma once

#include <set>
#include <stdio.h>

#include "PKBField.h"

struct PKBResponse {
    union Response {
        std::set<PKBField> content;
    };
} PKBResponse;
