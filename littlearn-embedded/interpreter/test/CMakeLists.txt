cmake_minimum_required(VERSION 3.12)

project(LittlearnInterpreterTests)

find_package(GTest REQUIRED)

file(GLOB_RECURSE TEST_SOURCE_FILES "*.cpp")

# Exclude the CMakeCXXCompilerId.cpp file
list(FILTER TEST_SOURCE_FILES EXCLUDE REGEX ".*CMakeCXXCompilerId.cpp$")

# Create the test executable, including run_tests.cpp
add_executable(LittlearnInterpreterTests run_tests.cpp ${TEST_SOURCE_FILES})

# Include directories and link Google Test
target_include_directories(LittlearnInterpreterTests PRIVATE
    "${CMAKE_SOURCE_DIR}/../include"
    ${GTEST_INCLUDE_DIRS}
)

# Link Google Test and pthread
target_link_libraries(LittlearnInterpreterTests gtest gtest_main pthread)

# Register the test
add_test(NAME LittlearnInterpreterTests COMMAND LittlearnInterpreterTests)
