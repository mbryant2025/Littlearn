cmake_minimum_required(VERSION 3.12)
project(LittlearnInterpreter)

set(CMAKE_CXX_STANDARD 17)

# Add debug flags if in debug mode
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Add release flags for optimization
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s") # -s for strip symbols


# Specify the source files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "include/*.hpp")

# Add the executable target
add_executable(LittlearnInterpreter ${SOURCE_FILES} ${HEADER_FILES})

# Include the 'include' directory for header files
target_include_directories(LittlearnInterpreter PRIVATE include)
