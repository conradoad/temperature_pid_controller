/*
 * REST Server Implementation
 */

#include "rest_server.h"
#include "esp_log.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "REST_SERVER";

static httpd_handle_t server = NULL;
static max6675_handle_t *temp_sensor = NULL;
static mosfet_pwm_handle_t *pwm_controller = NULL;

// HTML page for the interface
static const char* html_page = 
"<!DOCTYPE html>"
"<html>"
"<head>"
"    <title>Temperature PID Controller</title>"
"    <meta charset='UTF-8'>"
"    <meta name='viewport' content='width=device-width, initial-scale=1.0'>"
"    <style>"
"        body { font-family: Arial, sans-serif; margin: 20px; background-color: #f0f0f0; }"
"        .container { max-width: 600px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }"
"        h1 { color: #333; text-align: center; }"
"        .status { background: #e8f5e8; padding: 15px; border-radius: 5px; margin: 10px 0; }"
"        .control { background: #f8f8f8; padding: 15px; border-radius: 5px; margin: 10px 0; }"
"        .slider { width: 100%; margin: 10px 0; }"
"        .value { font-size: 18px; font-weight: bold; color: #007bff; }"
"        button { background: #007bff; color: white; border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer; margin: 5px; }"
"        button:hover { background: #0056b3; }"
"        .error { color: red; }"
"        .success { color: green; }"
"    </style>"
"</head>"
"<body>"
"    <div class='container'>"
"        <h1>🔥 Temperature PID Controller</h1>"
"        <div class='status'>"
"            <h3>📊 Status</h3>"
"            <p>Temperature: <span id='temperature' class='value'>--°C</span></p>"
"            <p>Power: <span id='power' class='value'>--%</span></p>"
"            <p>Status: <span id='status' class='value'>--</span></p>"
"        </div>"
"        <div class='control'>"
"            <h3>🎛️ Power Control</h3>"
"            <input type='range' id='powerSlider' class='slider' min='0' max='100' value='0'>"
"            <p>Power: <span id='powerValue'>0%</span></p>"
"            <button onclick='setPower()'>Set Power</button>"
"            <button onclick='stopPower()'>Stop</button>"
"        </div>"
"        <div class='control'>"
"            <h3>🔄 Manual Control</h3>"
"            <button onclick='getTemperature()'>Read Temperature</button>"
"            <button onclick='startAutoUpdate()'>Start Auto Update</button>"
"            <button onclick='stopAutoUpdate()'>Stop Auto Update</button>"
"        </div>"
"    </div>"
"    <script>"
"        let autoUpdateInterval;"
"        const powerSlider = document.getElementById('powerSlider');"
"        const powerValue = document.getElementById('powerValue');"
"        const temperature = document.getElementById('temperature');"
"        const power = document.getElementById('power');"
"        const status = document.getElementById('status');"
"        "
"        powerSlider.oninput = function() {"
"            powerValue.textContent = this.value + '%';"
"        };"
"        "
"        function setPower() {"
"            const powerLevel = powerSlider.value;"
"            fetch('/api/power', {"
"                method: 'POST',"
"                headers: { 'Content-Type': 'application/json' },"
"                body: JSON.stringify({power: parseInt(powerLevel)})"
"            })"
"            .then(response => response.json())"
"            .then(data => {"
"                if (data.success) {"
"                    power.textContent = powerLevel + '%';"
"                    status.textContent = 'Power set successfully';"
"                    status.className = 'value success';"
"                } else {"
"                    status.textContent = 'Error: ' + data.error;"
"                    status.className = 'value error';"
"                }"
"            })"
"            .catch(error => {"
"                status.textContent = 'Error: ' + error;"
"                status.className = 'value error';"
"            });"
"        }"
"        "
"        function stopPower() {"
"            powerSlider.value = 0;"
"            powerValue.textContent = '0%';"
"            setPower();"
"        }"
"        "
"        function getTemperature() {"
"            fetch('/api/temperature')"
"            .then(response => response.json())"
"            .then(data => {"
"                if (data.success) {"
"                    temperature.textContent = data.temperature.toFixed(2) + '°C';"
"                    status.textContent = 'Temperature updated';"
"                    status.className = 'value success';"
"                } else {"
"                    status.textContent = 'Error: ' + data.error;"
"                    status.className = 'value error';"
"                }"
"            })"
"            .catch(error => {"
"                status.textContent = 'Error: ' + error;"
"                status.className = 'value error';"
"            });"
"        }"
"        "
"        function startAutoUpdate() {"
"            if (autoUpdateInterval) clearInterval(autoUpdateInterval);"
"            autoUpdateInterval = setInterval(getTemperature, 2000);"
"            status.textContent = 'Auto update started';"
"            status.className = 'value success';"
"        }"
"        "
"        function stopAutoUpdate() {"
"            if (autoUpdateInterval) clearInterval(autoUpdateInterval);"
"            status.textContent = 'Auto update stopped';"
"            status.className = 'value success';"
"        }"
"        "
"        // Start auto update on page load"
"        window.onload = function() {"
"            getTemperature();"
"            startAutoUpdate();"
"        };"
"    </script>"
"</body>"
"</html>";

