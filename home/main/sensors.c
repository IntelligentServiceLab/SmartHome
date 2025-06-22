// sensors.c
// 传感器模块实现
// 负责初始化环境传感器（温湿度传感器、光敏传感器）
// 并启动相应的传感器任务进行数据采集与显示

// === 头文件导入 ===
// 自定义头文件
#include "sensors.h"   // 传感器模块声明及数据结构定义
#include "ssd1306.h"   // OLED 显示驱动，显示传感器信息

// === 宏定义及全局变量 ===

// 温湿度传感器相关定义
#define DHT_GPIO 1                   // DHT传感器数据引脚GPIO编号
#define DHT_TYPE_USED DHT_TYPE_DHT11 // 选用的DHT传感器型号，可选 DHT11 或 DHT22
DHTSensor dht_sensor;                // 温湿度传感器实例

// 光敏传感器相关定义
#define LIGHT_SENSOR_ADC_CHANNEL ADC1_CHANNEL_1 // 光敏传感器对应ADC1通道1（GPIO2）
LightSensor light_sensor;                      // 光敏传感器实例

// 传感器控制器集合，包含所有传感器指针
SensorController sensor_ctrl;

/**
 * @brief 传感器初始化函数
 * @details 初始化温湿度传感器和光敏传感器，
 *          将传感器实例赋值给传感器控制器，
 *          并创建相关任务进行周期性数据采集和处理。
 */
void sensors_init()
{
    // 初始化温湿度传感器，指定GPIO和传感器类型
    dht_sensor_init(&dht_sensor, DHT_GPIO, DHT_TYPE_USED);

    // 初始化光敏传感器，指定ADC通道
    light_sensor_init(&light_sensor, LIGHT_SENSOR_ADC_CHANNEL);

    // 将传感器实例指针赋值给传感器控制器，方便统一管理
    sensor_ctrl.dht_sensor = &dht_sensor;
    sensor_ctrl.light_sensor = &light_sensor;

    // 创建温湿度传感器任务，优先级5，堆栈大小4096字节
    xTaskCreate(dht_sensor_task, "dht_sensor_task", 4096, sensor_ctrl.dht_sensor, 5, NULL);

    // 创建光敏传感器任务，优先级5，堆栈大小4096字节
    xTaskCreate(light_sensor_task, "light_sensor_task", 4096, sensor_ctrl.light_sensor, 5, NULL);

    // 创建显示任务，负责OLED屏幕显示传感器数据，优先级5，堆栈大小4096字节
    xTaskCreate(show_task, "show_task", 4096, NULL, 5, NULL);
}
