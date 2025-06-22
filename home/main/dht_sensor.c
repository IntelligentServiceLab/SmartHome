// ===== 头文件导入 =====
// 下面全部为项目自定义或第三方库头文件，非标准C库头文件
#include "dht_sensor.h"  // 自定义DHT传感器驱动接口和结构定义
#include "cJSON.h"       // 第三方轻量级JSON解析和生成库，方便构造JSON格式消息
#include "beep.h"        // 自定义蜂鸣器控制接口
#include "ssd1306.h"     // 自定义OLED显示驱动接口，用于屏幕显示

#define TAG "DHT_SENSOR" // 日志打印TAG，定位日志来源为DHT传感器模块

/**
 * @brief 初始化DHT传感器结构体，配置GPIO和传感器类型，并设置温湿度阈值
 * 
 * @param dht_sensor 指向DHTSensor结构体指针
 * @param gpio       传感器数据引脚GPIO号
 * @param type       传感器类型（DHT11或DHT22）
 * 
 * 功能：
 * - 设置传感器GPIO为输入模式
 * - 初始化温度高低阈值，湿度高低阈值
 * - 打印初始化完成日志
 */
void dht_sensor_init(DHTSensor *dht_sensor, gpio_num_t gpio, dht_sensor_type_t type)
{
    dht_sensor->gpio = gpio;
    dht_sensor->type = type;
    dht_sensor->tempThreshold.highThresholdValue = 40;  // 最高温度报警阈值40°C
    dht_sensor->tempThreshold.lowThresholdValue = 0;    // 最低温度阈值0°C（未使用）
    dht_sensor->humiThreshold.highThresholdValue = 100; // 最高湿度阈值100%（未使用）
    dht_sensor->humiThreshold.lowThresholdValue = 0;    // 最低湿度阈值0%（未使用）
    gpio_set_direction(gpio, GPIO_MODE_INPUT);          // 设置GPIO为输入模式，读取传感器数据
    ESP_LOGI(TAG, "DHT sensor initialization completed");
}

/**
 * @brief DHT传感器任务函数，用于周期性读取数据、显示和上传MQTT
 * 
 * @param param 传入DHTSensor结构体指针
 * 
 * 功能：
 * - 循环执行：
 *   - 读取DHT温湿度数据（float类型）
 *   - 在OLED屏幕显示温度和湿度
 *   - 生成JSON格式温度和湿度消息，分别发布到MQTT主题"sensor/data"
 *   - 根据温度阈值控制蜂鸣器报警
 * - 读取失败时输出警告日志
 * - 任务延时2秒，避免过于频繁读取
 */
void dht_sensor_task(void *param)
{
    char display_buf[128];                      // OLED显示缓冲区
    memset(display_buf, 0, sizeof(display_buf)); // 清空缓冲区

    DHTSensor *dht_sensor = (DHTSensor *)param; // 参数强转为DHTSensor结构体指针
    while (1)
    {
        // 读取温湿度数据到结构体成员
        if (dht_read_float_data(dht_sensor->type, dht_sensor->gpio, &dht_sensor->data.humi, &dht_sensor->data.temp) == ESP_OK)
        {
            ESP_LOGI(TAG, "Temp: %.1f°C, Humidity: %.1f%%", dht_sensor->data.temp, dht_sensor->data.humi);

            // OLED显示温度
            sprintf(display_buf, "Temp: %.1fC", dht_sensor->data.temp);
            ssd1306_draw_string8x16(0, 0, display_buf);

            // OLED显示湿度
            sprintf(display_buf, "Humi: %.1f%%", dht_sensor->data.humi);
            ssd1306_draw_string8x16(0, 2, display_buf);

            // 构建并发布温度JSON消息
            cJSON *temp_json = cJSON_CreateObject();
            if (temp_json)
            {
                cJSON_AddStringToObject(temp_json, "sensorType", "temperature");

                char temp_str[8];
                snprintf(temp_str, sizeof(temp_str), "%.1f", dht_sensor->data.temp);
                cJSON_AddStringToObject(temp_json, "sensorValue", temp_str);

                cJSON_AddStringToObject(temp_json, "roomId", "room-001");

                char *temp_msg = cJSON_PrintUnformatted(temp_json);
                if (temp_msg)
                {
                    esp_mqtt_client_publish(mqtt_client, "sensor/data", temp_msg, 0, 1, 0);
                    free(temp_msg);
                }

                cJSON_Delete(temp_json);
            }

            // 构建并发布湿度JSON消息
            cJSON *humi_json = cJSON_CreateObject();
            if (humi_json)
            {
                cJSON_AddStringToObject(humi_json, "sensorType", "humidity");

                char humi_str[8];
                snprintf(humi_str, sizeof(humi_str), "%.1f", dht_sensor->data.humi);
                cJSON_AddStringToObject(humi_json, "sensorValue", humi_str);

                cJSON_AddStringToObject(humi_json, "roomId", "room-001");

                char *humi_msg = cJSON_PrintUnformatted(humi_json);
                if (humi_msg)
                {
                    esp_mqtt_client_publish(mqtt_client, "sensor/data", humi_msg, 0, 1, 0);
                    free(humi_msg);
                }

                cJSON_Delete(humi_json);
            }

            // 温控报警逻辑：温度超过阈值，蜂鸣器响；否则关闭蜂鸣器
            if (dht_sensor->data.temp > dht_sensor->tempThreshold.highThresholdValue)
            {
                buzzer_on();
            }
            else
            {
                buzzer_off();
            }
        }
        else
        {
            ESP_LOGW(TAG, "Failed to read DHT11");
        }

        // 延时2秒，避免读取过频繁
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
