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

static struct DosoWifiUser wifi_user_info_ = {
    .ssid = "gao",
    .password = "",
    .is_open = 1
};

static void wifi_task(void *pvParameters)
{

    doso_wifi_init_sta(&wifi_user_info_);

    while (1)
    {
        doso_wifi_get_status(&wifi_user_info_);
        if(!wifi_user_info_.is_connected){
            doso_wifi_connect_retry();
            printf("wifi retry:%ld\n",wifi_user_info_.retry_num);
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        
        printf("wifi status:%d ip:%d.%d.%d.%d\n", wifi_user_info_.is_connected, wifi_user_info_.ip_addr[0], wifi_user_info_.ip_addr[1], wifi_user_info_.ip_addr[2], wifi_user_info_.ip_addr[3]);
        printf("wifi sta test running\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
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