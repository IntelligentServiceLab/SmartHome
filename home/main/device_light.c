// ===== 头文件导入 =====
// 全部为项目自定义模块和ESP-IDF官方NVS模块头文件，非标准C库头文件
#include "device_light.h"   // 灯光设备控制模块声明，包含LightController结构和接口
#include "mqtt.h"           // MQTT客户端模块，负责设备状态上报
#include "nvs_flash.h"      // ESP-IDF NVS Flash初始化模块，通常用于系统启动时初始化NVS
#include "nvs.h"            // ESP-IDF NVS操作接口，支持非易失性存储读写

// 日志打印TAG，用于定位日志来源模块
static const char *TAG = "device_light";

/**
 * @brief 初始化灯光控制器
 * 
 * @param ctrl      指向LightController结构体的指针，用于保存控制器状态
 * @param pins      指向GPIO编号数组，表示灯光控制引脚
 * @param state_buf 指向LightState结构体数组，保存每个灯光的状态
 * @param count     灯光数量
 * 
 * 功能：
 * - 遍历所有灯光，尝试从NVS读取对应的保存状态
 * - 初始化每个GPIO为输出模式
 * - 根据保存状态，打开或关闭对应灯光GPIO输出
 */
void light_controller_init(LightController *ctrl, const gpio_num_t *pins, LightState *state_buf, int count)
{
    ctrl->light_pins = pins;
    ctrl->light_count = count;
    ctrl->state = state_buf;

    for (int i = 0; i < count; i++)
    {
        // 读取灯光状态，存储到state_buf对应位置
        esp_err_t err = load_light_state(state_buf, i);
        if (err == ESP_OK)
        {
            printf("Light %d: mode=%d, is_on=%d\n", i + 1,
                   state_buf[i].mode, state_buf[i].is_on);
        }
        else
        {
            printf("Failed to load Light state: %s\n", esp_err_to_name(err));
        }

        // 配置GPIO为输出模式，无上下拉，无中断
        gpio_config_t io_conf = {
            .pin_bit_mask = 1ULL << pins[i],
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE};
        gpio_config(&io_conf);

        // 根据状态打开或关闭灯光GPIO输出
        if (state_buf[i].is_on)
            light_controller_on(ctrl, i);
        else
            light_controller_off(ctrl, i);
    }
}

/**
 * @brief 将所有灯光全部打开
 * 
 * @param ctrl 指向LightController结构体指针
 * 
 * 功能：
 * - 遍历所有灯光GPIO，设置为高电平（打开灯光）
 * - 更新状态结构体中的is_on和mode字段（手动模式）
 * - 发送MQTT状态消息
 * - 将状态保存到NVS
 */
void light_controller_all_on(LightController *ctrl)
{
    for (int i = 0; i < ctrl->light_count; i++)
    {
        gpio_set_level(ctrl->light_pins[i], 1);
        ctrl->state[i].is_on = true;
        ctrl->state[i].mode = CONTROL_MANUAL;

        // 根据索引发送对应设备状态到MQTT
        if (i == 0)
            send_device_status(mqtt_client, "light-001", "on");
        if (i == 1)
            send_device_status(mqtt_client, "light-002", "on");
        if (i == 2)
            send_device_status(mqtt_client, "light-003", "on");
        if (i == 3)
            send_device_status(mqtt_client, "light-004", "on");

        save_light_state(ctrl->state, i);
    }
}

/**
 * @brief 将所有灯光全部关闭
 * 
 * @param ctrl 指向LightController结构体指针
 * 
 * 功能同light_controller_all_on，区别为设置GPIO低电平，关闭灯光，发送off状态
 */
void light_controller_all_off(LightController *ctrl)
{
    for (int i = 0; i < ctrl->light_count; i++)
    {
        gpio_set_level(ctrl->light_pins[i], 0);
        ctrl->state[i].is_on = false;
        ctrl->state[i].mode = CONTROL_MANUAL;

        if (i == 0)
            send_device_status(mqtt_client, "light-001", "off");
        if (i == 1)
            send_device_status(mqtt_client, "light-002", "off");
        if (i == 2)
            send_device_status(mqtt_client, "light-003", "off");
        if (i == 3)
            send_device_status(mqtt_client, "light-004", "off");

        save_light_state(ctrl->state, i);
    }
}

/**
 * @brief 打开指定索引灯光
 * 
 * @param ctrl  指向LightController结构体指针
 * @param index 灯光索引（0~count-1）
 * 
 * 功能：
 * - GPIO置高打开灯光
 * - 状态结构体更新
 * - 发送MQTT开灯状态
 * - 保存NVS状态
 */
