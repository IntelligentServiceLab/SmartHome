#ifndef __SU03T_H__
#define __SU03T_H__

#include "driver/uart.h"
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SU03T_UART_PORT UART_NUM_1
#define SU03T_TX_PIN    (17)
#define SU03T_RX_PIN    (18)
#define SU03T_BUF_SIZE  256

/**
 * 初始化 UART 和模块连接
 */
esp_err_t su03t_init(void);

/**
 * 读取 UART 返回的二进制数据（十六进制），打印输出并返回原始字节
 * @param data     数据缓冲区
 * @param max_len  缓冲区最大长度
 * @param timeout  超时时间（ticks）
 * @param out_len  实际读取长度
 */
esp_err_t su03t_read_response_hex(uint8_t *data, size_t max_len, TickType_t timeout, size_t *out_len);

/**
 * 可选：根据协议解析 SU03T 数据帧（起始0xA0 + cmd + param...）
 * @param data     输入数据
 * @param len      输入长度
 */
void su03t_parse_response(const uint8_t *data, size_t len);

void su03t_task(void *param);

#ifdef __cplusplus
}
#endif

#endif // __SU03T_H__