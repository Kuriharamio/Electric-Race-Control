#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "User/config.h"
#include "Algorithm/pid.h"

typedef enum
{
    FORWARD,
    BACKWARD,
    BRAKE
} Motor_Dir;

typedef enum
{
    LEFT_BACK,
    LEFT_FRONT,
    RIGHT_BACK,
    RIGHT_FRONT,
} SIDE;

typedef struct Class_Motor
{
    SIDE Side;
    Motor_Dir Direction;
    bool is_inited;
    // PID速度环控制
    Class_PID *PID_Speed;
    // 目标速度
    float Target_Speed;
    // 实际速度
    float Now_Speed;
    // PWM输出
    float Output_Now;
    float Output_Max;
    float Speed_Max;
    // 编码器计数
    int Encoder_Num_Per_Round;
    int Last_Encoder_Tick;
    int Total_Encoder_Tick;

    float Radius;
    // 引脚定义
    GPIO_Regs *IN_1_PORT;
    uint32_t IN_1_PIN;

    GPIO_Regs *IN_2_PORT;
    uint32_t IN_2_PIN;

    GPIO_Regs *STBY_PORT;
    uint32_t STBY_PIN;

    GPTIMER_Regs *PWM_INST;
    uint32_t PWM_IDX;

    GPIO_Regs *ENCODER_PORT;
    GPIO_Regs *ENCODER_A_PORT;
    uint32_t ENCODER_A_PIN;
    GPIO_Regs *ENCODER_B_PORT;
    uint32_t ENCODER_B_PIN;

    void (*Init)(struct Class_Motor *this, float __radius, float __Output_Max, float __Speed_Max, int __Gearbox_Rate, int __Per_Pulse, int __Frequency_doubling); // 电机初始化

    void (*Configure_IN_1)(struct Class_Motor *this, GPIO_Regs *__IN_1_PORT, uint32_t __IN_1_PIN);                // 电机引脚IN1配置
    void (*Configure_IN_2)(struct Class_Motor *this, GPIO_Regs *__IN_2_PORT, uint32_t __IN_2_PIN);                // 电机引脚IN2配置
    void (*Configure_STBY)(struct Class_Motor *this, GPIO_Regs *__STBY_PORT, uint32_t __STBY_PIN);                // 电机待机引脚配置
    void (*Configure_PWM)(struct Class_Motor *this, GPTIMER_Regs *__PWM_INST, uint32_t __PWM_IDX);                // 电机PWM配置
    void (*Configure_ENCODER_A)(struct Class_Motor *this, GPIO_Regs *__ENCODER_A_PORT, uint32_t __ENCODER_A_PIN); // 电机引脚编码器A配置
    void (*Configure_ENCODER_B)(struct Class_Motor *this, GPIO_Regs *__ENCODER_B_PORT, uint32_t __ENCODER_B_PIN); // 电机引脚编码器B配置

    void (*TIM_PID_PeriodElapsedCallback)(struct Class_Motor *this); // 定时器回调函数
    void (*Encoder_Callback)(struct Class_Motor *this, char phase);  // 编码器回调函数
    void (*Control)(struct Class_Motor *this);                       // 电机输入引脚控制函数
    void (*Output)(struct Class_Motor *this);                        // 电机PWM输出函数

} Class_Motor, *pClass_Motor;

pClass_Motor create_motor(SIDE side);
pClass_Motor Get_Motor_INST(SIDE side);

void Motor_Configure_IN_1(pClass_Motor this, GPIO_Regs *__IN_1_PORT, uint32_t __IN_1_PIN);
void Motor_Configure_IN_2(pClass_Motor this, GPIO_Regs *__IN_2_PORT, uint32_t __IN_2_PIN);
void Motor_Configure_STBY(pClass_Motor this, GPIO_Regs *__STBY_PORT, uint32_t __STBY_PIN);
void Motor_Configure_PWM(pClass_Motor this, GPTIMER_Regs *__PWM_INST, uint32_t __PWM_IDX);
void Motor_Configure_ENCODER_A(pClass_Motor this, GPIO_Regs *__ENCODER_A_PORT, uint32_t __ENCODER_A_PIN);
void Motor_Configure_ENCODER_B(pClass_Motor this, GPIO_Regs *__ENCODER_B_PORT, uint32_t __ENCODER_B_PIN);

void Motor_Init(pClass_Motor this, float __radius, float __Output_Max, float __Speed_Max, int __Gearbox_Rate, int __Per_Pulse, int __Frequency_doubling);

void Motor_Encoder_Callback(pClass_Motor this, char phase);
void Motor_TIM_PID_PeriodElapsedCallback(pClass_Motor this);
void Motor_Control(pClass_Motor this);
void Motor_Output(pClass_Motor this);

#endif  // __MOTOR_H__