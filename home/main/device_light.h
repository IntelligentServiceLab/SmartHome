#pragma once

#include "common.h"
#include "driver/gpio.h"

// 灯光状态结构体
typedef struct
{
    ControlMode mode; // 控制模式：自动 / 手动
    bool is_on;       // 是否亮灯
} LightState;

// 灯光控制器结构体
typedef struct
{
    const gpio_num_t *light_pins; // Light 引脚数组
    int light_count;              // Light 数量
    LightState *state;             // 当前灯光状态
} LightController;

// 初始化灯光控制器
void light_controller_init(LightController *ctrl, const gpio_num_t *pins, LightState *state_buf, int count);

// 打开所有灯
void light_controller_all_on(LightController *ctrl);

// 关闭所有灯
void light_controller_all_off(LightController *ctrl);

// 打开指定灯
void light_controller_on(LightController *ctrl, int index);

// 关闭指定灯
void light_controller_off(LightController *ctrl, int index);

// 翻转灯光（开/关）
void light_controller_turn(LightController *ctrl, int index);

esp_err_t save_light_state(LightState *state, int index);
esp_err_t load_light_state(LightState *state, int index);