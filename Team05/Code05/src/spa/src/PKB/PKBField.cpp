#pragma once

#include "PKBField.h"
#include "PKBDataTypes.h"

struct PKBField {
	public:
		PKBField() {}

		bool operator == (const PKBField& other) const {
			if (tag == other.tag && isConcrete == other.isConcrete) {
				PKBType type = tag;
				switch (type) {
				case PKBType::STATEMENT:
					return Content.stmtNum == other.Content.stmtNum;
				case PKBType::VARIABLE:
					return Content.varName == other.Content.varName;
				case PKBType::PROCEDURE:
					return Content.procName == other.Content.procName;
				case PKBType::CONST:
					return Content.constName == other.Content.constName
				default:
					break;
				}
			}
			return false;
		}
};