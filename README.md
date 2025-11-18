# Electric-Race-Control 电赛控制题备赛代码库

## 项目概述

本仓库是基于TI MSPM0G3507微控制器的电赛控制题备赛代码，采用模块化设计和面向对象的C语言编程思想，构建了一套可灵活配置、易于扩展的嵌入式控制系统。代码结构清晰，重点实现了下位机核心控制逻辑，适用于各类智能车、机器人等竞赛场景。

## 硬件平台

- **主控制器**：TI MSPM0G3507
- **支持外设**：
  - 飞特舵机（PTZ云台控制）
  - 蓝牙模块（无线调试与通信）
  - 灰度传感器（巡迹）
  - IMU惯性测量单元（姿态感知）
  - 电机驱动（运动控制）
  - ADC按键（人机交互）
  - 树莓派通信接口（上下位机串口通信）

## 代码结构

```
Electric-Race-Control/
├── User/                           # 核心应用代码
│   ├── config.h                    # 系统配置宏定义
│   ├── Base_Modules/               # 基础模块实现
│   │   ├── uart.c/.h               # 串口通信模块
│   │   ├── imu.c/.h                # 惯性测量单元驱动
│   │   ├── motor.c/.h              # 电机控制模块
│   │   ├── ptz.c/.h                # 云台控制模块
│   │   ├── bluetooth.c/.h          # 蓝牙通信模块
│   │   ├── FT_servo.c/.h           # 飞特舵机模块
│   │   ├── hmi.c/.h                # 串口屏模块
│   │   ├── follow_sensor.c/.h      # 感为灰度传感器模块
│   │   ├── servo.c/.h              # PWM舵机模块
│   │   ├── raspberry_serial.c/.h   # 树莓派串口通信
│   │   └── adc_button.c/.h         # ADC按键模块
│   └── Car/                        # 小车控制相关
│       └── car.c/.h                # 小车控制逻辑
├── BSP/                            # 板级支持包
│   ├── board.c/.h                  # 板级初始化
│   ├── uart.c/.h                   # 串口硬件驱动
│   ├── i2c.c/.h                    # i2c硬件驱动
│   ├── gpio.c/.h                   # gpio硬件驱动
│   ├── delay.c/.h                  # 延时函数
│   └── timer.c/.h                  # 定时器驱动
├── Algorithm/                      # 算法库
│   ├── drv_math.c/.h               # 数学工具函数
│   ├── pid.c/.h                    # PID 算法
│   └── pure_pursuit.c/.h           # Pure Pursuit 算法
├── Utils/                          # 工具函数（用于生成路径，给小车跟踪）
└── K230/                           # 上位机视觉代码
└── Raspberry_pi/                   # 上位机视觉代码
```

## 核心设计思想

### 1. 面向对象的C语言实现

通过结构体封装数据和函数指针，模拟类的概念，实现了模块化和信息隐藏：

```c
// 串口类定义示例
typedef struct Class_UART {
    // 成员变量
    uint8_t index;
    uint8_t rx_max_len;
    uint8_t rx_len;
    uint8_t* rxbuffer;
    uint8_t current_byte;
    bool is_inited;
    UART_MODE mode;
    uint8_t param_len;
    float**param_list;
    char** param_name;
    
    // 成员函数指针
    void (*Init)(struct Class_UART* this, uint8_t rx_max_len, uint8_t param_len);
    void (*Send_Bit)(struct Class_UART* this, uint8_t data);
    void (*Send_Datas)(struct Class_UART* this, uint8_t* datas, size_t size);
    // ... 更多函数
} Class_UART, *pClass_UART;
```

### 2. 模块化设计

每个功能模块独立封装，通过统一接口交互，降低耦合度：
- 硬件抽象层（BSP）：隔离硬件细节
- 基础模块层：提供标准化功能接口
- 应用层：实现具体业务逻辑

### 3. 灵活的配置机制

通过`config.h`中的宏定义实现功能裁剪和参数配置：

```c
// 功能模块开关
#define USE_LED         // 启用LED
#define USE_BLUETOOTH   // 启用蓝牙
#define USE_PTZ         // 启用云台
#define USE_ADC_BUTTON  // 启用ADC按钮

// 硬件参数配置
#define BLUETOOTH_UART_INDEX 0  // 蓝牙串口索引号
#define SERVO_DOWN_INDEX 2      // 下舵机索引号
#define LONG_PRESS_TIME 0.6f    // 长按时间阈值（秒）
```

## 关键模块说明

### 1. 串口通信模块（uart.c）

实现了通用串口通信框架，支持多种工作模式（调试波形、原始数据、HMI交互），提供：
- 面向对象的串口操作接口
- 数据发送/接收封装
- 中断处理与回调机制
- 参数绑定与自动发送

### 2. 云台控制模块（ptz.c）

基于飞特舵机的云台控制系统，支持：
- 舵机位置精确控制
- 限位保护
- PID闭环控制
- 预设位置保存与调用

### 3. 传感器模块

- **IMU模块**：提供姿态数据采集与校准
- **灰度传感器**：支持路径识别与线跟踪
- **ADC按键**：实现多按键检测，支持短按/长按事件

### 4. 小车控制模块（car.c）

实现智能车运动控制核心逻辑，包括：
- 电机速度闭环控制
- 路径跟踪算法
- 运动学模型

## 使用方法

1. 根据硬件配置修改`User/config.h`中的宏定义
2. 在`main.c`中初始化所需模块：
   ```c
   // 初始化示例
   pClass_Car Car = Create_Car();
   Car->Init(Car);
   
   pClass_PTZ PTZ = Create_PTZ();
   PTZ->Init(PTZ);
   ```
3. 根据需求实现具体控制逻辑
4. 通过宏定义启用/禁用特定功能模块

## 扩展建议

1. 新增模块时，建议遵循现有面向对象框架
2. 硬件接口变更只需修改BSP层对应驱动
3. 可通过`config.h`添加自定义宏定义扩展功能
4. 复杂算法建议放在Algorithm目录下

## 注意事项

- 确保各模块初始化顺序正确
- 串口索引与硬件连接需对应
- 舵机参数需根据实际硬件校准
- 中断处理函数应尽量简短，避免阻塞

本代码库提供了电赛控制题的基础框架，可根据具体竞赛题目要求进行扩展和优化。
