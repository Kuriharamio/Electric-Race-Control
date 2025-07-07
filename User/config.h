#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "ti_msp_dl_config.h"
#include "Algorithm/drv_math.h"
#include <stdlib.h>
#include <string.h>
//* 声光提示配置
#define USE_LED  // 启用LED
#define USE_BUZZ // 启用蜂鸣器

//* 蓝牙配置
#define USE_BLUETOOTH
#define BLUETOOTH_UART_INDEX 0  // 蓝牙串口索引号
#define USE_UART_0              // 启用串口0
#define BLUETOOTH_RX_LEN_MAX 20 // 蓝牙接收数据最大长度

//* 飞特舵机串口配置
// #define USE_FT_SERVO          // 启用飞特舵机
// #define FT_SERVO_UART_INDEX 0 // 飞特舵机串口索引号
// #define USE_UART_0            // 启用串口0
// #define FT_SERVO_RX_LEN_MAX 100 // 飞特舵机接收数据最大长度
// #define PROTECT_CURRENT 30 // 保护电流大小

//* K230 串口配置
// #define USE_K230
// #define K230_UART_INDEX 1   // K230串口索引号
// #define USE_UART_1          // 启用串口1
// #define K230_RX_LEN_MAX 100 // K230接收数据最大长度

//* IMU 串口配置
// #define USE_IMU
// #define USE_IMU_IN_ODOM
// #define USE_IMU_IN_ANGULAR_PID
// #define IMU_UART_INDEX 2  // IMU 串口索引号
// #define USE_UART_2        // 启用串口2
// #define IMU_RX_LEN_MAX 11 // IMU 接收数据最大长度

//* 舵机配置
// #define USE_SERVO          // 启用舵机
// #define SERVO_UP_INDEX 0   // 上舵机索引号
// #define SERVO_DOWN_INDEX 1 // 下舵机索引号

//* ADC 按钮配置
// #define USE_ADC_BUTTON       // 启用ADC 按钮
// #define LONG_PRESS_TIME 0.6f // 长按时间阈值（秒）
// #define BUTTON_DELTA_T 0.01f // 按钮检测时间间隔（秒）

//* 灰度传感器配置
#define USE_GRAY_SENSOR
#define FILTER_SIZE 1

//* PID 配置
#define USE_PID                 // 启用PID
#define PID_DELTA_T 0.03f      // PID 时间间隔（秒）
#define PID_MOTOR_FACTOR 1      // 电机 PID 时间间隔比值
#define PID_CAR_SPEED_FACTOR 2  // 小车速度 PID 时间间隔比值
#define PID_CAR_POS_FACTOR 3    // 小车轮速 PID 时间间隔比值
#define PID_CAR_FOLLOW_FACTOR 3 // 小车跟随 PID 时间间隔比值

//* 编码器配置
#define USE_ENCODER           // 启用编码器
#define ENCODER_TIMER_T 0.02f // 编码器时间间隔（秒）

//* 小车参数配置
#define USE_CAR                       // 启用小车参数
#define WHEEL_TRACK (0.12855f / 2.0f) // 轮距
#define WHEEL_BASE (0.1117f / 2.0f)   // 轴距
#define WHEEL_RADIUS 0.024f           // 轮子半径
#define WHEEL_Gearbox_Rate 20.0f      // 减速比
#define WHEEL_Per_Pulse 13.0f         // 每转脉冲数
#define WHEEL_Ferq_Rate 4
#define MAX_LINEAR_SPEED 0.8f
#define MAX_ANGULAR_SPEED 0.3/(WHEEL_TRACK + WHEEL_BASE)// 1.5 * 3.14f

//* PurePursuit 配置
#define USE_PURE_PURSUIT
#define POINTS_NUM 30
#define LOOKAHEAD_DISTANCE 0.2f // 前视距离
#define TARGET_SPEED 0.6f       // 基准目标速度
#define DECELERATION_START 0.2f // 减速开始距离
#define K_CURVATURE 0.5f        // 曲率对速度的影响系数
#define FILTER_GAIN 0.3f        // 角速度滤波系数

#endif // __CONFIG_H__