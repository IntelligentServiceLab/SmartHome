// ===== 头文件导入 =====
// 这些均为自定义模块头文件，非标准库
#include "device_fan.h"   // 风扇控制相关声明，结构体和函数接口
#include "mqtt.h"         // MQTT消息发布模块，发送设备状态
#include "nvs_flash.h"    // ESP-IDF NVS Flash模块，用于非易失性存储初始化（通常初始化时调用）
#include "nvs.h"          // ESP-IDF NVS操作接口，用于读写持久化存储

// PWM配置宏定义，使用ESP-IDF LEDC外设实现风扇PWM调速
#define FAN_PWM_TIMER LEDC_TIMER_0          // 使用LED PWM定时器0
#define FAN_PWM_MODE LEDC_LOW_SPEED_MODE    // 低速模式，适合非高速应用
#define FAN_PWM_RES LEDC_TIMER_13_BIT       // PWM分辨率13位，即PWM计数范围0~8191
#define FAN_PWM_FREQ_HZ 1000                 // PWM频率1000Hz

// 日志TAG，用于ESP_LOG打印区分模块
static const char *TAG = "FanController";

/**
 * @brief 初始化风扇控制器
 * 
 * @param ctrl         指向FanController结构体实例的指针，用于保存风扇状态和GPIO配置
 * @param pwm_gpio     用于PWM输出控制风扇转速的GPIO编号
 * @param dir_gpio     用于风扇转向控制的GPIO编号
 * @param pwm_channel  LEDC PWM通道编号
 * 
 * 函数功能：
 * - 从NVS读取保存的风扇状态（模式和转速等级）
 * - 配置转向控制GPIO为输出
 * - 配置LEDC定时器及PWM通道
 * - 更新风扇驱动输出，使硬件状态与保存的状态同步
 */
