#pragma once

#include "light_sensor.h"
#include "dht_sensor.h"

// 房间内所有传感器的集合
typedef struct
{
    DHTSensor *dht_sensor;
    LightSensor *light_sensor;
} SensorController;

void sensors_init();

extern SensorController sensor_ctrl;