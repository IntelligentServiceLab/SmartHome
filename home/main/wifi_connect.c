// wifi_connect.c
// Wi-Fi 连接模块实现，基于 ESP-IDF 框架
// 负责初始化 Wi-Fi，管理事件回调，实现 STA 模式自动连接与重连

// === 头文件导入 ===
// ESP-IDF 标准库头文件
#include "esp_wifi.h"     // ESP32 Wi-Fi 驱动及 API
#include "esp_netif.h"    // 网络接口初始化
#include "nvs_flash.h"    // 非易失性存储初始化，用于保存 Wi-Fi 配置信息
#include "wifi_connect.h" // 自定义头文件，Wi-Fi配置宏定义及接口声明

// Wi-Fi 事件标志位，EventGroup 用于同步任务等待 Wi-Fi 连接成功
static EventGroupHandle_t event_group;
#define WIFI_CONNECTED_BIT BIT0

/**
 * @brief Wi-Fi 和 IP 事件处理回调函数
 * @param ctx 用户上下文指针，未使用
 * @param event_base 事件类型（Wi-Fi 事件或 IP 事件）
 * @param event_id 具体事件 ID
 * @param event_data 事件数据指针
 * @details 根据事件类型处理连接启动、断开、获取 IP 地址等逻辑，
 *          并通过事件组标志位通知连接状态
 */
static void event_handler(void *ctx, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START: // STA 模式启动事件
            ESP_LOGI("Wi-Fi", "STA Mode Started. Connecting...");
            esp_wifi_connect();               // 发起连接请求
            xEventGroupClearBits(event_group, WIFI_CONNECTED_BIT); // 清除连接成功标志
            break;
        case WIFI_EVENT_STA_DISCONNECTED: // 断开连接事件
            ESP_LOGI("Wi-Fi", "Disconnected from Wi-Fi. Reconnecting...");
            esp_wifi_connect();               // 断线后自动重新连接
            xEventGroupClearBits(event_group, WIFI_CONNECTED_BIT);
            break;
        default:
            break;
        }
    }
    else if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
        case IP_EVENT_STA_GOT_IP: // 获取到 IP 地址事件
        {
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            ESP_LOGI("Wi-Fi", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip)); // 打印 IP 地址
            xEventGroupSetBits(event_group, WIFI_CONNECTED_BIT);             // 设置连接成功标志
        }
        break;
        default:
            break;
        }
    }
}

/**
 * @brief 初始化并启动 Wi-Fi 以 STA 模式连接指定的 AP
 * @details 依次完成 NVS 初始化、网络接口初始化、Wi-Fi 驱动初始化、事件注册、
 *          配置 Wi-Fi 连接参数，并启动 Wi-Fi，开始连接 AP
 */
void wifi_init_sta()
{
    // 初始化 NVS，用于存储 Wi-Fi 配置信息
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase()); // 需要擦除后重新初始化
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 初始化网络接口
    esp_netif_init();
    esp_event_loop_create_default();

    // 创建默认 Wi-Fi STA 网络接口实例
    esp_netif_create_default_wifi_sta();

    // 初始化 Wi-Fi 驱动，使用默认配置参数
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 创建事件组用于同步等待 Wi-Fi 连接状态
    event_group = xEventGroupCreate();

    // 注册 Wi-Fi 及 IP 事件回调处理函数
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    // 配置 STA 模式的 SSID 和密码
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,           // Wi-Fi 名称
            .password = WIFI_PASS,       // Wi-Fi 密码
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // 启动 Wi-Fi 驱动，开始连接 AP
    ESP_ERROR_CHECK(esp_wifi_start());
}
