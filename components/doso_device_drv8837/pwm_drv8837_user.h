#ifndef _PWM_DRV8837_USER_H
#define _PWM_DRV8837_USER_H

#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

#include "doso_gpio_user.h"



#define PWM1_TIMER              LEDC_TIMER_0
#define PWM1_CHANNEL            LEDC_CHANNEL_0
#define PWM1_OUTPUT_IO          (GPIO_NUM_3)

#define PWM2_TIMER              LEDC_TIMER_1
#define PWM2_CHANNEL            LEDC_CHANNEL_1
#define PWM2_OUTPUT_IO          (GPIO_NUM_9)


#define PWM_SPEED_MODE          LEDC_LOW_SPEED_MODE
#define PWM_DUTY_RES            LEDC_TIMER_7_BIT // Set duty resolution to 12 bits
#define PWM_DUTY_MAX            (128 - 1)        // 
#define PWM_FREQUENCY           (50000)           // Frequency in Hertz. Set frequency at 50 kHz
#define PWM_CLK                 LEDC_AUTO_CLK



#define PWM_RESOLUTION                  PWM_DUTY_MAX       // PWM波分辨率
#define DRV8837_IN_1                    PWM1_CHANNEL
#define DRV8837_IN_2                    PWM2_CHANNEL
#define DRV8837_SLEEP_GPIO              (GPIO_NUM_46)

void drv8837_init(void);
/*
 * duty: 0~100%
 */
void drv8837_set_forward(uint8_t duty);
/*
 * duty: 0~100%
 */
void drv8837_set_reverse(uint8_t duty);
void drv8837_set_stop(void);



#endif