void fan_controller_init(FanController *ctrl, gpio_num_t pwm_gpio, gpio_num_t dir_gpio, ledc_channel_t pwm_channel)
{
    ctrl->pwm_gpio = pwm_gpio;
    ctrl->dir_gpio = dir_gpio;
    ctrl->pwm_channel = pwm_channel;

    // 尝试从NVS加载风扇状态，初始化ctrl->state
    esp_err_t err = load_fan_state(&ctrl->state);
    if (err == ESP_OK)
    {
        printf("Fan Mode: %d, Level: %d\n", ctrl->state.mode, ctrl->state.level);
    }
    else
    {
        printf("Failed to load fan state: %s\n", esp_err_to_name(err));
    }

    // 配置转向控制GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << dir_gpio,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // 配置LEDC定时器参数
    ledc_timer_config_t ledc_timer = {
        .speed_mode = FAN_PWM_MODE,
        .timer_num = FAN_PWM_TIMER,
        .duty_resolution = FAN_PWM_RES,
        .freq_hz = FAN_PWM_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // 配置LEDC PWM通道
    ledc_channel_config_t ledc_channel = {
        .speed_mode = FAN_PWM_MODE,
        .channel = pwm_channel,
        .timer_sel = FAN_PWM_TIMER,
        .gpio_num = pwm_gpio,
        .duty = 0,                 // 初始占空比0（关闭）
        .hpoint = 0,
        .intr_type = LEDC_INTR_DISABLE
    };
    ledc_channel_config(&ledc_channel);

    // 根据当前状态更新风扇PWM输出和转向
    fan_controller_update(ctrl);

    ESP_LOGI(TAG, "FanController initialized on GPIO %d", pwm_gpio);
}

/**
 * @brief 设置风扇转速等级
 * 
 * @param ctrl  风扇控制器指针
 * @param level 目标转速等级（FAN_OFF、FAN_LOW、FAN_MEDIUM、FAN_HIGH）
 * 
 * 功能：
 * - 更新风扇状态结构体的level和mode（手动控制）
 * - 调用更新函数调整硬件PWM和GPIO输出
 * - 发送风扇状态消息到MQTT服务器
 * - 将状态持久化保存到NVS
 */
void fan_controller_set_level(FanController *ctrl, FanLevel level)
{
    ctrl->state.level = level;
    ctrl->state.mode = CONTROL_MANUAL;

    fan_controller_update(ctrl);

    if (level == FAN_OFF)
        send_device_status(mqtt_client, "fan-001", "off");  // MQTT发布风扇关闭状态
    else
        send_device_status(mqtt_client, "fan-001", "on");   // MQTT发布风扇开启状态

    save_fan_state(ctrl->state);  // 保存状态到NVS
}

/**
 * @brief 根据当前风扇状态更新PWM占空比及转向GPIO
 * 
 * @param ctrl 风扇控制器指针
 * 
 * 说明：
 * - 根据风扇状态level计算对应PWM占空比
 * - 默认风扇转向为正转（forward = true）
 * - 更新PWM和转向GPIO输出
 * - 打印日志方便调试
 */
void fan_controller_update(FanController *ctrl)
{
    uint32_t duty = 0;
    bool forward = true; // 默认正转

    // 根据风速等级计算占空比（0~2^13-1范围）
    switch (ctrl->state.level)
    {
    case FAN_OFF:
        duty = 0;
        break;
    case FAN_LOW:
        duty = (40 * ((1 << FAN_PWM_RES) - 1)) / 100;   // 40%占空比
        break;
    case FAN_MEDIUM:
        duty = (60 * ((1 << FAN_PWM_RES) - 1)) / 100;   // 60%占空比
        break;
    case FAN_HIGH:
        duty = (100 * ((1 << FAN_PWM_RES) - 1)) / 100;  // 100%占空比
        break;
    default:
        duty = 0;
        break;
    }

    // 设置转向GPIO（true时为0，false为1）
    gpio_set_level(ctrl->dir_gpio, forward ? 0 : 1);

    // 设置PWM占空比并更新
    ledc_set_duty(FAN_PWM_MODE, ctrl->pwm_channel, duty);
    ledc_update_duty(FAN_PWM_MODE, ctrl->pwm_channel);

    ESP_LOGI(TAG, "Fan level set to %d (duty = %lu)", ctrl->state.level, duty);
}

/**
 * @brief 保存风扇状态到NVS非易失性存储
 * 
 * @param state 风扇当前状态结构体
 * @return esp_err_t 操作状态，ESP_OK表示成功
 * 
 * 打开命名空间 "fan001_ns"，写入mode和level两个int32键值，提交并关闭NVS
 */
esp_err_t save_fan_state(FanState state)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open("fan001_ns", NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return err;

    nvs_set_i32(handle, "fan001_mode", (int32_t)state.mode);
    nvs_set_i32(handle, "fan001_level", (int32_t)state.level);

    err = nvs_commit(handle);
    nvs_close(handle);
    return err;
}

/**
 * @brief 从NVS读取保存的风扇状态
 * 
 * @param state 传入的风扇状态结构体指针，用于存储读取结果
 * @return esp_err_t 操作状态，ESP_OK表示读取成功，其他表示失败
 * 
 * 打开命名空间 "fan001_ns"，读取mode和level键值，默认值为0（若不存在）
 */
esp_err_t load_fan_state(FanState *state)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open("fan001_ns", NVS_READONLY, &handle);
    if (err != ESP_OK)
        return err;

    int32_t mode = 0;
    int32_t level = 0;

    err = nvs_get_i32(handle, "fan001_mode", &mode);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
    {
        nvs_close(handle);
        return err;
    }

    err = nvs_get_i32(handle, "fan001_level", &level);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
    {
        nvs_close(handle);
        return err;
    }

    // 转换为枚举类型赋值
    state->mode = (ControlMode)mode;
    state->level = (FanLevel)level;

    nvs_close(handle);
    return ESP_OK;
}
