idf_component_register(SRCS "ble.c"
                       INCLUDE_DIRS "."
                       REQUIRES bt
                       PRIV_REQUIRES nvs_flash
)
