#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "ti_msp_dl_config.h"
#include <stdlib.h>

//* 声光提示配置
#define USE_LED

//* 串口配置
#define USE_UART_0
// #define USE_UART_1
// #define USE_UART_2

//* 舵机配置
#define SERVO_UP_INDEX 0
#define SERVO_DOWN_INDEX 1

//* ADC 按钮配置
#define USE_ADC_BUTTON

//* K230 串口配置
#define K230_UART_INDEX 0 // K230串口索引号
#define K230_RX_LEN_MAX 20 // K230接收数据最大长度

//* TIMER 配置
#define USE_PID_TIMER
#define PID_DELTA_T 0.1f

#endif // __CONFIG_H__