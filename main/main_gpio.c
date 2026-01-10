#include <string.h>
#include <math.h>
#include <errno.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "doso_gpio_user.h"

#define MAIN_TAG            "main"

uint32_t g_interrupt = 0;


static void IRAM_ATTR s_gpio_isr_handler(void* arg)
{
	g_interrupt++;
}


struct DosoGpioUser gpio_interrupt = {
    .gpio_pin = GPIO_NUM_45,
    .gpio_mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE, 
    .intr_enable = 1,
    .intr_type = GPIO_INTR_NEGEDGE
};

struct DosoGpioUser gpio_output = {
    .gpio_pin = GPIO_NUM_46,
    .gpio_mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE, 
    .intr_enable = 0,
    .intr_type = GPIO_INTR_DISABLE
};


static void gpio_task(void *pvParameters)
{
    uint8_t state = 0;
    doso_gpio_user_init(&gpio_interrupt,s_gpio_isr_handler);
    doso_gpio_user_init(&gpio_output,NULL);
    while (1)
    {
        printf("gpio_task running,g_interrupt value:%ld\n", g_interrupt);
        state = !state;
        gpio_output.set_state(&gpio_output, state);
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }

    vTaskDelete(NULL);
}


void thread_init()
{
    TaskHandle_t xHandle1 = NULL;
    xTaskCreatePinnedToCore( gpio_task, "gpio_task", 4096, NULL, 3, &xHandle1, 0 );
    configASSERT( xHandle1 );

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