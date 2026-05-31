#include "rmt_ws2812_user.h"


static size_t encoder_callback(
    const void *data,
    size_t data_size,
    size_t symbols_written,
    size_t symbols_free,
    rmt_symbol_word_t *symbols,
    bool *done,
    void *arg);



esp_err_t ws2812_device_init(struct RMT_WS2812_USER_t *ws2812_user)
{
    if (ws2812_user->use_ws2812_num == 0 || ws2812_user->use_ws2812_num > WS2812_MAX_NUM) {
        return ESP_ERR_INVALID_ARG;
    }

    const rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = ws2812_user->gpio_num,
        .mem_block_symbols = 64,
        .resolution_hz = RMT_RESOLUTION_HZ,
        .trans_queue_depth = 4,
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &ws2812_user->ws2812_chan));

    const rmt_simple_encoder_config_t encoder_cfg = {
        .callback = encoder_callback
    };
    ESP_ERROR_CHECK(rmt_new_simple_encoder(&encoder_cfg, &ws2812_user->ws2812_encoder));

    ESP_ERROR_CHECK(rmt_enable(ws2812_user->ws2812_chan));

    ws2812_user->use_ws2812_num = ws2812_user->use_ws2812_num;
    memset(ws2812_user->rgb_buf, 0, sizeof(ws2812_user->rgb_buf));
    return ESP_OK;
}

esp_err_t ws2812_set_color(struct RMT_WS2812_USER_t *ws2812_user, uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if (index >= ws2812_user->use_ws2812_num) {
        return ESP_ERR_INVALID_ARG;
    }
    uint8_t *p = &ws2812_user->rgb_buf[index * 3];
    /* WS2812颜色顺序为GRB */
    p[0] = g;
    p[1] = r;
    p[2] = b;
    return ESP_OK;
}

esp_err_t ws2812_refresh(struct RMT_WS2812_USER_t *ws2812_user)
{
    if (!ws2812_user->ws2812_chan || !ws2812_user->ws2812_encoder) {
        return ESP_ERR_INVALID_STATE;
    }
    ESP_ERROR_CHECK(rmt_transmit(ws2812_user->ws2812_chan, ws2812_user->ws2812_encoder,
                                 ws2812_user->rgb_buf, ws2812_user->use_ws2812_num * 3, &ws2812_user->tx_config));
    ESP_ERROR_CHECK(rmt_tx_wait_all_done(ws2812_user->ws2812_chan, 0xffffffff));
    return ESP_OK;
}








static const rmt_symbol_word_t ws2812_zero = {
    .level0 = 1,
    .duration0 = 0.4f * RMT_RESOLUTION_HZ / 1000000, // T0H=0.4us
    .level1 = 0,
    .duration1 = 0.85f * RMT_RESOLUTION_HZ / 1000000, // T0L=0.85us
};

static const rmt_symbol_word_t ws2812_one = {
    .level0 = 1,
    .duration0 = 0.8f * RMT_RESOLUTION_HZ / 1000000, // T1H=0.8us
    .level1 = 0,
    .duration1 = 0.45f * RMT_RESOLUTION_HZ / 1000000, // T1L=0.45us
};
 
static const rmt_symbol_word_t ws2812_reset = {
    .level0 = 1,
    .duration0 = RMT_RESOLUTION_HZ / 1000000 * 50 / 2,
    .level1 = 0,
    .duration1 = RMT_RESOLUTION_HZ / 1000000 * 50 / 2,
};

static size_t encoder_callback(
    const void *data,
    size_t data_size,
    size_t symbols_written,
    size_t symbols_free,
    rmt_symbol_word_t *symbols,
    bool *done,
    void *arg)
{
    /* 写一个字节至少需要8个符号 */
    if (symbols_free < 8) {
        return 0;
    }
 
    size_t data_pos = symbols_written / 8;
    uint8_t *data_bytes = (uint8_t*) data;
    if (data_pos < data_size) {
        /* 写一个字节，高位在先 */
        size_t symbol_pos = 0;
        for (uint8_t bitmask = 0x80; bitmask; bitmask >>= 1) {
            if (data_bytes[data_pos] & bitmask) {
                symbols[symbol_pos++] = ws2812_one;
            } else {
                symbols[symbol_pos++] = ws2812_zero;
            }
        }
        return symbol_pos;
    } else {
        /* 传输完成，发送复位 */
        symbols[0] = ws2812_reset;
        *done = 1;
        return 1;
    }
}



