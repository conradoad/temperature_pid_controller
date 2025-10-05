/*
 * REST Server for Temperature PID Controller
 * Provides API endpoints for power control and temperature reading
 */

#ifndef REST_SERVER_H
#define REST_SERVER_H

#include "esp_err.h"
#include "esp_http_server.h"
#include "max6675.h"
#include "mosfet_pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

// Server configuration
#define REST_SERVER_PORT 80
#define REST_SERVER_MAX_URI_HANDLERS 8

// Function prototypes
esp_err_t rest_server_init(max6675_handle_t *temp_sensor, mosfet_pwm_handle_t *pwm_controller);
esp_err_t rest_server_start(void);
esp_err_t rest_server_stop(void);
void rest_server_deinit(void);

#ifdef __cplusplus
}
#endif

#endif // REST_SERVER_H
