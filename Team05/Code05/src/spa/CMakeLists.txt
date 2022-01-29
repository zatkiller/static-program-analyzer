file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_library(spa ${srcs} ${headers} "src/PKB/Table.cpp" "src/PKB/Row.cpp" "src/PKB/Row.h" "src/PKB/Table.h" "src/PKB/StatementRow.h" "src/PKB/StatementRow.cpp" "src/PKB/StatementTable.h" "src/PKB/StatementTable.cpp" "src/PKB/VariableRow.cpp" "src/PKB/VariableRow.h" "src/PKB/VariableTable.cpp"  "src/PKB/RelationshipRow.h" "src/PKB/RelationshipRow.cpp" "src/PKB/RelationshipTable.h" "src/PKB/ModifiesRelationshipTable.cpp" "src/PKB/RelationshipTable.cpp" "src/PKB/VariableTable.h" "src/PKB/ModifiesRelationshipTable.h" "src/PKB/ProcedureTable.cpp" "src/PKB/ProcedureTable.h" "src/PKB/ProcedureRow.h" "src/PKB/ProcedureRow.cpp")
# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


