#ifndef _DISPLAY_SPI_SOFTWARE_H_
#define _DISPLAY_SPI_SOFTWARE_H_

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define SPI_MOSI_PIN        GPIO_NUM_11
#define SPI_CLK_PIN         GPIO_NUM_12
#define SPI_CS_PIN          GPIO_NUM_10
#define SPI_DC_PIN          GPIO_NUM_9
#define DISPLAY_RST_PIN     GPIO_NUM_14
#define DISPLAY_BUSY_PIN    GPIO_NUM_13


#define SPI_MOSI_0          gpio_set_level(SPI_MOSI_PIN, 0)
#define SPI_MOSI_1          gpio_set_level(SPI_MOSI_PIN, 1)

#define SPI_CLK_0           gpio_set_level(SPI_CLK_PIN, 0)
#define SPI_CLK_1           gpio_set_level(SPI_CLK_PIN, 1)

#define SPI_CS_0            gpio_set_level(SPI_CS_PIN, 0)
#define SPI_CS_1            gpio_set_level(SPI_CS_PIN, 1)

#define SPI_DC_0            gpio_set_level(SPI_DC_PIN, 0)
#define SPI_DC_1            gpio_set_level(SPI_DC_PIN, 1)

#define DISPLAY_RST_0       gpio_set_level(DISPLAY_RST_PIN, 0)
#define DISPLAY_RST_1       gpio_set_level(DISPLAY_RST_PIN, 1)

#define IS_DISPLAY_BUSY     gpio_get_level(DISPLAY_BUSY_PIN)


struct DosoSpiSoftwareDisplayUser {
    uint8_t mosi_pin;
    uint8_t clk_pin;
    uint8_t cs_pin;
    uint8_t dc_pin;
    uint8_t rst_pin;
    uint8_t busy_pin;

    uint16_t tx_buffer_size;
    uint8_t tx_buffer[128];
};


void spi_write_data(unsigned char datas);
void spi_write_cmd(unsigned char command);
void spi_gpio_init(void);

#endif  // #ifndef _DISPLAY_SPI_SOFTWARE_H_
