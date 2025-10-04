/*
 * MOSFET PWM Control Driver
 * 
 * This driver provides PWM control for MOSFET IRF3205 to control
 * nichrome wire heating element.
 */

#ifndef MOSFET_PWM_H
#define MOSFET_PWM_H

#include "esp_err.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

// PWM Configuration
#define MOSFET_PWM_TIMER          LEDC_TIMER_0
#define MOSFET_PWM_MODE           LEDC_LOW_SPEED_MODE
#define MOSFET_PWM_CHANNEL        LEDC_CHANNEL_0
#define MOSFET_PWM_FREQUENCY      1000    // 1 kHz PWM frequency
#define MOSFET_PWM_RESOLUTION      LEDC_TIMER_12_BIT  // 0-4095 duty cycle
#define MOSFET_PWM_MAX_DUTY       4095    // Maximum duty cycle (100%)

// GPIO Pin Configuration
#define MOSFET_PWM_PIN            GPIO_NUM_2   // PWM output pin

// Power Control Functions
typedef struct {
    ledc_channel_config_t channel_config;
    bool initialized;
    uint32_t current_duty;
} mosfet_pwm_handle_t;

// Function prototypes
esp_err_t mosfet_pwm_init(mosfet_pwm_handle_t *handle);
esp_err_t mosfet_pwm_set_duty(mosfet_pwm_handle_t *handle, uint32_t duty_percent);
esp_err_t mosfet_pwm_set_power(mosfet_pwm_handle_t *handle, float power_percent);
esp_err_t mosfet_pwm_stop(mosfet_pwm_handle_t *handle);
esp_err_t mosfet_pwm_start(mosfet_pwm_handle_t *handle);
esp_err_t mosfet_pwm_deinit(mosfet_pwm_handle_t *handle);

// Utility functions
uint32_t mosfet_pwm_percent_to_duty(float percent);
float mosfet_pwm_duty_to_percent(uint32_t duty);

#ifdef __cplusplus
}
#endif

#endif // MOSFET_PWM_H
