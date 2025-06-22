// ===== 头文件导入 =====
// 本文件仅包含项目自定义头文件，无标准库头文件
#include "beep.h"    // 蜂鸣器控制模块头文件，声明蜂鸣器初始化及开关接口

/**
 * @brief 蜂鸣器初始化函数
 * 
 * 配置蜂鸣器GPIO为输出模式，关闭内部上拉和下拉，禁用中断。
 * 初始化时将GPIO输出置为高电平，关闭蜂鸣器。
 */
void buzzer_init(void)
{
    gpio_config_t io_conf = {
        // 设置控制蜂鸣器的GPIO引脚位掩码，1ULL左移BUZZER_GPIO位
        .pin_bit_mask = 1ULL << BUZZER_GPIO,

        // 设置GPIO模式为输出模式
        .mode = GPIO_MODE_OUTPUT,

        // 禁用GPIO上拉电阻
        .pull_up_en = GPIO_PULLUP_DISABLE,

        // 禁用GPIO下拉电阻
        .pull_down_en = GPIO_PULLDOWN_DISABLE,

        // 禁用GPIO中断
        .intr_type = GPIO_INTR_DISABLE
    };
    // 应用GPIO配置
    gpio_config(&io_conf);

    // 将GPIO初始输出设置为高电平，蜂鸣器处于关闭状态
    gpio_set_level(BUZZER_GPIO, 1);
}

/**
 * @brief 打开蜂鸣器
 * 
 * 通过将GPIO输出设置为低电平来驱动蜂鸣器发声。
 */
void buzzer_on(void)
{
    gpio_set_level(BUZZER_GPIO, 0);  // 低电平有效，蜂鸣器响
}

/**
 * @brief 关闭蜂鸣器
 * 
 * 通过将GPIO输出设置为高电平来关闭蜂鸣器。
 */
void buzzer_off(void)
{
    gpio_set_level(BUZZER_GPIO, 1);  // 高电平无声，蜂鸣器关
}