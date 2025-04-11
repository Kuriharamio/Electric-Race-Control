#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "ti_msp_dl_config.h"
#include <stdlib.h>

// 定时器配置
#define PID_MOTOR_TIMER_T 0.005f
#define PID_CAR_SPEED_TIMER_T (2 * PID_MOTOR_TIMER_T)
#define PID_CAR_POSITION_TIMER_T (3 * PID_MOTOR_TIMER_T)
#define ENCODER_TIMER_T 0.0025f

// 小车参数
#define WHEEL_TRACK 0.12855f // 轮距
#define WHEEL_BASE 0.1117f // 轴距
#define WHEEL_RADIUS 0.024f // 轮子半径
#define WHEEL_Gearbox_Rate 20.0f // 减速比
#define WHEEL_Per_Pulse 13.0f // 每转脉冲数


// 蓝牙配置
#define BLUETOOTH_UART_INDEX 0 // 蓝牙串口索引号
#define BLUETOOTH_RX_LEN_MAX 20 // 蓝牙接收数据最大长度

// K230 串口配置
#define K230_UART_INDEX 1 // K230串口索引号
#define K230_RX_LEN_MAX 128 // K230接收数据最大长度

// IMU 配置
#define IMU_RX_LEN_MAX 11 // IMU接收数据最大长度

#endif // __CONFIG_H__