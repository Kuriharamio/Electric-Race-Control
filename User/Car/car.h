#ifndef __CAR_H__
#define __CAR_H__

#include "Base_Modules/motor.h"

typedef struct
{
    float x;
    float y;
    float yaw;
}Axis;

typedef struct Class_Car
{
    // 电机对象
    pClass_Motor Motor_RB; // 右后轮
    pClass_Motor Motor_LB; // 左后轮
    pClass_Motor Motor_RF; // 右前轮
    pClass_Motor Motor_LF; // 左前轮    

    // PID对象
    pClass_PID PID_Speed; // 速度环PID
    pClass_PID PID_Position; // 位置环PID

    // 里程计
    Axis Position; // 里程计位置

    // 速度
    Axis Speed; // 线速度和角速度

    float wheel_distance_; // 轮距


    void (*Init)(struct Class_Car *this);
    void (*kinematic_forward)(struct Class_Car *this);
    void (*kinematic_inverse)(struct Class_Car *this);
    void (*kinematic_update_odom)(struct Class_Car *this);
    void (*TIM_PID_PeriodElapsedCallback)(struct Class_Car *this);

} Class_Car, *pClass_Car;

pClass_Car create_car(void);

void Car_Init(pClass_Car this);
void Car_kinematic_forward(pClass_Car this);
void Car_kinematic_inverse(pClass_Car this);
void Car_kinematic_update_odom(pClass_Car this);
void Car_TIM_PID_PeriodElapsedCallback(pClass_Car this);

#endif // __CAR_H__