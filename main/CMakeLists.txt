idf_component_register(SRCS "max6675.c" "temperature_controller_main.c"
                       PRIV_REQUIRES spi_flash driver
                       INCLUDE_DIRS "")
