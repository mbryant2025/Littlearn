idf_component_register(SRCS "radio.c"
                       INCLUDE_DIRS "."
                       REQUIRES "esp_wifi"
                    #    PRIV_REQUIRES "esp_driver_uart"
)
