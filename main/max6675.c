/*
 * MAX6675 Thermocouple-to-Digital Converter Driver Implementation
 */

#include "max6675.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MAX6675";

esp_err_t max6675_init(max6675_handle_t *handle)
{
    if (handle == NULL) {
        ESP_LOGE(TAG, "Handle is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    // Initialize SPI bus configuration
    spi_bus_config_t bus_cfg = {
        .miso_io_num = MAX6675_MISO_PIN,
        .mosi_io_num = MAX6675_MOSI_PIN,
        .sclk_io_num = MAX6675_CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };

    // Initialize SPI bus
    esp_err_t ret = spi_bus_initialize(MAX6675_SPI_HOST, &bus_cfg, SPI_DMA_DISABLED);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return ret;
    }

    // Configure SPI device
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = MAX6675_CLOCK_SPEED,
        .mode = 0,  // SPI mode 0 (CPOL=0, CPHA=0)
        .spics_io_num = MAX6675_CS_PIN,
        .queue_size = 1,
        .flags = SPI_DEVICE_NO_DUMMY,  // MAX6675 doesn't need dummy bytes
    };

    // Add SPI device
    ret = spi_bus_add_device(MAX6675_SPI_HOST, &dev_cfg, &handle->spi_device);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add SPI device: %s", esp_err_to_name(ret));
        spi_bus_free(MAX6675_SPI_HOST);
        return ret;
    }

    handle->cs_pin = MAX6675_CS_PIN;
    handle->initialized = true;

    ESP_LOGI(TAG, "MAX6675 initialized successfully");
    ESP_LOGI(TAG, "SPI Host: %d, CS Pin: %d, Clock: %d Hz", 
             MAX6675_SPI_HOST, MAX6675_CS_PIN, MAX6675_CLOCK_SPEED);

    return ESP_OK;
}

esp_err_t max6675_read_temperature(max6675_handle_t *handle, float *temperature)
{
    if (handle == NULL || temperature == NULL) {
        ESP_LOGE(TAG, "Invalid parameters");
        return ESP_ERR_INVALID_ARG;
    }

    if (!handle->initialized) {
        ESP_LOGE(TAG, "MAX6675 not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // MAX6675 sends 16 bits of data
    uint8_t rx_data[2] = {0};
    uint8_t tx_data[2] = {0};  // MAX6675 only needs CS to be pulled low

    spi_transaction_t trans = {
        .length = 16,  // 16 bits
        .tx_buffer = tx_data,
        .rx_buffer = rx_data,
    };

    esp_err_t ret = spi_device_transmit(handle->spi_device, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI transaction failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // Combine the two bytes (MSB first)
    uint16_t raw_data = (rx_data[0] << 8) | rx_data[1];

    // Check for thermocouple connection (bit 2)
    if (raw_data & 0x04) {
        ESP_LOGW(TAG, "Thermocouple not connected");
        return ESP_ERR_INVALID_RESPONSE;
    }

    // Extract temperature data (bits 15-3)
    // Temperature is in 0.25°C increments
    int16_t temp_raw = (raw_data >> 3) & 0x0FFF;
    
    // Convert to Celsius
    *temperature = temp_raw * 0.25f;

    ESP_LOGD(TAG, "Raw data: 0x%04X, Temperature: %.2f°C", raw_data, *temperature);

    return ESP_OK;
}

esp_err_t max6675_deinit(max6675_handle_t *handle)
{
    if (handle == NULL) {
        ESP_LOGE(TAG, "Handle is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (!handle->initialized) {
        ESP_LOGW(TAG, "MAX6675 not initialized");
        return ESP_OK;
    }

    esp_err_t ret = spi_bus_remove_device(handle->spi_device);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to remove SPI device: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = spi_bus_free(MAX6675_SPI_HOST);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to free SPI bus: %s", esp_err_to_name(ret));
        return ret;
    }

    handle->initialized = false;
    ESP_LOGI(TAG, "MAX6675 deinitialized");

    return ESP_OK;
}
