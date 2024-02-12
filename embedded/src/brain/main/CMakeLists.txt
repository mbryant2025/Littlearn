idf_component_register(SRCS "main.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES freertos esp_event nvs_flash esp_wifi
)

target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++11)