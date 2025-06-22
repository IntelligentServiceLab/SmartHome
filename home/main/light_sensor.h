// light_sensor.h

#ifndef _LIGHT_SENSOR_H_
#define _LIGHT_SENSOR_H_

#include "common.h"
#include "driver/adc.h"

// 光照数据结构体
typedef struct
{
    int light; // 光照强度百分比（0~100）
} LightData;

// 光照传感器控制器结构体
typedef struct
{
    adc1_channel_t adc_channel; // ADC 通道
    LightData data;             // 当前光照数据
    Threshold lightThreshold;        // 光照阈值
} LightSensor;

void light_sensor_init(LightSensor *sensor, adc1_channel_t channel);
void light_sensor_task(void *param);

#endif
