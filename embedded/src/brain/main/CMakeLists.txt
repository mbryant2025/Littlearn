idf_component_register(SRCS "main.cpp"
                        INCLUDE_DIRS "../include" "../src"
)

target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++11)