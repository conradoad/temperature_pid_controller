/*
 * Temperature PID Controller - ESP32 DevKitC
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
#include "mosfet_pwm.h"
#include "wifi_manager.h"
#include "rest_server.h"

static const char *TAG = "TEMP_CONTROLLER";

void app_main(void)
{
    ESP_LOGI(TAG, "Temperature PID Controller Starting on ESP32 DevKitC...");
    
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
    ESP_LOGI(TAG, "- Board: ESP32 DevKitC");
    ESP_LOGI(TAG, "- Temperature sensor: Termopar + MAX6675 (SPI)");
    ESP_LOGI(TAG, "- Actuator: Nichrome wire + MOSFET IRF3205 (PWM on GPIO4)");
    ESP_LOGI(TAG, "- Control: PID algorithm");
    ESP_LOGI(TAG, "- Interface: Web server");
    ESP_LOGI(TAG, "- Status LED: Onboard blue LED (GPIO2)");

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

    // Initialize MOSFET PWM control
    mosfet_pwm_handle_t mosfet_handle = {0};
    ret = mosfet_pwm_init(&mosfet_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MOSFET PWM: %s", esp_err_to_name(ret));
        ESP_LOGE(TAG, "Please check PWM pin connection: GPIO%d (GPIO2 reserved for onboard LED)", MOSFET_PWM_PIN);
        return;
    }

    ESP_LOGI(TAG, "MOSFET PWM initialized successfully");

    // Initialize WiFi
    ret = wifi_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WiFi: %s", esp_err_to_name(ret));
        return;
    }

    ret = wifi_connect();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to connect to WiFi: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "WiFi connected successfully");

    // Initialize REST server
    ret = rest_server_init(&max6675_handle, &mosfet_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize REST server: %s", esp_err_to_name(ret));
        return;
    }

    ret = rest_server_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start REST server: %s", esp_err_to_name(ret));
        return;
    }

    esp_ip4_addr_t ip = wifi_get_ip();
    ESP_LOGI(TAG, "REST server started successfully");
    ESP_LOGI(TAG, "Web interface available at: http://" IPSTR, IP2STR(&ip));
    ESP_LOGI(TAG, "API endpoints:");
    ESP_LOGI(TAG, "  GET  /api/temperature - Read temperature");
    ESP_LOGI(TAG, "  POST /api/power      - Set power (0-100%%)");

    // Main application loop - monitor system status
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
        
        // Check WiFi connection status
        if (!wifi_is_connected()) {
            ESP_LOGW(TAG, "WiFi disconnected, attempting to reconnect...");
            wifi_connect();
        }
        
        ESP_LOGI(TAG, "System running - Web interface available");
        ESP_LOGI(TAG, "Free heap: %" PRIu32 " bytes", esp_get_free_heap_size());
        
        // Wait 10 seconds before next status check
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}