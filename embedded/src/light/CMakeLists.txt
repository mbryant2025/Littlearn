cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_STANDARD 11)

list(APPEND EXTRA_COMPONENT_DIRS "../radio")

include_directories(../flags)
include_directories(../tile_types)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(brain)

idf_build_set_property(COMPILE_OPTIONS "-Wno-error" APPEND)
