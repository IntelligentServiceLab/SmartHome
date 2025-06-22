// ===== 头文件导入 =====
// "devices.h" 是项目自定义头文件，包含设备控制器相关的声明，如 LightController, FanController, DeviceController 结构和接口
#include "devices.h"

// ===== 灯光控制相关变量定义 =====
// 定义4个灯光GPIO引脚号数组，绑定具体硬件引脚
static const gpio_num_t lights[] = {GPIO_NUM_8, GPIO_NUM_46, GPIO_NUM_10, GPIO_NUM_12};
// 4个灯光的状态数组，保存每个灯光的开关状态和控制模式
LightState light_states[4];
// 灯光控制器实例，管理灯光设备
LightController light_ctrl;

// ===== 风扇控制相关变量定义 =====
// PWM控制风扇的GPIO编号
#define FAN_PWM_GPIO GPIO_NUM_4
// 风扇方向控制GPIO编号
#define FAN_DIR_GPIO GPIO_NUM_14
// 使用的LEDC PWM通道号
#define FAN_PWM_CHANNEL LEDC_CHANNEL_0
// 风扇控制器实例，管理风扇设备
FanController fan_ctrl;

// ===== 设备集合控制器 =====
// 统一管理所有设备（灯光、风扇等）的结构体
DeviceController device_ctrl;

/**
 * @brief 设备模块初始化函数
 * 
 * 功能：
 * - 初始化灯光控制器，传入灯光GPIO数组，状态数组和灯光数量
 * - 初始化风扇控制器，指定PWM和方向GPIO，PWM通道
 * - 将灯光控制器和风扇控制器分别绑定到设备控制器结构体的对应成员
 * 
 * @note 该函数应在系统启动时调用，完成所有设备的初始化配置
 */
void devices_init()
{
    // 灯光控制器初始化
    light_controller_init(&light_ctrl, lights, light_states, sizeof(lights) / sizeof(lights[0]));

    // 风扇控制器初始化
    fan_controller_init(&fan_ctrl, FAN_PWM_GPIO, FAN_DIR_GPIO, FAN_PWM_CHANNEL);

    // 设备控制器结构体成员赋值，方便统一管理
    device_ctrl.lights = &light_ctrl;
    device_ctrl.fans = &fan_ctrl;
}
