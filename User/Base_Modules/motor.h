#ifndef __WHEEL_H__
#define __WHEEL_H__

#include "ti_msp_dl_config.h"
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
    // PID速度环控制
    Class_PID *PID_Speed;
    // 目标速度
    double Target_Speed;
    // 实际速度
    double Now_Speed;
    // PWM输出
    double Output_Now;
    double Output_Max;
    double Speed_Max;
    // 编码器计数
    int Encoder_Num_Per_Round;
    int Last_Encoder_Tick;
    int Total_Encoder_Tick;

    double Radius;
    // 引脚定义
    GPIO_Regs* SLP_PORT;
    uint32_t SLP_PIN;

    GPIO_Regs* DIR_PORT;
    uint32_t DIR_PIN;

    GPTIMER_Regs* PWM_INST;
    uint32_t PWM_IDX;

    GPIO_Regs* ENCODER_A_PORT;
    uint32_t ENCODER_A_PIN;
    GPIO_Regs* ENCODER_B_PORT;
    uint32_t ENCODER_B_PIN;

    /**
     * @brief 电机初始化
     *
     */
    void (*Init)(struct Class_Motor *self, double __side, double __radius, double __Output_Max, double __Speed_Max, int __Gearbox_Rate, int __Per_Pulse, int __Frequency_doubling);

    /**
     * @brief TIM定时器中断计算回调函数
     *
     */
    void (*TIM_PID_PeriodElapsedCallback)(struct Class_Motor *self);

    /**
     * @brief 编码器计算回调函数
     *
     * @param phase 编码器相位
     */
    void (*Encoder_Callback)(struct Class_Motor *self, char phase);

    /**
     * @brief 电机控制
     * 
     * @param __DIR 电机方向
     */
    void (*Control)(struct Class_Motor *self, Motor_Dir __DIR);

    /**
     * @brief 输出PWM
     *
     */
    void (*Output)(struct Class_Motor *self);

} Class_Motor;

Class_Motor *create_motor(GPIO_Regs *__SLP_PORT, uint32_t __SLP_PIN, GPTIMER_Regs *__PWM_INST, uint32_t __PWM_IDX, GPIO_Regs *__DIR_PORT, uint32_t __DIR_PIN, GPIO_Regs *__ENCODER_A_PORT, uint32_t __ENCODER_A_PIN, GPIO_Regs *__ENCODER_B_PORT, uint32_t __ENCODER_B_PIN);
void Motor_Init(Class_Motor *self, double __side, double __radius, double __Output_Max, double __Speed_Max, int __Gearbox_Rate, int __Per_Pulse, int __Frequency_doubling);

void Motor_Encoder_Callback(Class_Motor *self, char phase);
void Motor_TIM_PID_PeriodElapsedCallback(Class_Motor *self);
void Motor_Control(Class_Motor *self, Motor_Dir __DIR);
void Motor_Output(Class_Motor *self);

#endif 