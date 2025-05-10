#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "ti_msp_dl_config.h"
#include <stdlib.h>

//* 声光提示配置
#define USE_LED  // 启用LED
#define USE_BUZZ // 启用蜂鸣器

//! <<< 串口配置
#define USE_UART_0 // 启用串口0
// #define USE_UART_1 // 启用串口1
// #define USE_UART_2 // 启用串口2

//* K230 串口配置
#define USE_K230
#define K230_UART_INDEX 0  // K230串口索引号
#define K230_RX_LEN_MAX 20 // K230接收数据最大长度

//* IMU 串口配置
// #define USE_IMU
#define IMU_UART_INDEX 1  // IMU 串口索引号
#define IMU_RX_LEN_MAX 11 // IMU 接收数据最大长度

//* 蓝牙配置
// #define USE_BLUETOOTH
#define BLUETOOTH_UART_INDEX 2  // 蓝牙串口索引号
#define BLUETOOTH_RX_LEN_MAX 20 // 蓝牙接收数据最大长度

//! >>> 串口配置

//* 舵机配置
#define USE_SERVO          // 启用舵机
#define SERVO_UP_INDEX 0   // 上舵机索引号
#define SERVO_DOWN_INDEX 1 // 下舵机索引号

//* ADC 按钮配置
#define USE_ADC_BUTTON       // 启用ADC 按钮
#define LONG_PRESS_TIME 0.6f // 长按时间阈值（秒）
#define BUTTON_DELTA_T 0.01f // 按钮检测时间间隔（秒）

//* PID 配置
#define USE_PID           // 启用PID
#define PID_DELTA_T 0.12f // PID 时间间隔（秒）

//* 编码器配置
// #define USE_ENCODER           // 启用编码器
#define ENCODER_TIMER_T 0.01f // 编码器时间间隔（秒）

//* 小车参数配置
// #define USE_CAR                       // 启用小车参数
#define WHEEL_TRACK (0.12855f / 2.0f) // 轮距
#define WHEEL_BASE (0.1117f / 2.0f)   // 轴距
#define WHEEL_RADIUS 0.024f           // 轮子半径
#define WHEEL_Gearbox_Rate 20.0f      // 减速比
#define WHEEL_Per_Pulse 13.0f         // 每转脉冲数
#define WHEEL_Ferq_Rate 4
#define MAX_LINEAR_SPEED 0.2f
#define MAX_ANGULAR_SPEED 2.0f

//* PurePursuit 配置
// #define USE_PURE_PURSUIT
#define POINTS_NUM 20
#define MAX_LOOKAHEAD_DISTANCE 1.5f    // 最大前视距离
#define MIN_LOOKAHEAD_DISTANCE 0.01f   // 最小前视距离
#define MAX_SPEED 0.6f                 // 最大速度（单位：m/s）
#define MAX_CURVATURE 0.2f             // 最大曲率（限制转向角以防止过度转向）
#define LOOKAHEAD_DISTANCE_FACTOR 0.5f // 前视距离与速度的比例因子
#define K_STANLEY_HEADING 1.0f         // Stanley方法中航向误差权重
#define K_STANLEY_CROSSTRACK 1.0f      // Stanley方法中横向误差权重

#endif // __CONFIG_H__