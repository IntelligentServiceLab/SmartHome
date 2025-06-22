#pragma once
#include "device_light.h"
#include "device_fan.h"

// 房间内所有设备的集合
typedef struct
{
    LightController *lights; // 灯光
    FanController *fans;     // 风扇
} DeviceController;

void devices_init(void);

extern DeviceController device_ctrl;