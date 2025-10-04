/*
 * MAX6675 Thermocouple-to-Digital Converter Driver
 * 
 * This driver provides functions to read temperature from MAX6675 sensor
 * via SPI communication.
 */

#ifndef MAX6675_H
#define MAX6675_H

#include "esp_err.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

// MAX6675 Configuration
#define MAX6675_SPI_HOST    SPI2_HOST
#define MAX6675_CLOCK_SPEED 1000000  // 1 MHz (MAX6675 max is 4.3 MHz)

// GPIO Pins Configuration (adjust according to your hardware)
#define MAX6675_MISO_PIN    GPIO_NUM_19  // Master In, Slave Out
#define MAX6675_MOSI_PIN    GPIO_NUM_23  // Master Out, Slave In  
#define MAX6675_CLK_PIN     GPIO_NUM_18  // Clock
#define MAX6675_CS_PIN      GPIO_NUM_5   // Chip Select

// MAX6675 Data Structure
typedef struct {
    spi_device_handle_t spi_device;
    gpio_num_t cs_pin;
    bool initialized;
} max6675_handle_t;

// Function prototypes
esp_err_t max6675_init(max6675_handle_t *handle);
esp_err_t max6675_read_temperature(max6675_handle_t *handle, float *temperature);
esp_err_t max6675_deinit(max6675_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif // MAX6675_H
