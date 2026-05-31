#include <string.h>
#include <math.h>
#include <errno.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "doso_uart_user.h"
#include "display_epd_uc8179.h"


#define MAIN_TAG            "main"


struct DosoUartUser uart1_user_ = {
    .uart_num = UART_NUM_1,
    .baud_rate = 460800,
    .tx_pin = GPIO_NUM_43,
    .rx_pin = GPIO_NUM_44,
    .rx_timeout_ms = 1000,
    .rx_buffer_size = 512,
    .tx_buffer_size = 512
};

static uint8_t image_is_update = 0;
static uint8_t image_data[EPD_ARRAY_SIZE] = {0};


static void uart_task(void *pvParameters)
{
    uint8_t uart_recv_buffer[256] = {0};
    uint16_t recv_num = 0;

    doso_uart_init(&uart1_user_);

    while (1)
    {
        memset( uart1_user_.rx_buffer, 0, 160);
        
        uart1_user_.read(&uart1_user_, 160);
        recv_num = uart1_user_.receive_len;
        if( recv_num <= 0) {
            printf("uart timeout! read-len %d:  ", recv_num);
            for(int i = 0; i < 3; i++) {
                printf("%02x ", uart_recv_buffer[i]);
            }
            printf("----------------------------\n");
           continue;
       }
       memcpy(uart_recv_buffer, uart1_user_.rx_buffer, 160);


       // uart 发送
       memcpy(uart1_user_.tx_buffer, uart_recv_buffer, 160);
       uart1_user_.write(&uart1_user_, 160);
    }

    vTaskDelete(NULL);
}

static void display_task(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(100));
    spi_gpio_init();
    vTaskDelay(pdMS_TO_TICKS(10000));
    image_is_update = 1;

    while (1)
    {
        for(int i = 0;i<EPD_ARRAY_SIZE;i++)
        {
            image_data[i] = 0x95;
        }
        if(image_is_update == 1) {
            image_is_update = 0;
            // 全屏刷新方式初始化
            display_epd_init();
            // 全局图像刷新
            display_epd_write_image(image_data); 
            // 开启低功耗模式
            display_deep_sleep();
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);
}

void thread_init()
{
    TaskHandle_t xHandle1 = NULL;
    xTaskCreatePinnedToCore( uart_task, "uart_task", 4096, NULL, 3, &xHandle1, 0 );
    configASSERT( xHandle1 );

    TaskHandle_t xHandle2 = NULL;
    xTaskCreatePinnedToCore( display_task, "display_task", 4096, NULL, 2, &xHandle2, 0 );
    configASSERT( xHandle2 );
}

void app_main(void)
{
    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if  (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    thread_init();
    

}