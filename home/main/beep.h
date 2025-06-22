#ifndef __BEEP_H__
#define __BEEP_H__

#include "driver/gpio.h"

#define BUZZER_GPIO GPIO_NUM_48

// 初始化蜂鸣器 GPIO
void buzzer_init(void);

// 打开蜂鸣器（低电平触发）
void buzzer_on(void);

// 关闭蜂鸣器（高电平关闭）
void buzzer_off(void);

#endif // __BEEP_H__
