#include "pwm.h"
#include "esp_err.h"
#include <stdio.h>

esp_err_t pwm_init(uint32_t frequency, int gpio, ledc_channel_t channel, int duty){
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_HIGH_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_BIT_13,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = frequency
        //.clk_cfg          = LEDC_AUTO_CLK     Don't think we need this
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_HIGH_SPEED_MODE,
        .channel        = channel,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = gpio, //Output pin
        .duty           = duty, 
        .hpoint         = 0 //No idea
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}
