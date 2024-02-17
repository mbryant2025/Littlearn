cmake_minimum_required(VERSION 3.16)
# set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# list(APPEND EXTRA_COMPONENT_DIRS "interpreter")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(main)

idf_build_set_property(COMPILE_OPTIONS "-Wno-error" APPEND)
