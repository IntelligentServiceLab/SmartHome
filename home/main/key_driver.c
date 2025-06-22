// ===== 头文件导入 =====
// 自定义头文件
#include "key_driver.h"      // 按键驱动相关的接口声明和类型定义
#include "devices.h"         // 设备控制接口声明（灯光、风扇控制等）
// FreeRTOS相关头文件（标准FreeRTOS库）
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
// ESP-IDF系统服务头文件（标准库）
#include "esp_log.h"
#include "esp_timer.h"

#define MAX_KEYS 10               // 支持最大按键数量
#define DEBOUNCE_TIME_MS 50       // 防抖时间阈值（毫秒）
#define DOUBLE_CLICK_TIME_MS 400  // 双击时间间隔阈值（毫秒）

// 按键结构体，保存按键相关信息
typedef struct
{
    gpio_num_t gpio_num;          // 按键GPIO编号
    uint32_t last_press_time;     // 上次按下的时间（毫秒）
    uint32_t last_release_time;   // 上次松开的时间（毫秒）
    uint8_t click_count;          // 当前累计点击次数
    bool is_pressed;              // 当前按键是否按下状态
    key_event_callback_t cb;      // 按键事件回调函数指针
} key_t;

static key_t key_list[MAX_KEYS];  // 按键数组，存储所有注册的按键
static int key_count = 0;         // 已注册按键数量

static QueueHandle_t gpio_evt_queue = NULL;  // GPIO中断事件队列句柄
static const char *TAG = "key_driver";       // 日志TAG

// 获取当前时间戳（毫秒），用于时间测量和防抖
static uint32_t IRAM_ATTR get_ms()
{
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}

// GPIO中断服务程序，放入队列等待后续处理（IRAM_ATTR加速）
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

// 根据GPIO编号查找对应的key_t结构体指针，返回NULL表示未找到
static key_t *find_key(gpio_num_t gpio_num)
{
    for (int i = 0; i < key_count; i++)
    {
        if (key_list[i].gpio_num == gpio_num)
            return &key_list[i];
    }
    return NULL;
}

// 按键事件处理任务（独立FreeRTOS任务）
static void key_task(void *arg)
{
    uint32_t gpio_num;
    while (1)
    {
        // 阻塞等待GPIO事件队列消息（按键中断）
        if (xQueueReceive(gpio_evt_queue, &gpio_num, portMAX_DELAY))
        {
            key_t *key = find_key(gpio_num);
            if (!key)
                continue;

            uint32_t now = get_ms();
            bool level = gpio_get_level(gpio_num);

            // 按键按下事件处理（低电平按下）
            if (!level && !key->is_pressed)
            {
                key->last_press_time = now;
                key->is_pressed = true;
            }
            // 按键松开事件处理（高电平松开）
            else if (level && key->is_pressed)
            {
                uint32_t press_duration = now - key->last_press_time;
                key->is_pressed = false;

                // 判断按下时间是否超过防抖阈值
                if (press_duration >= DEBOUNCE_TIME_MS)
                {
                    if (key->click_count == 0)
                    {
                        // 第一次点击，记录松开时间，等待可能的双击
                        key->click_count = 1;
                        key->last_release_time = now;
                    }
                    else if (key->click_count == 1 && (now - key->last_release_time) <= DOUBLE_CLICK_TIME_MS)
                    {
                        // 双击检测成功，调用回调，重置计数
                        key->cb(gpio_num, KEY_DOUBLE_CLICK);
                        key->click_count = 0;
                        continue;
                    }
                }
            }

            // 单击超时判断，超过双击时间则确认单击事件
            if (key->click_count == 1 && (now - key->last_release_time) > DOUBLE_CLICK_TIME_MS)
            {
                key->cb(gpio_num, KEY_SINGLE_CLICK);
                key->click_count = 0;
            }
        }
    }
}

// 注册按键，初始化GPIO及中断，并保存回调函数
static void key_register(gpio_num_t gpio_num, key_event_callback_t cb)
{
    if (key_count >= MAX_KEYS)
    {
        ESP_LOGW(TAG, "Max key limit reached");
        return;
    }

    // 配置GPIO输入模式，开启上下拉，触发任何边沿中断
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = 1ULL << gpio_num,
        .pull_down_en = 0,
        .pull_up_en = 1,
    };
    gpio_config(&io_conf);

    // 安装GPIO中断服务，优先级0，单例调用
    gpio_install_isr_service(0);
    // 添加指定GPIO的中断处理函数
    gpio_isr_handler_add(gpio_num, gpio_isr_handler, (void *)gpio_num);

    // 保存按键结构体数据
    key_list[key_count++] = (key_t){
        .gpio_num = gpio_num,
        .last_press_time = 0,
        .last_release_time = 0,
        .click_count = 0,
        .is_pressed = false,
        .cb = cb};

    ESP_LOGI(TAG, "Registered key on GPIO %d", gpio_num);
}

// 启动按键驱动任务和事件队列
static void key_driver_start(void)
{
    if (!gpio_evt_queue)
        gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(key_task, "key_task", 4096, NULL, 10, NULL);
}

// 按键事件回调函数，处理单击和双击事件
static void key_event_cb(gpio_num_t gpio, key_event_t evt)
{
    switch (evt)
    {
    case KEY_SINGLE_CLICK:
        ESP_LOGI(TAG, "GPIO %d: SINGLE CLICK", gpio);
        switch (gpio)
        {
        case GPIO_NUM_3:
            light_controller_turn(device_ctrl.lights, 0);
            break;
        case GPIO_NUM_9:
            light_controller_turn(device_ctrl.lights, 1);
            break;
        case GPIO_NUM_11:
            light_controller_turn(device_ctrl.lights, 2);
            break;
        case GPIO_NUM_13:
            light_controller_turn(device_ctrl.lights, 3);
            break;
        case GPIO_NUM_5:
        {
            int level = (int)device_ctrl.fans->state.level;
            level = (level + 1) % 4;  // 依次循环风扇档位：OFF->LOW->MEDIUM->HIGH->OFF
            fan_controller_set_level(device_ctrl.fans, (FanLevel)level);
        }
        break;
        default:
            break;
        }
        break;

    case KEY_DOUBLE_CLICK:
        ESP_LOGI(TAG, "GPIO %d: DOUBLE CLICK", gpio);
        switch (gpio)
        {
        // 这里注释了灯光双击关闭的代码，如需启用可取消注释
        // case GPIO_NUM_3:
        //     light_controller_off(device_ctrl.lights, 0);
        //     break;
        // case GPIO_NUM_9:
        //     light_controller_off(device_ctrl.lights, 1);
        //     break;
        // case GPIO_NUM_11:
        //     light_controller_off(device_ctrl.lights, 2);
        //     break;
        // case GPIO_NUM_13:
        //     light_controller_off(device_ctrl.lights, 3);
        //     break;
        case GPIO_NUM_5:
            // 风扇双击关闭
            fan_controller_set_level(device_ctrl.fans, FAN_OFF);
            break;
        default:
            break;
        }
        break;
    }
}

// 对外接口，初始化按键驱动，注册GPIO按键及其回调
void keys_init(void)
{
    key_register(GPIO_NUM_3, key_event_cb);
    key_register(GPIO_NUM_9, key_event_cb);
    key_register(GPIO_NUM_11, key_event_cb);
    key_register(GPIO_NUM_13, key_event_cb);
    key_register(GPIO_NUM_5, key_event_cb);
    key_driver_start();
}
