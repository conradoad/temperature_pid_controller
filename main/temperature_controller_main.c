/*
 * Temperature PID Controller - ESP32
 * Testing MAX6675 temperature sensor reading
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "max6675.h"

static const char *TAG = "TEMP_CONTROLLER";

void app_main(void)
{
    ESP_LOGI(TAG, "Temperature PID Controller Starting...");
    
    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    
    ESP_LOGI(TAG, "This is %s chip with %d CPU core(s), %s%s%s%s",
             CONFIG_IDF_TARGET,
             chip_info.cores,
             (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
             (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
             (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
             (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    ESP_LOGI(TAG, "Silicon revision v%d.%d", major_rev, minor_rev);
    
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        ESP_LOGE(TAG, "Get flash size failed");
        return;
    }

    ESP_LOGI(TAG, "%" PRIu32 "MB %s flash",
             flash_size / (uint32_t)(1024 * 1024),
             (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    ESP_LOGI(TAG, "Minimum free heap size: %" PRIu32 " bytes", esp_get_minimum_free_heap_size());

    ESP_LOGI(TAG, "Project components:");
    ESP_LOGI(TAG, "- Temperature sensor: Termopar + MAX6675 (SPI)");
    ESP_LOGI(TAG, "- Actuator: Nichrome wire + MOSFET IRF3205 (PWM)");
    ESP_LOGI(TAG, "- Control: PID algorithm");
    ESP_LOGI(TAG, "- Interface: Web server");

    // Initialize MAX6675 temperature sensor
    max6675_handle_t max6675_handle = {0};
    esp_err_t ret = max6675_init(&max6675_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MAX6675: %s", esp_err_to_name(ret));
        ESP_LOGE(TAG, "Please check SPI connections:");
        ESP_LOGE(TAG, "- MISO: GPIO%d", MAX6675_MISO_PIN);
        ESP_LOGE(TAG, "- MOSI: GPIO%d", MAX6675_MOSI_PIN);
        ESP_LOGE(TAG, "- CLK:  GPIO%d", MAX6675_CLK_PIN);
        ESP_LOGE(TAG, "- CS:   GPIO%d", MAX6675_CS_PIN);
        return;
    }

    ESP_LOGI(TAG, "MAX6675 initialized successfully");
    ESP_LOGI(TAG, "Starting temperature readings every 1 second...");

    // Main application loop - read temperature every 1 second
    int reading_count = 0;
    float temperature = 0.0f;
    
    while (1) {
        reading_count++;
        
        // Read temperature from MAX6675
        ret = max6675_read_temperature(&max6675_handle, &temperature);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Reading #%d: Temperature = %.2f°C", reading_count, temperature);
        } else if (ret == ESP_ERR_INVALID_RESPONSE) {
            ESP_LOGW(TAG, "Reading #%d: Thermocouple not connected!", reading_count);
        } else {
            ESP_LOGE(TAG, "Reading #%d: Failed to read temperature: %s", 
                     reading_count, esp_err_to_name(ret));
        }
        
        ESP_LOGI(TAG, "Free heap: %" PRIu32 " bytes", esp_get_free_heap_size());
        
        // Wait 1 second before next reading
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}