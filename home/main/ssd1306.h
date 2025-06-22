#pragma once

#include "driver/i2c.h"
#include "common.h"

// OLED 的 I2C 地址
#define SSD1306_I2C_ADDR     0x3C

// 屏幕分辨率
#define SSD1306_WIDTH        128
#define SSD1306_HEIGHT       64
#define SSD1306_PAGES        (SSD1306_HEIGHT / 8) // 一页=8像素，共8页

// I2C 接口配置
#define I2C_PORT             I2C_NUM_0          // 使用 I2C0
#define I2C_SDA_PIN          42                 // SDA 引脚
#define I2C_SCL_PIN          41                 // SCL 引脚
#define I2C_FREQ_HZ          400000             // 400kHz 传输速度

#ifdef __cplusplus
extern "C" {
#endif

// 初始化 OLED
void ssd1306_init(void);

// 清屏（清空缓冲区 + 刷新显示）
void ssd1306_clear(void);

// 刷新屏幕，将缓冲区内容显示到 OLED
void ssd1306_refresh(void);

// 在指定位置绘制一个字符（x: 列位置，page: 行页，c: 字符）
void ssd1306_draw_char(uint8_t x, uint8_t page, char c);
void ssd1306_draw_char8x16(uint8_t x, uint8_t page, char c);
// 在指定位置绘制字符串
void ssd1306_draw_string(uint8_t x, uint8_t page, const char *str);
void ssd1306_draw_string8x16(uint8_t x, uint8_t page, const char *str);

void show_task(void *pvParameters);

#ifdef __cplusplus
}
#endif
