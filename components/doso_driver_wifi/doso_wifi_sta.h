//
// Created by gao on 26-6-6.
//

#ifndef ESP_WIFI_USER_H
#define ESP_WIFI_USER_H

#include <stdint.h>


struct DosoWifiUser {
    // 设置信息
    uint8_t ssid[32];
    uint8_t password[64]; 
    uint8_t is_open;        // 是否有密码，0：有，1：没有
    uint8_t is_farst_init;
    
    // 反馈信息
    uint8_t ip_addr[4];
    uint8_t is_connected;   // 是否连接成功，0：未连接，1：已连接
    uint8_t is_get_ip;      // 是否获取到IP，0：未获取，1：已获取
    uint8_t reserve;
    uint32_t retry_num;     // 重试次数
};


void doso_wifi_init_sta(struct DosoWifiUser* _wifi_info);

void doso_wifi_connect_retry(void);

void doso_wifi_get_status(struct DosoWifiUser* _wifi_info);

#endif //ESP_UART_USER_H
