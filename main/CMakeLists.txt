idf_component_register(SRCS "rest_server.c" "wifi_manager.c" "mosfet_pwm.c" "max6675.c" "temperature_controller_main.c"
                       PRIV_REQUIRES spi_flash driver esp_wifi esp_http_server nvs_flash json
                       INCLUDE_DIRS "")
