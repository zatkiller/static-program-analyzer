file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_executable(unit_testing ${srcs} "src/TestModifiesRelationshipTable.cpp" "src/TestRelationshipRow.cpp" "src/TestProcedureTable.cpp" "src/TestStatementRow.cpp" "src/TestStatementTable.cpp" "src/TestVariableRow.cpp" "src/TestVariableTable.cpp" "src/TestConstantRow.cpp" "src/TestConstantTable.cpp")

target_link_libraries(unit_testing spa)

