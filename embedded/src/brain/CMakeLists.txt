cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_COMPILER_IDF $ENV{IDF_PATH}/tools/cmake/idf_cmake.sh)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_C_STANDARD 11)

list(APPEND EXTRA_COMPONENT_DIRS "interpreter")
list(APPEND EXTRA_COMPONENT_DIRS "ble")
list(APPEND EXTRA_COMPONENT_DIRS "../radio")

include_directories(../flags)
include_directories(../tile_types)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(brain)

idf_build_set_property(COMPILE_OPTIONS "-Wno-error" APPEND)
