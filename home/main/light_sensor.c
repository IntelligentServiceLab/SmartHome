// light_sensor.c
// 光照传感器驱动及任务实现
// 采集光照强度，通过ADC读取，显示到OLED屏幕，并通过MQTT发布数据

// === 头文件导入 ===
// 自定义头文件
#include "light_sensor.h"   // 光照传感器数据结构及接口声明
// 第三方JSON库（标准库）
#include "cJSON.h"          // JSON处理库，用于组装发送数据
// 自定义OLED驱动
#include "ssd1306.h"        // OLED显示驱动接口

#define TAG "LIGHT_SENSOR"  // 日志打印标签

/**
 * @brief 初始化光照传感器
 * @param sensor 指向LightSensor结构体指针
 * @param channel ADC1通道，指定传感器连接的ADC通道
 * @details 配置ADC分辨率及衰减，初始化阈值，准备采样
 */
void light_sensor_init(LightSensor *sensor, adc1_channel_t channel)
{
    sensor->adc_channel = channel;       // 记录ADC通道
    sensor->data.light = 0;              // 初始光照值为0
    sensor->lightThreshold.highThresholdValue = 100;  // 亮度高阈值（百分比）
    sensor->lightThreshold.lowThresholdValue = 0;     // 亮度低阈值（百分比）

    adc1_config_width(ADC_WIDTH_BIT_12);                 // 配置ADC为12位精度（0~4095）
    adc1_config_channel_atten(channel, ADC_ATTEN_DB_12); // 配置ADC衰减，允许采样0~3.3V范围

    ESP_LOGI(TAG, "Light sensor initialized on ADC1 channel %d", channel);
}

/**
 * @brief 光照传感器采集任务
 * @param param 传入LightSensor结构体指针
 * @details 周期性读取ADC数据，转换成百分比光照强度，
 *          显示到OLED屏幕指定位置，组装JSON并通过MQTT发布数据
 */
void light_sensor_task(void *param)
{
    char light_buf[128];
    memset(light_buf, 0, sizeof(light_buf)); // 清空字符串缓冲区

    LightSensor *sensor = (LightSensor *)param;
    while (1)
    {
        int raw = adc1_get_raw(sensor->adc_channel);  // 读取ADC原始值，范围0~4095
        // 将ADC值映射为光照百分比（反比，数值越大光线越暗）
        int light = 100 - (raw * 100) / 4095;
        sensor->data.light = light;  // 保存当前光照数据

        ESP_LOGI(TAG, "Light Intensity: %d%%", light);

        // OLED显示光照强度信息，行号4对应Y轴位置
        sprintf(light_buf, "Light: %d%%", light);
        ssd1306_draw_string8x16(0, 4, light_buf);

        // 创建JSON对象，用于发布光照传感器数据
        cJSON *json = cJSON_CreateObject();
        if (json)
        {
            cJSON_AddStringToObject(json, "sensorType", "light");  // 传感器类型字段

            char value_str[8];
            snprintf(value_str, sizeof(value_str), "%d", light);  // 数值转字符串
            cJSON_AddStringToObject(json, "sensorValue", value_str); // 传感器数值字段

            cJSON_AddStringToObject(json, "roomId", "room-001");   // 采集房间ID

            // 将JSON对象打印成字符串（无格式）
            char *json_string = cJSON_PrintUnformatted(json);
            if (json_string)
            {
                // 通过MQTT客户端发布到主题 "sensor/data"，QOS=1，非保留消息
                esp_mqtt_client_publish(mqtt_client, "sensor/data", json_string, 0, 1, 0);
                free(json_string);
            }

            cJSON_Delete(json);  // 释放JSON对象内存
        }

        vTaskDelay(pdMS_TO_TICKS(2000));  // 延时2秒，周期采样
    }
}
