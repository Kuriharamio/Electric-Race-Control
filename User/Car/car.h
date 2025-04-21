#ifndef __CAR_H__
#define __CAR_H__

#include "Base_Modules/motor.h"
#include "Algorithm/pure_pursuit.h"
typedef struct Class_Car
{
    // 电机对象
    pClass_Motor Motor_RB; // 右后轮
    pClass_Motor Motor_LB; // 左后轮
    pClass_Motor Motor_RF; // 右前轮
    pClass_Motor Motor_LF; // 左前轮

    // 控制器
    pClass_PurePursuit PurePursuit; //  PurePursuit对象
    CONTROL_MODE Mode;              // 控制模式

    // PID对象
    pClass_PID PID_Straight_Position; // 直线位置PID
    pClass_PID PID_Angle_Position;

    pClass_PID PID_Linear;            // 速度环PID
    pClass_PID PID_Angular;           // 角度环PID
    pClass_PID PID_Follow;

    // 里程计
    POSITION Target_Position; // 目标位置
    POSITION Now_Position;    // 里程计位置
    float imu_yaw;
    float Target_imu_yaw;

    // 速度
    SPEED Target_Speed; // 目标速度
    SPEED Output_Speed; // 输出速度
    SPEED Now_Speed;    // 实际速度

    float follow_error;


    bool is_inited; // 是否初始化完成

    void (*Init)(struct Class_Car *this);
    void (*Kinematic_Forward)(struct Class_Car *this);
    void (*Kinematic_Inverse)(struct Class_Car *this);
    void (*Update_Odom)(struct Class_Car *this);

    void (*Upadate_Controller)(struct Class_Car *this);

    void (*Update_Follow_PID)(struct Class_Car *this);
    void (*Update_Straight_Position_PID)(struct Class_Car *this);
    void (*Update_Angle_Position_PID)(struct Class_Car *this);
    void (*Update_Speed_PID)(struct Class_Car *this);

} Class_Car, *pClass_Car;

pClass_Car Create_Car(void);
pClass_Car Get_Car_Handle(void);

void Car_Init(pClass_Car this);
void Car_Kinematic_Forward(pClass_Car this);
void Car_Kinematic_Inverse(pClass_Car this);
void Car_Update_Odom(pClass_Car this);

void Car_Update_Follow_PID(pClass_Car this);
void Car_Update_Straight_Position_PID(pClass_Car this);
void Car_Update_Angle_Position_PID(pClass_Car this);
void Car_Update_Speed_PID(pClass_Car this);
void Car_Upadate_Controller(pClass_Car this);

#endif // __CAR_H__