#pragma once

#include "driver/gpio.h"

typedef enum
{
    KEY_SINGLE_CLICK,
    KEY_DOUBLE_CLICK
} key_event_t;

typedef void (*key_event_callback_t)(gpio_num_t gpio_num, key_event_t event);
// 按键初始化
void keys_init(void);