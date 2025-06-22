// room.c
// 房间模式管理模块实现
// 负责初始化房间状态、切换房间不同控制模式（HOME, AWAY, SLEEP），
// 并执行相应设备控制逻辑

// === 头文件导入 ===
// 自定义头文件
#include "room.h"    // 房间模块声明及相关数据结构

// 全局房间模块实例，供整个系统使用
RoomModule room;

/**
 * @brief 房间模块初始化函数
 * @details 初始化房间的模式、设备控制器、传感器控制器及用户偏好参数，
 *          初始化网络状态及 ESP-NOW 状态标志
 */
void room_init()
{
    room.current_mode = MODE_HOME;               // 初始模式设为“家中”
    room.device_ctrl = &device_ctrl;             // 绑定全局设备控制器指针
    room.sensor_ctrl = &sensor_ctrl;             // 绑定全局传感器控制器指针

    // 设定用户环境偏好默认值（温度、湿度、光照）
    room.preference.tempPreference = 26;         // 温度偏好26摄氏度
    room.preference.humiPreference = 30;         // 湿度偏好30%
    room.preference.lightPreference = 50;        // 光照偏好50%

    room.network_connected = false;               // 默认网络未连接
    room.espnow_ready = false;                     // 默认ESP-NOW未准备好
}

/**
 * @brief 应用“回家模式”配置
 * @param room 指向房间模块实例的指针
 * @details 
 *   设置房间模式为HOME，
 *   自动打开客厅主灯（灯光0号），且设置为自动控制模式，
 *   根据温度偏好自动调节风扇为低速，且风扇进入自动控制模式
 */
void apply_home_mode(RoomModule *room)
{
    room->current_mode = MODE_HOME;

    // 自动打开客厅主灯（灯0），且标记为自动控制
    light_controller_on(room->device_ctrl->lights, 0);
    room->device_ctrl->lights->state[0].mode = CONTROL_AUTO;

    // 如果当前温度高于用户偏好温度，开启风扇低速
    if (room->sensor_ctrl->dht_sensor->data.temp > room->preference.tempPreference)
    {
        fan_controller_set_level(room->device_ctrl->fans, FAN_LOW);
    }
    // 设置风扇为自动控制模式
    room->device_ctrl->fans->state.mode = CONTROL_AUTO;
}

/**
 * @brief 应用“离家模式”配置
 * @param room 指向房间模块实例的指针
 * @details 
 *   设置房间模式为AWAY，
 *   关闭所有灯光，
 *   关闭风扇，
 *   （可扩展：关闭窗帘等其它设备）
 */
void apply_away_mode(RoomModule *room)
{
    room->current_mode = MODE_AWAY;

    // 关闭所有灯光
    light_controller_all_off(room->device_ctrl->lights);

    // 关闭风扇
    fan_controller_set_level(room->device_ctrl->fans, FAN_OFF);

    // TODO: 关闭窗帘
}

/**
 * @brief 应用“睡眠模式”配置
 * @param room 指向房间模块实例的指针
 * @details 
 *   设置房间模式为SLEEP，
 *   关闭主灯，打开夜灯（假设灯1为夜灯），
 *   将风扇设置为低速运行，
 *   （可扩展：关闭窗帘等）
 */
void apply_sleep_mode(RoomModule *room)
{
    room->current_mode = MODE_SLEEP;

    // 关闭主灯（灯0）
    light_controller_off(room->device_ctrl->lights, 0);
    // 开启夜灯（灯1）
    light_controller_on(room->device_ctrl->lights, 1);

    // 风扇减速运行（低速）
    fan_controller_set_level(room->device_ctrl->fans, FAN_LOW);

    // TODO: 关闭窗帘
}
