// ===== 头文件导入 =====
// 下面全部为项目自定义头文件，非标准C库头文件
#include "common.h"   // 自定义，定义全局变量、事件标志、枚举等通用资源
#include "cJSON.h"    // 第三方轻量级JSON解析与生成库
#include "devices.h"  // 自定义，设备控制相关结构和接口声明（灯光、风扇控制等）
#include "sensors.h"  // 自定义，传感器相关结构和接口声明
#include "room.h"     // 自定义，房间状态及偏好控制相关结构和接口

static const char *TAG = "handle_message";  // 日志标识，用于区分打印模块

/**
 * @brief 处理设备状态消息的函数
 * 
 * @param data JSON格式的字符串，包含deviceId和deviceStatus字段
 * 
 * 功能：
 * - 解析JSON字符串
 * - 识别设备ID（灯光或风扇）
 * - 根据设备状态字段控制对应设备开关或风扇档位
 * - 释放JSON对象内存
 */
void handle_device_message(const char *data)
{
    cJSON *json = cJSON_Parse(data); // 解析字符串为JSON对象
    if (json == NULL)
    {
        ESP_LOGE(TAG, "Error parsing device message JSON");
        return;
    }

    // 从JSON对象中获取deviceId和deviceStatus字段
    cJSON *deviceId = cJSON_GetObjectItem(json, "deviceId");
    cJSON *deviceStatus = cJSON_GetObjectItem(json, "deviceStatus");

    // 判断字段是否均为字符串类型
    if (cJSON_IsString(deviceId) && cJSON_IsString(deviceStatus))
    {
        ESP_LOGI(TAG, "Received device message: deviceId=%s, deviceStatus=%s",
                 deviceId->valuestring, deviceStatus->valuestring);

        // 根据deviceId判断具体设备，依次判断灯光1-4和风扇
        if (strcmp(deviceId->valuestring, "light-001") == 0)
        {
            if (strcmp(deviceStatus->valuestring, "on") == 0)
                light_controller_on(device_ctrl.lights, 0);
            else
                light_controller_off(device_ctrl.lights, 0);
        }
        else if (strcmp(deviceId->valuestring, "light-002") == 0)
        {
            if (strcmp(deviceStatus->valuestring, "on") == 0)
                light_controller_on(device_ctrl.lights, 1);
            else
                light_controller_off(device_ctrl.lights, 1);
        }
        else if (strcmp(deviceId->valuestring, "light-003") == 0)
        {
            if (strcmp(deviceStatus->valuestring, "on") == 0)
                light_controller_on(device_ctrl.lights, 2);
            else
                light_controller_off(device_ctrl.lights, 2);
        }
        else if (strcmp(deviceId->valuestring, "light-004") == 0)
        {
            if (strcmp(deviceStatus->valuestring, "on") == 0)
                light_controller_on(device_ctrl.lights, 3);
            else
                light_controller_off(device_ctrl.lights, 3);
        }
        else if (strcmp(deviceId->valuestring, "fan-001") == 0)
        {
            if (strcmp(deviceStatus->valuestring, "on") == 0)
                fan_controller_set_level(device_ctrl.fans, FAN_LOW);
            else
                fan_controller_set_level(device_ctrl.fans, FAN_OFF);
        }
    }

    cJSON_Delete(json); // 释放JSON对象内存
}

/**
 * @brief 处理阈值更新消息的函数
 * 
 * @param data JSON格式的字符串，包含lowThreshold, highThreshold和thresholdId字段
 * 
 * 功能：
 * - 解析JSON字符串
 * - 根据thresholdId判断是温度、湿度还是光照的阈值
 * - 更新对应传感器阈值结构体的上下限值
 * - 释放JSON对象内存
 */
void handle_threshold_message(const char *data)
{
    cJSON *json = cJSON_Parse(data);
    if (json == NULL)
    {
        ESP_LOGE(TAG, "Error parsing threshold message JSON");
        return;
    }

    // 获取低阈值、高阈值和阈值ID
    cJSON *lowThreshold = cJSON_GetObjectItem(json, "lowThreshold");
    cJSON *highThreshold = cJSON_GetObjectItem(json, "highThreshold");
    cJSON *thresholdId = cJSON_GetObjectItem(json, "thresholdId");

    // 确认字段类型正确
    if (cJSON_IsNumber(lowThreshold) && cJSON_IsNumber(highThreshold) && cJSON_IsString(thresholdId))
    {
        ESP_LOGI(TAG, "Received threshold message: thresholdId=%s, lowThreshold=%.2f, highThreshold=%.2f",
                 thresholdId->valuestring, lowThreshold->valuedouble, highThreshold->valuedouble);

        // 根据thresholdId更新对应传感器阈值
        if (strcmp(thresholdId->valuestring, "temperature-001") == 0)
        {
            sensor_ctrl.dht_sensor->tempThreshold.highThresholdValue = highThreshold->valuedouble;
            sensor_ctrl.dht_sensor->tempThreshold.lowThresholdValue = lowThreshold->valuedouble;
        }
        else if (strcmp(thresholdId->valuestring, "humidity-001") == 0)
        {
            sensor_ctrl.dht_sensor->humiThreshold.highThresholdValue = highThreshold->valuedouble;
            sensor_ctrl.dht_sensor->humiThreshold.lowThresholdValue = lowThreshold->valuedouble;
        }
        else if (strcmp(thresholdId->valuestring, "light-001") == 0)
        {
            sensor_ctrl.light_sensor->lightThreshold.highThresholdValue = highThreshold->valuedouble;
            sensor_ctrl.light_sensor->lightThreshold.lowThresholdValue = lowThreshold->valuedouble;
        }
    }

    cJSON_Delete(json);
}

/**
 * @brief 处理用户偏好设置消息的函数
 * 
 * @param data JSON格式字符串，包含preferenceValue和preferenceId字段
 * 
 * 功能：
 * - 解析JSON字符串
 * - 根据preferenceId识别温度、湿度、光照偏好
 * - 更新room模块中的偏好值
 * - 释放JSON对象内存
 */
void handle_preference_message(const char *data)
{
    cJSON *json = cJSON_Parse(data);
    if (json == NULL)
    {
        ESP_LOGE(TAG, "Error parsing preference message JSON");
        return;
    }

    // 获取偏好值和偏好ID
    cJSON *preferenceValue = cJSON_GetObjectItem(json, "preferenceValue");
    cJSON *preferenceId = cJSON_GetObjectItem(json, "preferenceId");

    // 确认字段类型
    if (cJSON_IsNumber(preferenceValue) && cJSON_IsString(preferenceId))
    {
        ESP_LOGI(TAG, "Received preference message: preferenceId=%s, preferenceValue=%.2f",
                 preferenceId->valuestring, preferenceValue->valuedouble);

        // 根据preferenceId更新对应偏好值
        if (strcmp(preferenceId->valuestring, "temperature-001") == 0)
        {
            room.preference.tempPreference = preferenceValue->valuedouble;
        }
        else if (strcmp(preferenceId->valuestring, "humidity-001") == 0)
        {
            room.preference.humiPreference = preferenceValue->valuedouble;
        }
        else if (strcmp(preferenceId->valuestring, "light-001") == 0)
        {
            room.preference.lightPreference = preferenceValue->valuedouble;
        }
    }

    cJSON_Delete(json);
}
