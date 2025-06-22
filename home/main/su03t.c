// su03t.c
// SU03T 语音模块驱动及控制任务实现
// 通过 UART 与语音模块通信，接收控制指令，控制设备状态，
// 并周期性发送环境数据帧给语音模块。

// === 头文件导入 ===
// 自定义头文件
#include "su03t.h"      // SU03T 模块相关声明
#include "devices.h"    // 设备控制接口，灯光与风扇控制
#include "sensors.h"    // 传感器数据接口，温湿度、光敏
#include "room.h"       // 房间状态及模式控制

// ESP-IDF 标准库头文件见 su03t.h 中包含

#define TAG "SU03T_TASK" // 日志TAG

/**
 * @brief 初始化 UART 串口，配置波特率、引脚及驱动
 * @return esp_err_t 初始化结果
 * @details 配置UART端口，安装驱动，创建语音模块任务su03t_task
 */
esp_err_t su03t_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_driver_install(SU03T_UART_PORT, SU03T_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(SU03T_UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(SU03T_UART_PORT, SU03T_TX_PIN, SU03T_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_LOGI(TAG, "UART initialized successfully.");
    xTaskCreate(su03t_task, "su03t_task", 4096, NULL, 5, NULL); // 创建任务，堆栈4KB，优先级5
    return ESP_OK;
}

/**
 * @brief 读取 UART 串口数据，打印十六进制格式
 * @param data 指向存储读取数据的缓冲区
 * @param max_len 最大可读取字节数
 * @param timeout 读取超时时间（ticks）
 * @param out_len 实际读取到的字节数（输出参数）
 * @return esp_err_t 读取状态，ESP_OK表示成功
 */
esp_err_t su03t_read_response_hex(uint8_t *data, size_t max_len, TickType_t timeout, size_t *out_len)
{
    if (!data || max_len < 1)
        return ESP_ERR_INVALID_ARG;

    int len = uart_read_bytes(SU03T_UART_PORT, data, max_len, timeout);
    if (len > 0)
    {
        if (out_len)
            *out_len = len;

        // 打印十六进制格式的接收数据
        printf("Received (%d bytes): ", len);
        for (int i = 0; i < len; ++i)
        {
            printf("%02X ", data[i]);
        }
        printf("\n");

        return ESP_OK;
    }
    else if (len == 0)
    {
        ESP_LOGW(TAG, "Read timeout.");
    }
    else
    {
        ESP_LOGE(TAG, "UART read error.");
    }

    return ESP_FAIL;
}

/**
 * @brief 发送控制命令数据帧给语音模块
 * @details 将温度（double，8字节）、湿度（double，8字节）、光强（int，4字节）封装进25字节帧，
 *          帧头帧尾固定，使用 UART 发送
 */
void send_control_command()
{
    uint8_t frame[25] = {0};

    // 帧头
    frame[0] = 0xAA;
    frame[1] = 0x55;
    // 消息号
    frame[2] = 0x01;

    // 使用 double 类型存储温湿度，避免 memcpy 错位和脏数据
    double temp_d = (double)(sensor_ctrl.dht_sensor->data.temp);
    double humi_d = (double)(sensor_ctrl.dht_sensor->data.humi);

    // 拷贝温度数据（8字节）至frame[3]开始处
    memcpy(&frame[3], &temp_d, sizeof(double));
    // 拷贝湿度数据（8字节）至frame[11]开始处
    memcpy(&frame[11], &humi_d, sizeof(double));
    // 拷贝光强数据（4字节）至frame[19]开始处
    memcpy(&frame[19], &sensor_ctrl.light_sensor->data.light, sizeof(int));

    // 帧尾
    frame[23] = 0x55;
    frame[24] = 0xAA;

    ESP_LOG_BUFFER_HEX(TAG, frame, 25);
    uart_write_bytes(SU03T_UART_PORT, frame, 25);
}

/**
 * @brief 解析语音模块返回的控制响应帧，执行相应设备控制操作
 * @param data 接收到的数据缓冲区
 * @param len 接收数据长度
 */
void su03t_parse_response(const uint8_t *data, size_t len)
{
    // 简单校验帧长度和帧头
    if (len != 5 || data[0] != 0xAA)
    {
        ESP_LOGW(TAG, "Invalid or incomplete frame.");
        return;
    }

    uint8_t deviceId = data[1]; // 设备ID
    uint8_t cmd = data[2];      // 命令类型（未使用）
    uint8_t param = data[3];    // 参数（开关/等级）
    uint8_t check = data[4];    // 校验字节（未使用）

    switch (deviceId)
    {
    case 0x01: // 灯1
        if (param == 0x00)
            light_controller_off(device_ctrl.lights, 0);
        else if (param == 0x01)
            light_controller_on(device_ctrl.lights, 0);
        break;
    case 0x02: // 灯2
        if (param == 0x00)
            light_controller_off(device_ctrl.lights, 1);
        else if (param == 0x01)
            light_controller_on(device_ctrl.lights, 1);
        break;
    case 0x03: // 灯3
        if (param == 0x00)
            light_controller_off(device_ctrl.lights, 2);
        else if (param == 0x01)
            light_controller_on(device_ctrl.lights, 2);
        break;
    case 0x04: // 灯4
        if (param == 0x00)
            light_controller_off(device_ctrl.lights, 3);
        else if (param == 0x01)
            light_controller_on(device_ctrl.lights, 3);
        break;
    case 0x05: // 风扇控制
        switch (param)
        {
        case 0x00:
            fan_controller_set_level(device_ctrl.fans, FAN_OFF);
            break;
        case 0x01:
            fan_controller_set_level(device_ctrl.fans, FAN_LOW);
            break;
        case 0x02:
            fan_controller_set_level(device_ctrl.fans, FAN_MEDIUM);
            break;
        case 0x03:
            fan_controller_set_level(device_ctrl.fans, FAN_HIGH);
            break;
        }
        break;
    case 0x06: // 窗帘（暂未实现）
        if (param == 0x00)
        {
            // 关闭窗帘逻辑
        }
        else if (param == 0x01)
        {
            // 打开窗帘逻辑
        }
        break;
    case 0x0F: // 房间模式切换
        switch (param)
        {
        case 0x01:
            apply_sleep_mode(&room);
            break;
        case 0x02:
            apply_home_mode(&room);
            break;
        case 0x03:
            apply_away_mode(&room);
            break;
        }
        break;
    default: // 其它未知设备ID时，发送当前环境数据
        send_control_command();
        break;
    }
}

/**
 * @brief SU03T 语音模块任务，周期读取 UART 数据并解析响应
 * @param param 任务参数（未使用）
 */
void su03t_task(void *param)
{
    uint8_t buffer[SU03T_BUF_SIZE];
    size_t len = 0;

    while (1)
    {
        if (su03t_read_response_hex(buffer, sizeof(buffer), pdMS_TO_TICKS(1000), &len) == ESP_OK)
        {
            su03t_parse_response(buffer, len); // 解析响应
        }
        else
        {
            ESP_LOGW(TAG, "No response or timeout.");
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时1秒
    }
}
