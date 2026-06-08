//
// Created by gao on 26-6-6.
//

#include "doso_wifi_sta.h"

#include <string.h>
#include "esp_wifi.h"
// #include "esp_system.h"
// #include "esp_wifi.h"
// #include "esp_event.h"
// #include "esp_log.h"
// #include "nvs_flash.h"

// #include "lwip/err.h"
// #include "lwip/sys.h"


static struct DosoWifiUser s_wifi_user = {0};


static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        s_wifi_user.is_connected = 0;
        s_wifi_user.is_farst_init = 1;
        s_wifi_user.is_get_ip = 0;
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        s_wifi_user.ip_addr[0] = esp_ip4_addr_get_byte(&event->ip_info.ip, 0);
        s_wifi_user.ip_addr[1] = esp_ip4_addr_get_byte(&event->ip_info.ip, 1);
        s_wifi_user.ip_addr[2] = esp_ip4_addr_get_byte(&event->ip_info.ip, 2);
        s_wifi_user.ip_addr[3] = esp_ip4_addr_get_byte(&event->ip_info.ip, 3);
        printf("got ip:" IPSTR "\n", s_wifi_user.ip_addr[0],s_wifi_user.ip_addr[1],s_wifi_user.ip_addr[2],s_wifi_user.ip_addr[3]);
        s_wifi_user.retry_num = 0;
        s_wifi_user.is_get_ip = 1;
        s_wifi_user.is_connected = 1;
        s_wifi_user.is_farst_init = 0;
    }
}


void doso_wifi_init_sta(struct DosoWifiUser* _wifi_info)
{

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (password len => 8).
             * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
             * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
             * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
             */
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            .sae_h2e_identifier = "doso_wifi",
        },
    };
    if(_wifi_info->is_open)
    {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
    }
    memcpy(wifi_config.sta.ssid,_wifi_info->ssid,strlen((char*)_wifi_info->ssid));
    memcpy(wifi_config.sta.password,_wifi_info->password,strlen((char*)_wifi_info->password));
    
    memcpy(s_wifi_user.ssid,_wifi_info->ssid,strlen((char*)_wifi_info->ssid));
    memcpy(s_wifi_user.password,_wifi_info->password,strlen((char*)_wifi_info->password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

}

void doso_wifi_connect_retry(void)
{
    if(s_wifi_user.is_farst_init == 1)
    {
        esp_wifi_connect();
        s_wifi_user.is_farst_init = 0;
        s_wifi_user.retry_num++;
    }
}


void doso_wifi_get_status(struct DosoWifiUser* _wifi_info)
{
    _wifi_info->is_connected = s_wifi_user.is_connected;
    _wifi_info->retry_num = s_wifi_user.retry_num;
    memcpy(_wifi_info->ip_addr,s_wifi_user.ip_addr,4);
}












