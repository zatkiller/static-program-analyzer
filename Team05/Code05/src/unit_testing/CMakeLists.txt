file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_executable(unit_testing ${srcs} "src/pql/test_lexer.cpp" src/pql/test_parser.cpp src/pql/test_query.cpp)

target_link_libraries(unit_testing spa)