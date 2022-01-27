file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_library(spa ${srcs} ${headers} "src/PKB/Table.cpp" "src/PKB/Row.cpp" "src/PKB/Row.h" "src/PKB/Table.h"      "src/PKB/StatementRow.h" "src/PKB/StatementRow.cpp" "src/PKB/StatementTable.h" "src/PKB/StatementTable.cpp")
# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


