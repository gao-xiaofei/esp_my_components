/* 
 *
 */
#include "pwm_drv8837_user.h"

static struct doso_gpio_user_t drv8837_sleep_gpio = {
    .gpio_pin = DRV8837_SLEEP_GPIO,
    .gpio_mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_enable = false,
    .intr_type = GPIO_INTR_DISABLE,
};

static void pwm_config(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t pwm_timer = {
        .speed_mode       = PWM_SPEED_MODE,
        .timer_num        = PWM1_TIMER,
        .duty_resolution  = PWM_DUTY_RES,
        .freq_hz          = PWM_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = PWM_CLK
    };

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t pwm_channel = {
        .speed_mode     = PWM_SPEED_MODE,
        .channel        = PWM1_CHANNEL,
        .timer_sel      = PWM1_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PWM1_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer));
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_channel));

    pwm_timer.timer_num     = PWM2_TIMER;
    pwm_channel.timer_sel   = PWM2_TIMER;
    pwm_channel.channel     = PWM2_CHANNEL;
    pwm_channel.gpio_num    = PWM2_OUTPUT_IO;
    ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer));
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_channel));

}

/*
 * duty: 0~100%
 */
static void set_pwm_duty(ledc_channel_t pwm_channel, uint8_t duty)
{
    uint32_t duty_res = duty * PWM_RESOLUTION / 100;
    // Set duty
    ESP_ERROR_CHECK(ledc_set_duty(PWM_SPEED_MODE, pwm_channel, duty_res));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(PWM_SPEED_MODE, pwm_channel));
}

static void gpio_init(void)
{
    doso_gpio_user_init(&drv8837_sleep_gpio, NULL);
}

void drv8837_init(void)
{
    gpio_init();
    pwm_config();
    // Set sleep gpio to high -> DRV8837 is active
    drv8837_sleep_gpio.set_state(&drv8837_sleep_gpio, 1);
}
/*
 * duty: 0~100%
 */
void drv8837_set_forward(uint8_t duty)
{
    set_pwm_duty(DRV8837_IN_1, duty);
    set_pwm_duty(DRV8837_IN_2, 0);
}
/*
 * duty: 0~100%
 */
void drv8837_set_reverse(uint8_t duty)
{
    set_pwm_duty(DRV8837_IN_1, 0);
    set_pwm_duty(DRV8837_IN_2, duty);
}

void drv8837_set_stop(void)
{
    set_pwm_duty(DRV8837_IN_1, 100);
    set_pwm_duty(DRV8837_IN_2, 100);
}


