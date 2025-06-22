// dht_sensor.h

#ifndef _DHT_SENSOR_H_
#define _DHT_SENSOR_H_
#include "common.h"
#include "driver/gpio.h"
#include "dht.h"

// DHT 数据结构体
typedef struct
{
    float temp;
    float humi;
} DHTData;

// DHT 控制器结构体
typedef struct
{
    gpio_num_t gpio;         // 数据引脚
    dht_sensor_type_t type;  // DHT11 / DHT22
    DHTData data;            // 当前温湿度数据
    Threshold tempThreshold; // 温度阈值
    Threshold humiThreshold; // 湿度阈值
} DHTSensor;

void dht_sensor_init(DHTSensor *dht_sensor, gpio_num_t gpio, dht_sensor_type_t type);
void dht_sensor_task(void *param);

#endif
