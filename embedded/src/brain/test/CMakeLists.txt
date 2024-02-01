cmake_minimum_required(VERSION 3.12)

project(BrainTests)

find_package(GTest REQUIRED)

file(GLOB_RECURSE TEST_SOURCE_FILES "*.cpp")

# Exclude the CMakeCXXCompilerId.cpp file
list(FILTER TEST_SOURCE_FILES EXCLUDE REGEX ".*CMakeCXXCompilerId.cpp$")

# Include the implementation files, excluding main.cpp
file(GLOB_RECURSE IMPLEMENTATION_FILES "${CMAKE_SOURCE_DIR}/../interpreter/src/*.cpp")

# Create the test executable, including run_tests.cpp and implementation files
add_executable(BrainTests run_tests.cpp ${TEST_SOURCE_FILES} ${IMPLEMENTATION_FILES})

# Include directories and link Google Test
target_include_directories(BrainTests PRIVATE
    "${CMAKE_SOURCE_DIR}/../interpreter/include"
    ${GTEST_INCLUDE_DIRS}
)

# Link Google Test and pthread
target_link_libraries(BrainTests gtest gtest_main pthread)

# Register the test
add_test(NAME BrainTests COMMAND BrainTests)
