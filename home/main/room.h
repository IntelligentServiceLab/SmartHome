#ifndef __ROOM_H__
#define __ROOM_H__
#ifdef __cplusplus
extern "C"
{
#endif
#include "devices.h"
#include "sensors.h"
    // 偏好定义
    typedef struct
    {
        float tempPreference;
        float humiPreference;
        float lightPreference;
    } Preference;
    // 整个房间模块的结构体（核心管理单元）
    typedef struct
    {
        SystemMode current_mode;       // 当前房间模式（回家/离家/睡觉）
        DeviceController *device_ctrl; // 所有设备合集
        SensorController *sensor_ctrl; // 所有传感器集合
        Preference preference;         // 房间偏好
        bool network_connected;        // 当前是否连接到 Wi-Fi
        bool espnow_ready;             // ESP-NOW 是否初始化完成
    } RoomModule;

    void room_init(void);
    void apply_home_mode(RoomModule *room);
    void apply_away_mode(RoomModule *room);
    void apply_sleep_mode(RoomModule *room);

    extern RoomModule room;

#ifdef __cplusplus
}
#endif

#endif // __ROOM_H__