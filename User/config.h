#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "ti_msp_dl_config.h"
#include <stdlib.h>

// 定时器配置
#define PID_MOTOR_TIMER_T 0.01f
#define PID_CAR_SPEED_TIMER_T (1 * PID_MOTOR_TIMER_T)
#define PID_CAR_POSITION_TIMER_T (1 * PID_MOTOR_TIMER_T)
#define ENCODER_TIMER_T 0.01f

// 小车参数
#define WHEEL_TRACK 0.12855f // 轮距
#define WHEEL_BASE 0.1117f // 轴距
#define WHEEL_RADIUS 0.024f // 轮子半径
#define WHEEL_Gearbox_Rate 20.0f // 减速比
#define WHEEL_Per_Pulse 13.0f // 每转脉冲数
#define MAX_LINEAR_SPEED 5.0f
#define MAX_ANGULAR_SPEED 5.0f

// 蓝牙配置
#define BLUETOOTH_UART_INDEX 0 // 蓝牙串口索引号
#define BLUETOOTH_RX_LEN_MAX 20 // 蓝牙接收数据最大长度

// K230 串口配置
#define K230_UART_INDEX 1 // K230串口索引号
#define K230_RX_LEN_MAX 128 // K230接收数据最大长度

// IMU 配置
#define IMU_RX_LEN_MAX 11 // IMU接收数据最大长度

// PurePursuit 配置
#define POINTS_NUM 100
#define MAX_LOOKAHEAD_DISTANCE 1.5f    // 最大前视距离
#define MIN_LOOKAHEAD_DISTANCE 0.01f    // 最小前视距离
#define MAX_SPEED 0.6f                 // 最大速度（单位：m/s）
#define MAX_CURVATURE 0.2f             // 最大曲率（限制转向角以防止过度转向）
#define LOOKAHEAD_DISTANCE_FACTOR 0.5f // 前视距离与速度的比例因子
#define K_STANLEY_HEADING 1.0f         // Stanley方法中航向误差权重
#define K_STANLEY_CROSSTRACK 1.0f      // Stanley方法中横向误差权重

#endif // __CONFIG_H__