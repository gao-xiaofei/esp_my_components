#ifndef ESP_RMT_WS2812_USER_H
#define ESP_RMT_WS2812_USER_H

#include <string.h>
#include "driver/rmt_tx.h"
#include "esp_log.h"

#define RMT_RESOLUTION_HZ   10000000  // 10MHz resolution, 0.1us
#define WS2812_MAX_NUM          3 // 每个pin最大支持的WS2812数量

struct RMT_WS2812_USER_t {

    uint8_t use_ws2812_num;
    uint8_t gpio_num;
    rmt_transmit_config_t tx_config;

    uint8_t rgb_buf[WS2812_MAX_NUM * 3];
    rmt_channel_handle_t ws2812_chan;
    rmt_encoder_handle_t ws2812_encoder;

};

esp_err_t ws2812_device_init(struct RMT_WS2812_USER_t *ws2812_user);
esp_err_t ws2812_set_color(struct RMT_WS2812_USER_t *ws2812_user, uint8_t index, uint8_t r, uint8_t g, uint8_t b);
esp_err_t ws2812_refresh(struct RMT_WS2812_USER_t *ws2812_user);

#endif
