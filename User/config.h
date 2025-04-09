#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "ti_msp_dl_config.h"
#include <stdlib.h>

// 串口配置
#define RX_LEN_MAX_0 128
#define RX_LEN_MAX_1 128

// 定时器配置
#define PID_TIMER_T 0.005f
#define ENCODER_TIMER_T 0.0025f

// 小车参数
#define WHEEL_TRACK 0.12855f // 轮距
#define WHEEL_BASE 0.1117f // 轴距

#endif // __CONFIG_H__