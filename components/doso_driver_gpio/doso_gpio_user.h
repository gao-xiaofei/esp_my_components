#ifndef __DOSO_GPIO_USER_H__
#define __DOSO_GPIO_USER_H__

#include "driver/gpio.h"
#include <stdio.h>
#include <stdint.h>


struct DosoGpioUser {
    uint8_t gpio_pin;
    uint8_t gpio_mode;
    uint8_t pull_down_en;
    uint8_t pull_up_en;

    uint8_t intr_enable;
    uint8_t intr_type;

    uint8_t gpio_state;

    uint8_t (*get_state)(struct DosoGpioUser *);
    uint8_t (*set_state)(struct DosoGpioUser *,uint8_t);
    uint8_t (*intr_control)(struct DosoGpioUser *,uint8_t);
};


int8_t doso_gpio_user_init(struct DosoGpioUser *_gpio_user,gpio_isr_t isr_handler);


#endif
