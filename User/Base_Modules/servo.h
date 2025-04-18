#ifndef __SERVO_H_
#define __SERVO_H_

#include "config.h"

typedef struct Class_Servo
{
    uint32_t ARR;
    GPTIMER_Regs *PWM_INST;
    uint32_t PWM_IDX;
    float angle_max;

    /**
     * @brief 初始化舵机
     *
     * @param this 舵机结构体指针
     * @param angle 初始角度
     */
    void (*Init)(struct Class_Servo *this, GPTIMER_Regs *PWM_INST, uint32_t PWM_IDX, uint32_t ARR, float angle_max);

    /**
     * @brief 将角度转换为CCR
     *
     * @param this 舵机结构体指针
     * @param angle 角度
     */
    uint32_t (*angle_to_CCR)(struct Class_Servo *this, float angle);

    /**
     * @brief 舵机输出
     *
     * @param this 舵机结构体指针
     */
    void (*Set_Angle)(struct Class_Servo *this, float angle);

} Class_Servo, *pClass_Servo;

pClass_Servo create_Servo(uint8_t index);
void Servo_Init(pClass_Servo this, GPTIMER_Regs *PWM_INST, uint32_t PWM_IDX, uint32_t ARR, float angle_max);
void Servo_Set_Angle(pClass_Servo this, float angle);
uint32_t Servo_angle_to_CCR(pClass_Servo this, float angle);

#endif