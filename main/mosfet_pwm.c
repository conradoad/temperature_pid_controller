/*
 * MOSFET PWM Control Driver Implementation
 */

#include "mosfet_pwm.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MOSFET_PWM";

esp_err_t mosfet_pwm_init(mosfet_pwm_handle_t *handle)
{
    if (handle == NULL) {
        ESP_LOGE(TAG, "Handle is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    // Configure LEDC timer
    ledc_timer_config_t timer_config = {
        .speed_mode = MOSFET_PWM_MODE,
        .timer_num = MOSFET_PWM_TIMER,
        .duty_resolution = MOSFET_PWM_RESOLUTION,
        .freq_hz = MOSFET_PWM_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };

    esp_err_t ret = ledc_timer_config(&timer_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC timer: %s", esp_err_to_name(ret));
        return ret;
    }

    // Configure LEDC channel
    ledc_channel_config_t channel_config = {
        .speed_mode = MOSFET_PWM_MODE,
        .channel = MOSFET_PWM_CHANNEL,
        .timer_sel = MOSFET_PWM_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = MOSFET_PWM_PIN,
        .duty = 0,  // Start with 0% duty cycle
        .hpoint = 0
    };

    ret = ledc_channel_config(&channel_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC channel: %s", esp_err_to_name(ret));
        return ret;
    }

    handle->channel_config = channel_config;
    handle->initialized = true;
    handle->current_duty = 0;

    ESP_LOGI(TAG, "MOSFET PWM initialized successfully");
    ESP_LOGI(TAG, "PWM Pin: GPIO%d, Frequency: %d Hz, Resolution: %d bits", 
             MOSFET_PWM_PIN, MOSFET_PWM_FREQUENCY, MOSFET_PWM_RESOLUTION);
    ESP_LOGI(TAG, "Max duty cycle: %d (100%%)", MOSFET_PWM_MAX_DUTY);

    return ESP_OK;
}

esp_err_t mosfet_pwm_set_duty(mosfet_pwm_handle_t *handle, uint32_t duty_percent)
{
    if (handle == NULL) {
        ESP_LOGE(TAG, "Handle is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (!handle->initialized) {
        ESP_LOGE(TAG, "MOSFET PWM not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (duty_percent > 100) {
        ESP_LOGW(TAG, "Duty cycle clamped to 100%% (was %d%%)", duty_percent);
        duty_percent = 100;
    }

    uint32_t duty_value = mosfet_pwm_percent_to_duty(duty_percent);
    
    esp_err_t ret = ledc_set_duty(MOSFET_PWM_MODE, MOSFET_PWM_CHANNEL, duty_value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set duty cycle: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = ledc_update_duty(MOSFET_PWM_MODE, MOSFET_PWM_CHANNEL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update duty cycle: %s", esp_err_to_name(ret));
        return ret;
    }

    handle->current_duty = duty_value;
    
    ESP_LOGI(TAG, "PWM duty cycle set to %d%% (duty value: %d)", duty_percent, duty_value);

    return ESP_OK;
}

esp_err_t mosfet_pwm_set_power(mosfet_pwm_handle_t *handle, float power_percent)
{
    if (handle == NULL) {
        ESP_LOGE(TAG, "Handle is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (power_percent < 0.0f) {
        ESP_LOGW(TAG, "Power percentage clamped to 0%% (was %.2f%%)", power_percent);
        power_percent = 0.0f;
    } else if (power_percent > 100.0f) {
        ESP_LOGW(TAG, "Power percentage clamped to 100%% (was %.2f%%)", power_percent);
        power_percent = 100.0f;
    }

    uint32_t duty_percent = (uint32_t)power_percent;
    return mosfet_pwm_set_duty(handle, duty_percent);
}

esp_err_t mosfet_pwm_stop(mosfet_pwm_handle_t *handle)
{
    if (handle == NULL) {
        ESP_LOGE(TAG, "Handle is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (!handle->initialized) {
        ESP_LOGW(TAG, "MOSFET PWM not initialized");
        return ESP_OK;
    }

    esp_err_t ret = mosfet_pwm_set_duty(handle, 0);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "MOSFET PWM stopped (0%% duty cycle)");
    }

    return ret;
}

esp_err_t mosfet_pwm_start(mosfet_pwm_handle_t *handle)
{
    if (handle == NULL) {
        ESP_LOGE(TAG, "Handle is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (!handle->initialized) {
        ESP_LOGE(TAG, "MOSFET PWM not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGI(TAG, "MOSFET PWM started (current duty: %d%%)", 
             mosfet_pwm_duty_to_percent(handle->current_duty));

    return ESP_OK;
}

esp_err_t mosfet_pwm_deinit(mosfet_pwm_handle_t *handle)
{
    if (handle == NULL) {
        ESP_LOGE(TAG, "Handle is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (!handle->initialized) {
        ESP_LOGW(TAG, "MOSFET PWM not initialized");
        return ESP_OK;
    }

    // Stop PWM first
    mosfet_pwm_stop(handle);

    // Note: LEDC doesn't have explicit deinit function
    // The timer and channel will be automatically cleaned up
    handle->initialized = false;
    
    ESP_LOGI(TAG, "MOSFET PWM deinitialized");

    return ESP_OK;
}

uint32_t mosfet_pwm_percent_to_duty(float percent)
{
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 100.0f) percent = 100.0f;
    
    return (uint32_t)((percent / 100.0f) * MOSFET_PWM_MAX_DUTY);
}

float mosfet_pwm_duty_to_percent(uint32_t duty)
{
    if (duty > MOSFET_PWM_MAX_DUTY) duty = MOSFET_PWM_MAX_DUTY;
    
    return (float)((duty * 100.0f) / MOSFET_PWM_MAX_DUTY);
}
