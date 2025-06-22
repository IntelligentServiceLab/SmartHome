// ===== 头文件导入 =====
// 以下头文件均为项目自定义模块，非标准库
#include "wifi_connect.h"  // Wi-Fi连接管理模块，负责Wi-Fi初始化与状态维护
#include "mqtt.h"          // MQTT客户端模块，实现消息发布与订阅
#include "key_driver.h"    // 按键驱动模块，实现按键扫描和事件处理
#include "beep.h"          // 蜂鸣器控制模块，用于发声反馈
#include "sensors.h"       // 传感器管理模块，包含温湿度和光照传感器初始化与数据采集
#include "devices.h"       // 设备控制模块，封装灯光和风扇控制接口
#include "room.h"          // 房间模式管理模块，实现HOME/AWAY/SLEEP模式切换及状态维护
#include "su03t.h"         // 语音识别模块，串口通信与语音指令解析
#include "ssd1306.h"       // OLED显示驱动模块，用于显示状态和数据

// ===== 全局变量 =====
EventGroupHandle_t event_group = NULL; // FreeRTOS事件组句柄，用于跨模块事件同步

// 日志TAG，用于区分日志输出模块
#define TAG "main"

/**
 * @brief 应用程序主入口
 *
 * 负责系统各模块的初始化及启动，流程如下：
 * 1. 创建事件组用于事件同步
 * 2. 初始化Wi-Fi连接
 * 3. 初始化按键扫描
 * 4. 初始化蜂鸣器
 * 5. 初始化传感器模块
 * 6. 初始化设备控制模块
 * 7. 初始化房间模式管理模块
 * 8. 初始化语音识别模块
 * 9. 初始化OLED显示模块
 * 10. 阻塞等待Wi-Fi连接成功事件
 * 11. 初始化并启动MQTT客户端连接
 */
void app_main()
{
    ESP_LOGI(TAG, "app Starting");

    // 创建FreeRTOS事件组，用于模块间的事件通知与同步
    event_group = xEventGroupCreate();

    // 初始化Wi-Fi，启动Station模式连接指定路由器
    wifi_init_sta();

    // 初始化按键驱动，开启按键扫描任务，处理用户输入事件
    keys_init();

    // 初始化蜂鸣器GPIO及控制逻辑，用于反馈提示音
    buzzer_init();

    // 初始化传感器模块，创建数据采集任务（如DHT11/DHT22和光照传感器）
    sensors_init();

    // 初始化设备控制模块，包括灯光、风扇等设备驱动接口
    devices_init();

    // 初始化房间模式管理模块，负责不同模式间的切换逻辑及状态维护
    room_init();

    // 初始化语音识别模块，配置串口通信及语音指令解析回调
    su03t_init();

    // 初始化OLED显示屏，负责显示模式状态及传感器数据
    ssd1306_init();

    // 阻塞等待Wi-Fi连接成功事件标志被置位
    // 1. WIFI_CONNECTED_BIT为等待的事件标志位
    // 2. pdFALSE 表示不清除该标志位
    // 3. pdTRUE 表示等待所有标志位集合（这里只有一个标志）
    // 4. portMAX_DELAY 永久等待直到事件发生
    xEventGroupWaitBits(event_group, WIFI_CONNECTED_BIT,
                        pdFALSE, pdTRUE, portMAX_DELAY);

    // Wi-Fi连接成功后初始化MQTT客户端，建立与Broker的连接
    mqtt_init();
}
