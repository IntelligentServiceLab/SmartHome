// mqtt.c
// MQTT客户端初始化与消息处理
// 实现连接MQTT服务器，订阅主题，处理接收消息，及发送设备状态消息

// === 头文件导入 ===
// 自定义头文件
#include "common.h"          // 常用宏定义、全局变量声明等
#include "handle_message.h"  // 自定义消息处理函数声明
#include "room.h"            // 房间数据结构及设备控制器声明
#include "mqtt.h"            // MQTT接口声明
// 第三方库
#include "cJSON.h"           // JSON解析与构建库

#define TAG "MQTT"           // 日志标签

// MQTT客户端句柄，全局使用
esp_mqtt_client_handle_t mqtt_client = NULL;

// 订阅的主题列表
char *topics[] = {
    "devices",
    "thresholds",
    "preferences"
};
#define NUM_TOPICS (sizeof(topics) / sizeof(topics[0]))

/**
 * @brief MQTT事件处理回调函数
 * @param handler_args 用户自定义参数（此处未使用）
 * @param base 事件基名
 * @param event_id 事件ID
 * @param event_data 事件数据，esp_mqtt_event_handle_t类型
 * @details
 *   处理MQTT连接、断开、消息接收等事件，
 *   连接成功时订阅主题，接收到消息时调用对应的消息处理函数
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:  // MQTT连接成功
        ESP_LOGI(TAG, "MQTT connected/reconnected");

        // 连接或重连成功后订阅所有主题
        for (int i = 0; i < NUM_TOPICS; i++)
        {
            msg_id = esp_mqtt_client_subscribe(mqtt_client, topics[i], 0);
            ESP_LOGI(TAG, "Subscribed to topic: %s, msg_id=%d", topics[i], msg_id);
        }

        // 连接成功后主动发送当前灯光状态（避免断线重连后状态不同步）
        if (room.device_ctrl->lights->state[0].is_on)
            send_device_status(mqtt_client, "light-001", "on");
        else
            send_device_status(mqtt_client, "light-001", "off");

        if (room.device_ctrl->lights->state[1].is_on)
            send_device_status(mqtt_client, "light-002", "on");
        else
            send_device_status(mqtt_client, "light-002", "off");

        if (room.device_ctrl->lights->state[2].is_on)
            send_device_status(mqtt_client, "light-003", "on");  // 修正：light-03 -> light-003
        else
            send_device_status(mqtt_client, "light-003", "off");

        if (room.device_ctrl->lights->state[3].is_on)
            send_device_status(mqtt_client, "light-004", "on");
        else
            send_device_status(mqtt_client, "light-004", "off");

        break;

    case MQTT_EVENT_DISCONNECTED:  // MQTT断开连接
        ESP_LOGW(TAG, "MQTT disconnected");
        break;

    case MQTT_EVENT_DATA:  // 收到消息
        ESP_LOGI(TAG, "Received: topic=%.*s data=%.*s",
                 event->topic_len, event->topic,
                 event->data_len, event->data);

        // 判断消息所属主题，调用不同处理函数
        if (strncmp(event->topic, "devices", event->topic_len) == 0)
        {
            handle_device_message(event->data);
        }
        else if (strncmp(event->topic, "thresholds", event->topic_len) == 0)
        {
            handle_threshold_message(event->data);
        }
        else if (strncmp(event->topic, "preferences", event->topic_len) == 0)
        {
            handle_preference_message(event->data);
        }
        else
        {
            ESP_LOGW(TAG, "Unknown topic: %.*s", event->topic_len, event->topic);
        }
        break;

    default:
        ESP_LOGD(TAG, "Other MQTT event: %d", event->event_id);
        break;
    }
}

/**
 * @brief 初始化并启动MQTT客户端
 * @details 使用配置参数初始化MQTT客户端，注册事件处理函数，启动客户端连接服务器
 */
void mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
        .credentials = {
            .username = MQTT_USER,
            .authentication.password = MQTT_PASSWORD
        }
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);  // 初始化客户端
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);  // 注册事件处理回调
    esp_mqtt_client_start(mqtt_client);  // 启动客户端，开始连接
}

/**
 * @brief 构建设备状态JSON并通过MQTT发送
 * @param mqtt_client MQTT客户端句柄
 * @param device_id 设备ID字符串，例如"light-001"
 * @param device_status 设备状态字符串，例如"on"或"off"
 * @details 
 *   创建一个JSON对象包含deviceId和deviceStatus字段，
 *   转换成字符串后发布到主题"device/status"
 */
void send_device_status(esp_mqtt_client_handle_t mqtt_client, const char *device_id, const char *device_status)
{
    // 1. 创建 JSON 对象
    cJSON *root = cJSON_CreateObject();
    if (!root)
    {
        ESP_LOGE(TAG, "Failed to create JSON root object");
        return;
    }

    // 2. 添加字段 deviceId 和 deviceStatus
    cJSON_AddStringToObject(root, "deviceId", device_id);
    cJSON_AddStringToObject(root, "deviceStatus", device_status);

    // 3. 将JSON对象转换成字符串（无格式化）
    char *json_str = cJSON_PrintUnformatted(root);
    if (!json_str)
    {
        ESP_LOGE(TAG, "Failed to print JSON string");
        cJSON_Delete(root);
        return;
    }

    // 4. 发送MQTT消息，主题为"device/status"，QoS等级1，非保留消息
    int msg_id = esp_mqtt_client_publish(mqtt_client, "device/status", json_str, 0, 1, 0);
    ESP_LOGI(TAG, "Sent MQTT message id=%d: %s", msg_id, json_str);

    // 5. 释放JSON对象及字符串内存
    cJSON_Delete(root);
    free(json_str);  // cJSON_PrintUnformatted分配的内存需手动释放
}
