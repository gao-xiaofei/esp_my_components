//
// Created by gao on 23-3-26.
//

#include "doso_uart_user.h"

static int uart_write(struct DosoUartUser * uart_user_,uint16_t date_len)
{
    int ret = uart_write_bytes(uart_user_->uart_num, (const char *)uart_user_->tx_buffer, date_len);
    if (date_len != ret)    ret = -2;
    return ret;
}

static int uart_read(struct DosoUartUser * uart_user_,uint16_t date_len)
{
    if(uart_user_->rx_timeout_ms < 10) uart_user_->rx_timeout_ms = 10;
    int ret = uart_read_bytes(uart_user_->uart_num, uart_user_->rx_buffer, date_len, uart_user_->rx_timeout_ms / portTICK_PERIOD_MS);
    uart_user_->receive_len = ret;
    return ret;
}

int8_t doso_uart_init(struct DosoUartUser * uart_user_)
{
        uart_config_t uart_config = {
            .baud_rate = uart_user_->baud_rate,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_DEFAULT,
    };

    if(uart_user_->rx_buffer_size < 128) uart_user_->rx_buffer_size = 128;
    if(uart_user_->tx_buffer_size < 128) uart_user_->tx_buffer_size = 128;

    if(uart_user_->rx_buffer_size > MAX_BUFFER_SIZE) uart_user_->rx_buffer_size = MAX_BUFFER_SIZE;
    if(uart_user_->tx_buffer_size > MAX_BUFFER_SIZE) uart_user_->tx_buffer_size = MAX_BUFFER_SIZE;

    uart_user_->write = uart_write;
    uart_user_->read = uart_read;

    esp_err_t ret = 0;
    ret = uart_driver_install(uart_user_->uart_num, uart_user_->rx_buffer_size, uart_user_->tx_buffer_size, 0, NULL, 0);
    ESP_ERROR_CHECK(ret);
    ret = uart_param_config(uart_user_->uart_num, &uart_config);
    ESP_ERROR_CHECK(ret);
    ret = uart_set_pin(uart_user_->uart_num, uart_user_->tx_pin, uart_user_->rx_pin, UART_RTS, UART_CTS);
    ESP_ERROR_CHECK(ret);

    return 0;
}





















