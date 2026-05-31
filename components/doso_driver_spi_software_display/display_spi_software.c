#include "display_spi_software.h"
#include "esp_rom_sys.h"

// spi gpio initialization
void spi_gpio_init(void)
{
    // Configure output pins
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SPI_MOSI_PIN) | 
                        (1ULL << SPI_CLK_PIN) | 
                        (1ULL << SPI_CS_PIN) | 
                        (1ULL << SPI_DC_PIN) | 
                        (1ULL << DISPLAY_RST_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // Configure BUSY pin as input with pull-up
    io_conf.pin_bit_mask = (1ULL << DISPLAY_BUSY_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // Set initial state
    SPI_CS_1;
    SPI_DC_1;
    DISPLAY_RST_1;
    SPI_CLK_0;
    SPI_MOSI_0;
}

// SPI write byte (software SPI)
static void spi_write(unsigned char value)
{
    unsigned char i;
    SPI_CLK_0;
    for(i = 0; i < 8; i++)
    {
        if(value & 0x80)
            SPI_MOSI_1;
        else
            SPI_MOSI_0;
        SPI_CLK_1;
        SPI_CLK_0;
        value = value << 1;
    }
}

// SPI write command
void spi_write_cmd(unsigned char command)
{
    SPI_CS_0;
    SPI_DC_0;  // D/C#   0:command  1:data
    spi_write(command);
    SPI_CS_1;
}

// SPI write data
void spi_write_data(unsigned char datas)
{
    SPI_CS_0;
    SPI_DC_1;  // D/C#   0:command  1:data
    spi_write(datas);
    SPI_CS_1;
}