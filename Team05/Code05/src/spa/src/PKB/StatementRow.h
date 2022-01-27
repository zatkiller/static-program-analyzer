#pragma once

#include <stdio.h>
#include <vector>
#include <set>

#include "../logging.h"
#include "Row.h"
#include "StatementType.h"

class StatementRow {
public:
	StatementRow(StatementType t, int l): type(t), lineNumber(l) {}

	// TODO: replace <int> withh <PKBField>
	bool contains(std::vector<int> queries) {
		// Check if the number of queries matches the type of row. queries should have only 1 element
		if (queries.size() == 0) {
			Logger(Level::ERROR) << "StatementRow.contains() received an empty query.";
			return false;
		}
		else if (queries.size() > 1) {
			Logger(Level::ERROR) << "StatementRow.contains() received an invalid query.";
			return false;
		}
		else {
			// TODO: update based on PKBField implementation
			// return queries.at(0).type == type && queries.at(0).v == lineNumber;
		}
	}

	bool equal(const StatementRow& row) {
		if (type != row.type || lineNumber != row.lineNumber) {
			return false;
		}

		return true;
	}

	bool operator == (const StatementRow& row) {
		return equal(row);
	}

	bool operator < (const StatementRow& row) {
		return !equal(row);
	}

private:
	StatementType type;
	int lineNumber;
};