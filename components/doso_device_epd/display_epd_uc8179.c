#include "display_epd_uc8179.h"

#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


// Delay Functions (ESP-IDF version)
void delay_xms(unsigned int xms)
{
    vTaskDelay(pdMS_TO_TICKS(xms));
}



// Busy function
static void lcd_chkstatus(void)
{
    while(1)
    {	 // =0 BUSY
        if(IS_DISPLAY_BUSY == 1) break;
        vTaskDelay(pdMS_TO_TICKS(10));  // Avoid watchdog timeout
    }  
}

// Full screen update update function
static void EPD_Update(void)
{   
    // update
    spi_write_cmd(0x12);		// DISPLAY update 	
    delay_xms(1);	            // !!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();          // waiting for the electronic paper IC to release the idle signal
}

// Full screen update initialization
void display_epd_init(void)
{
    // Module reset 
    DISPLAY_RST_0;            
    delay_xms(10);              // At least 10ms delay 
    DISPLAY_RST_1;
    delay_xms(10);              // At least 10ms delay 
    
    // POWER SETTING
    spi_write_cmd(0x01);			
    spi_write_data (0x07);
    spi_write_data (0x07);      // VGH=20V,VGL=-20V
    spi_write_data (0x3f);		// VDH=15V
    spi_write_data (0x3f);		// VDL=-15V

    // Enhanced display drive(Add 0x06 command)
    spi_write_cmd(0x06);	    // Booster Soft Start 
    spi_write_data (0x17);
    spi_write_data (0x17);   
    spi_write_data (0x28);		
    spi_write_data (0x17);	

    // POWER ON
    spi_write_cmd(0x04);  
    delay_xms(100);  
    lcd_chkstatus();            // waiting for the electronic paper IC to release the idle signal

    // PANNEL SETTING
    spi_write_cmd(0X00);	
#if(PANNEL_ANGLE == 180)
    spi_write_data(0x13);       // KW-3f   KWR-2F	BWROTP 0f	BWOTP 1f
#else
    spi_write_data(0x1F);       // KW-3f   KWR-2F	BWROTP 0f	BWOTP 1f
#endif

    // tres	
    spi_write_cmd(0x61);        			
    spi_write_data (0x03);		// source 800
    spi_write_data (0x20);
    spi_write_data (0x01);		// gate 480
    spi_write_data (0xE0);  

    spi_write_cmd(0X15);		
    spi_write_data(0x00);		

    spi_write_cmd(0X50);        // VCOM AND DATA INTERVAL SETTING
    spi_write_data(0x10);
    spi_write_data(0x07);

    spi_write_cmd(0X60);	    // TCON SETTING
    spi_write_data(0x22);
}





// Full screen update display function
void display_epd_write_image(const unsigned char *datas)
{
    unsigned int i;	
    spi_write_cmd(0x10);  // write old data 
    for(i = 0; i < EPD_ARRAY_SIZE; i++)
    {               
        spi_write_data(0x00);
    }
    spi_write_cmd(0x13);  // write new data 
    for(i = 0; i < EPD_ARRAY_SIZE; i++)
    {               
        spi_write_data(datas[i]);
    }	 
    EPD_Update();	 
}



// Clear screen display
void display_epd_write_white(void)
{
    unsigned int i;
    // Write Data
    spi_write_cmd(0x10);	     
    for(i = 0; i < EPD_ARRAY_SIZE; i++)	     
    {
        spi_write_data(0x00);  
    }
    spi_write_cmd(0x13);	     
    for(i = 0; i < EPD_ARRAY_SIZE; i++)	     
    {
        spi_write_data(0x00);  
    }
    EPD_Update();	 
}



// Display all black
void display_epd_write_black(void)
{
    unsigned int i;
    // Write Data
    spi_write_cmd(0x10);	     
    for(i = 0; i < EPD_ARRAY_SIZE; i++)	     
    {
        spi_write_data(0x00);  
    }
    spi_write_cmd(0x13);	     
    for(i = 0; i < EPD_ARRAY_SIZE; i++)	     
    {
        spi_write_data(0xff);  
    }
    EPD_Update();	
}


// Deep sleep function
void display_deep_sleep(void)
{  	
    // VCOM AND DATA INTERVAL SETTING	
    spi_write_cmd(0X50);  		
    spi_write_data(0xf7); // WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7	

    // power off
    spi_write_cmd(0X02);  	
    lcd_chkstatus();          // waiting for the electronic paper IC to release the idle signal 	
    
    // deep sleep
    spi_write_cmd(0X07);  	
    spi_write_data(0xA5);
}



// GUI display
void EPD_Display(unsigned char *Image)
{
    unsigned int i;	
    spi_write_cmd(0x10);  // write old data 
    for(i = 0; i < EPD_ARRAY_SIZE; i++)
    {               
        spi_write_data(0x00);
    }
    spi_write_cmd(0x13);  // write new data 
    for(i = 0; i < EPD_ARRAY_SIZE; i++)
    {               
        spi_write_data(~Image[i]);
    }	 
    EPD_Update();			 
}
