/*
 * WiFi Manager for Temperature PID Controller
 * Handles WiFi connection and configuration
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_event.h"

#ifdef __cplusplus
extern "C" {
#endif

// WiFi Configuration
#define WIFI_SSID "AP_E109"
#define WIFI_PASSWORD "Ja170493!"
#define WIFI_MAXIMUM_RETRY 5

// Function prototypes
esp_err_t wifi_init(void);
esp_err_t wifi_connect(void);
esp_err_t wifi_disconnect(void);
bool wifi_is_connected(void);
esp_ip4_addr_t wifi_get_ip(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_MANAGER_H