void light_controller_on(LightController *ctrl, int index)
{
    if (index >= 0 && index < ctrl->light_count)
    {
        gpio_set_level(ctrl->light_pins[index], 1);
        ctrl->state[index].is_on = true;
        ctrl->state[index].mode = CONTROL_MANUAL;

        if (index == 0)
            send_device_status(mqtt_client, "light-001", "on");
        if (index == 1)
            send_device_status(mqtt_client, "light-002", "on");
        if (index == 2)
            send_device_status(mqtt_client, "light-003", "on");
        if (index == 3)
            send_device_status(mqtt_client, "light-004", "on");

        save_light_state(ctrl->state, index);
    }
}

/**
 * @brief 关闭指定索引灯光
 * 
 * @param ctrl  指向LightController结构体指针
 * @param index 灯光索引
 * 
 * 功能同light_controller_on，GPIO置低关闭灯光，发送off状态
 */
void light_controller_off(LightController *ctrl, int index)
{
    if (index >= 0 && index < ctrl->light_count)
    {
        gpio_set_level(ctrl->light_pins[index], 0);
        ctrl->state[index].is_on = false;
        ctrl->state[index].mode = CONTROL_MANUAL;

        if (index == 0)
            send_device_status(mqtt_client, "light-001", "off");
        if (index == 1)
            send_device_status(mqtt_client, "light-002", "off");
        if (index == 2)
            send_device_status(mqtt_client, "light-003", "off");
        if (index == 3)
            send_device_status(mqtt_client, "light-004", "off");

        save_light_state(ctrl->state, index);
    }
}

/**
 * @brief 灯光状态翻转（开关切换）
 * 
 * @param ctrl  指向LightController结构体指针
 * @param index 灯光索引
 * 
 * 功能：
 * - 若当前灯光开着，则关闭
 * - 若当前灯光关闭，则打开
 */
void light_controller_turn(LightController *ctrl, int index)
{
    if (ctrl->state[index].is_on == true)
    {
        light_controller_off(ctrl, index);
    }
    else
    {
        light_controller_on(ctrl, index);
    }
}

/**
 * @brief 保存指定索引灯光状态到NVS
 * 
 * @param state 指向LightState数组
 * @param index 指定灯光索引
 * @return esp_err_t ESP_OK表示成功，其他为错误码
 * 
 * 功能：
 * - 打开命名空间"light_ns"
 * - 根据灯光索引，存储对应的mode和is_on状态值
 * - 提交并关闭NVS句柄
 */
esp_err_t save_light_state(LightState *state, int index)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open("light_ns", NVS_READWRITE, &handle);
    if (err != ESP_OK)
        return err;

    if (index == 0)
    {
        nvs_set_i32(handle, "light001_mode", (int32_t)state[index].mode);
        nvs_set_i32(handle, "light001_isOn", (int32_t)state[index].is_on);
    }
    if (index == 1)
    {
        nvs_set_i32(handle, "light002_mode", (int32_t)state[index].mode);
        nvs_set_i32(handle, "light002_isOn", (int32_t)state[index].is_on);
    }
    if (index == 2)
    {
        nvs_set_i32(handle, "light003_mode", (int32_t)state[index].mode);
        nvs_set_i32(handle, "light003_isOn", (int32_t)state[index].is_on);
    }
    if (index == 3)
    {
        nvs_set_i32(handle, "light004_mode", (int32_t)state[index].mode);
        nvs_set_i32(handle, "light004_isOn", (int32_t)state[index].is_on);
    }

    err = nvs_commit(handle);
    nvs_close(handle);
    return err;
}

/**
 * @brief 从NVS读取指定索引灯光状态
 * 
 * @param state 指向LightState数组
 * @param index 指定灯光索引
 * @return esp_err_t ESP_OK表示读取成功，其他表示失败或参数错误
 * 
 * 功能：
 * - 打开命名空间"light_ns"
 * - 读取对应灯光的mode和is_on状态
 * - 赋值给state数组对应元素
 * - 关闭NVS句柄
 */
esp_err_t load_light_state(LightState *state, int index)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open("light_ns", NVS_READONLY, &handle);
    if (err != ESP_OK)
        return err;

    int32_t mode = 0;
    int32_t is_on = 0;

    switch (index)
    {
    case 0:
        nvs_get_i32(handle, "light001_mode", &mode);
        nvs_get_i32(handle, "light001_isOn", &is_on);
        break;
    case 1:
        nvs_get_i32(handle, "light002_mode", &mode);
        nvs_get_i32(handle, "light002_isOn", &is_on);
        break;
    case 2:
        nvs_get_i32(handle, "light003_mode", &mode);
        nvs_get_i32(handle, "light003_isOn", &is_on);
        break;
    case 3:
        nvs_get_i32(handle, "light004_mode", &mode);
        nvs_get_i32(handle, "light004_isOn", &is_on);
        break;
    default:
        nvs_close(handle);
        return ESP_ERR_INVALID_ARG;
    }

    state[index].mode = (ControlMode)mode;
    state[index].is_on = (bool)is_on;

    nvs_close(handle);
    return ESP_OK;
}
