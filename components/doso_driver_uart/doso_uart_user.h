//
// Created by gao on 23-3-26.
//

#ifndef ESP_UART_USER_H
#define ESP_UART_USER_H

#include "driver/uart.h"
#include "driver/gpio.h"
#include <stdint.h>



#define MAX_BUFFER_SIZE     (512)



struct DosoUartUser {
    uint8_t uart_num;
    uint8_t tx_pin;
    uint8_t rx_pin;
    int baud_rate;

    uint16_t rx_timeout_ms;
    uint16_t rx_buffer_size;
    uint16_t tx_buffer_size;

    uint16_t receive_len;
    uint8_t rx_buffer[MAX_BUFFER_SIZE];
    uint8_t tx_buffer[MAX_BUFFER_SIZE];

    int (*write)(struct DosoUartUser *,uint16_t);
    int (*read)(struct DosoUartUser *,uint16_t);

};


int8_t doso_uart_init(struct DosoUartUser * uart_user_);

#endif //ESP_UART_USER_H
