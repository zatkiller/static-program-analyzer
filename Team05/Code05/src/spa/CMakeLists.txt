file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
file(GLOB_RECURSE pql ${CMAKE_CURRENT_SOURCE_DIR}/src/pql "*.cpp" "*.h")
add_library(spa ${srcs} ${headers} ${pql} src/pql/query.cpp src/pql/query.h src/pql/parser.cpp src/pql/parser.h)

# this makes the headers accessible for other projects which uses spa lib
target_include_directories(spa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)