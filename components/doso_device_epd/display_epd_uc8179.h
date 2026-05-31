#ifndef _DISPLAY_EPD_UC8179_H_
#define _DISPLAY_EPD_UC8179_H_

#include <stdint.h>
#include "display_spi_software.h"


#define EPD_WIDTH           800 
#define EPD_HEIGHT          480
#define EPD_ARRAY_SIZE      (EPD_WIDTH * EPD_HEIGHT / 8)
#define PANNEL_ANGLE        0       // 屏幕上下翻转


// Full screen update display
void display_epd_init(void); 
void display_epd_write_image(const unsigned char *datas);
void display_epd_write_white(void);
void display_epd_write_black(void);
void display_deep_sleep(void);


#endif