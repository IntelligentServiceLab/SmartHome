#pragma once


#include "driver/ledc.h"
#include "driver/gpio.h"
#include "common.h"

// 风扇档位定义
typedef enum {
    FAN_OFF = 0,
    FAN_LOW,
    FAN_MEDIUM,
    FAN_HIGH
} FanLevel;

// 风扇状态结构体
typedef struct {
    ControlMode mode;   // 自动或手动控制模式
    FanLevel level;     // 当前风速档位
} FanState;

// 风扇控制器结构体
typedef struct {
    gpio_num_t pwm_gpio;           // PWM 控制引脚
    gpio_num_t dir_gpio;           // 方向控制引脚（可选）
    ledc_channel_t pwm_channel;    // LEDC 通道
    FanState state;                // 当前风扇状态
} FanController;

// 初始化风扇控制器
void fan_controller_init(FanController *ctrl, gpio_num_t pwm_gpio, gpio_num_t dir_gpio, ledc_channel_t pwm_channel);

// 设置风扇档位
void fan_controller_set_level(FanController *ctrl, FanLevel level);

// 更新风扇硬件状态
void fan_controller_update(FanController *ctrl);


esp_err_t save_fan_state(FanState state);
esp_err_t load_fan_state(FanState *state);