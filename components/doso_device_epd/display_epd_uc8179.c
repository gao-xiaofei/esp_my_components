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


void display_epd_init_gray4(void) 
{		
    DISPLAY_RST_0;  // Module reset   
    delay_xms(10);  // At least 10ms delay 
    DISPLAY_RST_1;
    delay_xms(10);  // At least 10ms delay 
  
    spi_write_cmd(0X00);			// PANNEL SETTING
    spi_write_data(0x1F);   // KW-3f   KWR-2F	BWROTP 0f	BWOTP 1f

    spi_write_cmd(0X50);			// VCOM AND DATA INTERVAL SETTING
    spi_write_data(0x10);
    spi_write_data(0x07);

    spi_write_cmd(0x04);  // POWER ON
    delay_xms(100);  
    lcd_chkstatus();        // waiting for the electronic paper IC to release the idle signal

    // Enhanced display drive(Add 0x06 command)
    spi_write_cmd(0x06);			// Booster Soft Start 
    spi_write_data (0x27);
    spi_write_data (0x27);   
    spi_write_data (0x18);		
    spi_write_data (0x17);		

    spi_write_cmd(0xE0);
    spi_write_data(0x02);
    spi_write_cmd(0xE5);
    spi_write_data(0x5F);  // 0x5A--1.5s, 0x5F--4 Gray		
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

void display_epd_write_image_gray4(const unsigned char *datas)
{
    unsigned int i, j, k;
    unsigned char temp1, temp2, temp3;

    // old data
    spi_write_cmd(0x10);	       

    for(i = 0; i < 48000; i++)	               // 48000*2  800*480
    { 
        temp3 = 0;
        for(j = 0; j < 2; j++)	
        {
            temp1 = datas[i * 2 + j];
            for(k = 0; k < 4; k++)
            {
                temp2 = temp1 & 0xC0 ;
                if(temp2 == 0xC0)
                    temp3 |= 0x01;  // white
                else if(temp2 == 0x00)
                    temp3 |= 0x00;  // black
                else if((temp2 >= 0x80) && (temp2 < 0xC0)) 
                    temp3 |= 0x00;  // gray1
                else if(temp2 == 0x40)
                    temp3 |= 0x01;  // gray2
                if((j == 0 && k <= 3) || (j == 1 && k <= 2))
                {
                    temp3 <<= 1;	
                    temp1 <<= 2;
                }
            }
        }	
        spi_write_data(~temp3);			
    }

    // new data
    spi_write_cmd(0x13);	       
    for(i = 0; i < 48000 * 2; i++)	               // 48000*2   800*480
    { 
        temp3 = 0;
        for(j = 0; j < 2; j++)	
        {
            temp1 = datas[i * 2 + j];
            for(k = 0; k < 4; k++)
            {
                temp2 = temp1 & 0xC0 ;
                if(temp2 == 0xC0)
                    temp3 |= 0x01;  // white
                else if(temp2 == 0x00)
                    temp3 |= 0x00;  // black
                else if((temp2 >= 0x80) && (temp2 < 0xC0)) 
                    temp3 |= 0x01;  // gray1
                else if(temp2 == 0x40)
                    temp3 |= 0x00;  // gray2
                if((j == 0 && k <= 3) || (j == 1 && k <= 2))
                {
                    temp3 <<= 1;	
                    temp1 <<= 2;
                }
            }	
        }
        spi_write_data(~temp3);			
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