// Handler for root page
static esp_err_t root_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
}

// Handler for temperature API
static esp_err_t temperature_handler(httpd_req_t *req)
{
    cJSON *json = cJSON_CreateObject();
    float temperature = 0.0f;
    
    if (temp_sensor != NULL) {
        esp_err_t ret = max6675_read_temperature(temp_sensor, &temperature);
        if (ret == ESP_OK) {
            cJSON_AddBoolToObject(json, "success", true);
            cJSON_AddNumberToObject(json, "temperature", temperature);
            ESP_LOGI(TAG, "Temperature: %.2f°C", temperature);
        } else if (ret == ESP_ERR_INVALID_RESPONSE) {
            cJSON_AddBoolToObject(json, "success", false);
            cJSON_AddStringToObject(json, "error", "Thermocouple not connected");
        } else {
            cJSON_AddBoolToObject(json, "success", false);
            cJSON_AddStringToObject(json, "error", "Failed to read temperature");
        }
    } else {
        cJSON_AddBoolToObject(json, "success", false);
        cJSON_AddStringToObject(json, "error", "Temperature sensor not initialized");
    }
    
    char *json_string = cJSON_Print(json);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_string, HTTPD_RESP_USE_STRLEN);
    
    free(json_string);
    cJSON_Delete(json);
    return ESP_OK;
}

// Handler for power control API
static esp_err_t power_handler(httpd_req_t *req)
{
    cJSON *json = cJSON_CreateObject();
    char buf[100];
    int ret = httpd_req_recv(req, buf, sizeof(buf));
    
    if (ret <= 0) {
        cJSON_AddBoolToObject(json, "success", false);
        cJSON_AddStringToObject(json, "error", "Failed to receive data");
    } else {
        buf[ret] = '\0';
        cJSON *root = cJSON_Parse(buf);
        
        if (root != NULL) {
            cJSON *power_item = cJSON_GetObjectItem(root, "power");
            if (cJSON_IsNumber(power_item)) {
                int power_level = power_item->valueint;
                
                if (power_level >= 0 && power_level <= 100) {
                    if (pwm_controller != NULL) {
                        esp_err_t ret = mosfet_pwm_set_duty(pwm_controller, power_level);
                        if (ret == ESP_OK) {
                            cJSON_AddBoolToObject(json, "success", true);
                            cJSON_AddNumberToObject(json, "power", power_level);
                            ESP_LOGI(TAG, "Power set to %d%%", power_level);
                        } else {
                            cJSON_AddBoolToObject(json, "success", false);
                            cJSON_AddStringToObject(json, "error", "Failed to set power");
                        }
                    } else {
                        cJSON_AddBoolToObject(json, "success", false);
                        cJSON_AddStringToObject(json, "error", "PWM controller not initialized");
                    }
                } else {
                    cJSON_AddBoolToObject(json, "success", false);
                    cJSON_AddStringToObject(json, "error", "Power level must be between 0 and 100");
                }
            } else {
                cJSON_AddBoolToObject(json, "success", false);
                cJSON_AddStringToObject(json, "error", "Invalid power value");
            }
            cJSON_Delete(root);
        } else {
            cJSON_AddBoolToObject(json, "success", false);
            cJSON_AddStringToObject(json, "error", "Invalid JSON");
        }
    }
    
    char *json_string = cJSON_Print(json);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_string, HTTPD_RESP_USE_STRLEN);
    
    free(json_string);
    cJSON_Delete(json);
    return ESP_OK;
}

esp_err_t rest_server_init(max6675_handle_t *temp_sensor_handle, mosfet_pwm_handle_t *pwm_controller_handle)
{
    temp_sensor = temp_sensor_handle;
    pwm_controller = pwm_controller_handle;
    
    ESP_LOGI(TAG, "REST server initialized");
    return ESP_OK;
}

esp_err_t rest_server_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = REST_SERVER_PORT;
    config.max_uri_handlers = REST_SERVER_MAX_URI_HANDLERS;
    
    // Start the HTTP server
    if (httpd_start(&server, &config) == ESP_OK) {
        // Register URI handlers
        httpd_uri_t root_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &root_uri);
        
        httpd_uri_t temperature_uri = {
            .uri = "/api/temperature",
            .method = HTTP_GET,
            .handler = temperature_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &temperature_uri);
        
        httpd_uri_t power_uri = {
            .uri = "/api/power",
            .method = HTTP_POST,
            .handler = power_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &power_uri);
        
        ESP_LOGI(TAG, "REST server started on port %d", REST_SERVER_PORT);
        return ESP_OK;
    }
    
    ESP_LOGE(TAG, "Failed to start REST server");
    return ESP_FAIL;
}

esp_err_t rest_server_stop(void)
{
    if (server != NULL) {
        httpd_stop(server);
        server = NULL;
        ESP_LOGI(TAG, "REST server stopped");
        return ESP_OK;
    }
    return ESP_FAIL;
}

void rest_server_deinit(void)
{
    rest_server_stop();
    temp_sensor = NULL;
    pwm_controller = NULL;
    ESP_LOGI(TAG, "REST server deinitialized");
}
