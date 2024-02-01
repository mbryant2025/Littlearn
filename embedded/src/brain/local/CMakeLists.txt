cmake_minimum_required(VERSION 3.12)
project(Brain)

set(CMAKE_CXX_STANDARD 17)

# Add debug flags if in debug mode
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Add release flags for optimization if needed
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s") # -s for strip symbols

# Specify the source files
file(GLOB_RECURSE IMPLEMENTATION_FILES "${CMAKE_SOURCE_DIR}/../interpreter/src/*.cpp")
list(APPEND IMPLEMENTATION_FILES "main.cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/../interpreter/include/*.hpp")

message(STATUS "Source files: ${IMPLEMENTATION_FILES}")
message(STATUS "Header files: ${HEADER_FILES}")

# Add the executable target
add_executable(Brain ${IMPLEMENTATION_FILES} ${HEADER_FILES})

# Include the 'include' directory for header files
target_include_directories(Brain PRIVATE "${CMAKE_SOURCE_DIR}/../interpreter/include")
