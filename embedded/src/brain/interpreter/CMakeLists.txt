file(GLOB SOURCES "src/*.cpp")

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS "include")

target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++11)