#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "ti_msp_dl_config.h"
#include <stdlib.h>

// 定时器配置
#define PID_TIMER_T 0.005f
#define ENCODER_TIMER_T 0.0025f

// 小车参数
#define WHEEL_TRACK 0.12855f // 轮距
#define WHEEL_BASE 0.1117f // 轴距

// 蓝牙配置
#define BLUETOOTH_UART_INDEX 0 // 蓝牙串口索引号
#define BLUETOOTH_RX_LEN_MAX 20 // 蓝牙接收数据最大长度

#endif // __CONFIG_H__