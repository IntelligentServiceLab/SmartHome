#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/ringbuf.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_system.h>
#include <esp_event.h>
#include <mqtt_client.h>

#define WIFI_SSID "Xiaomi Civi 3" // WIFI名称
#define WIFI_PASS "00000000"      // WIFI密码

#define MQTT_BROKER_URI "mqtt://10.152.93.96:1883" // MQTT服务器地址
#define MQTT_USER "esp32"
#define MQTT_PASSWORD "123456"

#define WIFI_CONNECTED_BIT BIT0 // WIFI事件位
#define MQTT_CONNECTED_BIT BIT1 // MQTT事件位

extern EventGroupHandle_t event_group;       // 事件组句柄
extern esp_mqtt_client_handle_t mqtt_client; // MQTT 客户端句柄

// 系统模式定义
typedef enum
{
    MODE_HOME, // 回家模式
    MODE_AWAY, // 离家模式
    MODE_SLEEP // 睡觉模式
} SystemMode;

// 控制模式定义（自动 or 手动）
typedef enum
{
    CONTROL_AUTO,  // 自动控制（由传感器或流程决定）
    CONTROL_MANUAL // 手动控制（用户直接操作）
} ControlMode;

// 阈值定义
typedef struct
{
    float lowThresholdValue;
    float highThresholdValue;
} Threshold;

#endif