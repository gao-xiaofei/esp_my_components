#include <string.h>
#include <math.h>
#include <errno.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "doso_wifi_sta.h"


#define MAIN_TAG            "main"



static void wifi_task(void *pvParameters)
{

    wifi_init_sta((uint8_t*)"gao",(uint8_t*)"12345806");

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("wifi sta test running\n");
    }

    vTaskDelete(NULL);
}


void thread_init()
{
    TaskHandle_t xHandle1 = NULL;
    xTaskCreatePinnedToCore( wifi_task, "wifi_task", 4096, NULL, 3, &xHandle1, 0 );
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