#include "doso_gpio_user.h"


static uint8_t s_gpio_get_state(struct DosoGpioUser *_gpio_user)
{
    _gpio_user->gpio_state = gpio_get_level(_gpio_user->gpio_pin);
    return _gpio_user->gpio_state;
}

static uint8_t s_gpio_set_state(struct DosoGpioUser *_gpio_user, uint8_t state)
{
    if (_gpio_user->gpio_mode == GPIO_MODE_OUTPUT || _gpio_user->gpio_mode == GPIO_MODE_INPUT_OUTPUT) {
        gpio_set_level(_gpio_user->gpio_pin, state);
        _gpio_user->gpio_state = state;
    }
    return _gpio_user->gpio_state;
}

static uint8_t s_gpio_intr_control(struct DosoGpioUser *_gpio_user, uint8_t enable)
{
    if (enable) {
        gpio_intr_enable(_gpio_user->gpio_pin);
    } else {
        gpio_intr_disable(_gpio_user->gpio_pin);
    }
    return 0;
}

int8_t doso_gpio_user_init(struct DosoGpioUser *_gpio_user,gpio_isr_t isr_handler)
{
    gpio_config_t io_conf = {0};
    _gpio_user->gpio_state = 0;

    io_conf.pin_bit_mask    = 1ULL << _gpio_user->gpio_pin;
    io_conf.mode            = _gpio_user->gpio_mode;
    io_conf.pull_down_en    = _gpio_user->pull_down_en;
    io_conf.pull_up_en      = _gpio_user->pull_up_en;

    gpio_config(&io_conf);

    if(_gpio_user->intr_enable)
    {
        if(isr_handler == NULL)
        {
            printf("isr_handler is NULL\n");
            return -1;
        }
        gpio_set_intr_type(_gpio_user->gpio_pin, _gpio_user->intr_type);
        gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);
        gpio_isr_handler_add(_gpio_user->gpio_pin, isr_handler, (void*)(uintptr_t)_gpio_user->gpio_pin);
    }

    _gpio_user->get_state = s_gpio_get_state;
    _gpio_user->set_state = s_gpio_set_state;
    _gpio_user->intr_control = s_gpio_intr_control;

    if (_gpio_user->gpio_mode == GPIO_MODE_OUTPUT || _gpio_user->gpio_mode == GPIO_MODE_INPUT_OUTPUT) {
        gpio_set_level(_gpio_user->gpio_pin, _gpio_user->gpio_state);
    } else {
        _gpio_user->gpio_state = gpio_get_level(_gpio_user->gpio_pin);
    }

    return 0;
}



