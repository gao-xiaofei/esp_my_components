#include <string.h>
#include <math.h>
#include <errno.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"

#include "doso_wifi_sta.h"


#define MAIN_TAG            "main"
#define HOST_IP_ADDR        "255.255.255.255"
#define UDP_SEND_PORT       28828
#define UDP_RECEIVE_PORT    8315
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


static void udp_beat_task(void *pvParameters)
{
    int fd_sock = -1;
    struct sockaddr_in dest_addr;
    struct sockaddr_in listen_addr;
    uint8_t output_buf[512] = {0};
    uint16_t output_len = 0;

    dest_addr.sin_addr.s_addr = inet_addr("121.41.76.83");
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(UDP_SEND_PORT);
    while (1)
    {
        if (wifi_user_info_.is_connected == 0) {
            if (fd_sock >= 0) {
                close(fd_sock);
                fd_sock = -1;
            }
            printf("wifi is disconnect!!!\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        if (fd_sock < 0) {
            fd_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
            if (fd_sock < 0) {
                printf("Unable to create socket: errno %d\n", errno);
                continue;
            }

            // 设置超时
            struct timeval tv;
            tv.tv_sec = 1;  // 秒
            tv.tv_usec = 0; // 微秒
            if (setsockopt(fd_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
                printf("Failed to set socket timeout\n");
            }

            memset(&listen_addr, 0, sizeof(listen_addr));
            listen_addr.sin_family = AF_INET;
            listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
            listen_addr.sin_port = htons(UDP_RECEIVE_PORT);

            if (bind(fd_sock, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
                printf("Socket bind failed: errno %d\n", errno);
                close(fd_sock);
                fd_sock = -1;
                continue;
            }

            printf("Socket created successfully, listening on port %d\n", UDP_RECEIVE_PORT);
        }

        output_buf[0] = 'a';
        output_buf[1] = '8';
        output_buf[2] = '8';
        output_buf[3] = '2';
        output_buf[4] = '8';
        output_len = 5;
        if(fd_sock >= 0)
        {
            int err = sendto(fd_sock, output_buf, output_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if (err < 0) {
                printf( "Error occurred during sending: errno %d\n", errno);
            }
        }
        struct sockaddr_in source_addr;
        socklen_t socklen = sizeof(source_addr);
        uint8_t rx_buffer[512];
        ssize_t len = recvfrom(fd_sock, rx_buffer, sizeof(rx_buffer), 0, 
                            (struct sockaddr *)&source_addr, &socklen);
        
        if (len < 0) {
            
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            
            printf("recvfrom failed: errno %d, rebuilding socket\n", errno);
            close(fd_sock);
            fd_sock = -1;
            continue;
        }
        printf("data:%c\n", rx_buffer[0]);
        if (len < 4) {
            printf("Received packet too short: %zd bytes\n", len);
            // continue;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void thread_init()
{
    TaskHandle_t xHandle1 = NULL;
    xTaskCreatePinnedToCore( wifi_task, "wifi_task", 4096, NULL, 3, &xHandle1, 0 );
    configASSERT( xHandle1 );

    TaskHandle_t xHandle2 = NULL;
    xTaskCreatePinnedToCore( udp_beat_task, "udp_beat_task", 4096, NULL, 3, &xHandle2, 0 );
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