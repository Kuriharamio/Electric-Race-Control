/**
 * @file car.c
 * @author KuriharaMio
 * @brief 小车类实现文件
 * @version 0.1
 * @date 2025-04-11
 * @note 逆天小车，每个轮子千奇百怪，需要慢慢调试
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "car.h"

static Class_Car _Car = {0};

/**
 * @brief 创建小车对象
 *
 * @return Class_Car*
 */
pClass_Car Create_Car(void)
{
    pClass_Car car = &_Car;

    // 创建电机对象
    car->Motor_RB = create_motor(RIGHT_BACK);  // 右后轮
    car->Motor_LB = create_motor(LEFT_BACK);   // 左后轮
    car->Motor_RF = create_motor(RIGHT_FRONT); // 右前轮
    car->Motor_LF = create_motor(LEFT_FRONT);  // 左前轮

    // 创建PID对象
    car->PID_Straight_Position = create_PID(); // 直线位置PID
    car->PID_Linear = create_PID(); // 速度环PID
    car->PID_Angular = create_PID(); // 位置环PID

    // 函数指针赋值
    car->Init = Car_Init;
    car->Kinematic_Forward = Car_Kinematic_Forward;
    car->Kinematic_Inverse = Car_Kinematic_Inverse;
    car->Update_Odom = Car_Update_Odom;
    car->TIM_PID_PeriodElapsedCallback = Car_TIM_PID_PeriodElapsedCallback;

    return car;
}

pClass_Car Get_Car_Handle(void)
{
    return &_Car;
}

/**
 * @brief 小车初始化
 *
 * @param this
 */
void Car_Init(pClass_Car this)
{
    // 参数赋值
    this->Now_Position.x = 0.0f;   // x坐标
    this->Now_Position.y = 0.0f;   // y坐标
    this->Now_Position.yaw = 0.0f; // 偏航角

    this->Target_Position.x = 0.0f;   // 目标x坐标
    this->Target_Position.y = 0.0f;   // 目标y坐标
    this->Target_Position.yaw = 0.0f; // 目标偏航角

    this->Target_Speed.linear_velocity = 0.0f;  // 目标线速度
    this->Target_Speed.angular_velocity = 0.0f; // 目标角速度
    this->Output_Speed.linear_velocity = 0.0f;  // 输出线速度
    this->Output_Speed.angular_velocity = 0.0f; // 输出角速度
    this->Now_Speed.linear_velocity = 0.0f;     // 实际线速度
    this->Now_Speed.angular_velocity = 0.0f;    // 实际角速度

    // 初始化电机
    this->Motor_LF->Init(this->Motor_LF, WHEEL_RADIUS, 1600, 1.5, WHEEL_Gearbox_Rate, WHEEL_Per_Pulse, 4);                                                                 // 初始化电机对象
    this->Motor_LF->PID_Speed->PID_Init(this->Motor_LF->PID_Speed, 2000, 10000.0, 2000.0, 5000.0, 1500, 1600, PID_MOTOR_TIMER_T, 0.05, 0.0, 0.0, 0.0, PID_D_First_ENABLE); // 初始化PID参数
    this->Motor_LF->Configure_IN_1(this->Motor_LF, MOTOR_DRV_LF_IN1_PORT, MOTOR_DRV_LF_IN1_PIN);                                                                     // 配置电机引脚IN1
    this->Motor_LF->Configure_IN_2(this->Motor_LF, MOTOR_DRV_LF_IN2_PORT, MOTOR_DRV_LF_IN2_PIN);                                                                     // 配置电机引脚IN2
    this->Motor_LF->Configure_ENCODER_A(this->Motor_LF, ENCODER_LF_PORT, ENCODER_LF_LF_A_PIN);                                                                       // 配置电机引脚编码器A
    this->Motor_LF->Configure_ENCODER_B(this->Motor_LF, ENCODER_LF_PORT, ENCODER_LF_LF_B_PIN);                                                                       // 配置电机引脚编码器B
    this->Motor_LF->Configure_PWM(this->Motor_LF, PWM_MOTOR_L_INST, GPIO_PWM_MOTOR_L_C0_IDX);                                                                            // 配置电机PWM
    this->Motor_LF->Configure_STBY(this->Motor_LF, MOTOR_DRV_STBY_F_PORT, MOTOR_DRV_STBY_F_PIN);                                                                     // 配置电机待机引脚

    this->Motor_LB->Init(this->Motor_LB, WHEEL_RADIUS, 1600, 1.5, WHEEL_Gearbox_Rate, WHEEL_Per_Pulse, 4);                                                                 // 初始化电机对象
    this->Motor_LB->PID_Speed->PID_Init(this->Motor_LB->PID_Speed, 2000, 10000.0, 2000.0, 5000.0, 1500, 1600, PID_MOTOR_TIMER_T, 0.05, 0.0, 0.0, 0.0, PID_D_First_ENABLE); // 初始化PID参数
    this->Motor_LB->Configure_IN_1(this->Motor_LB, MOTOR_DRV_LB_IN1_PORT, MOTOR_DRV_LB_IN1_PIN);                                                                     // 配置电机引脚IN1
    this->Motor_LB->Configure_IN_2(this->Motor_LB, MOTOR_DRV_LB_IN2_PORT, MOTOR_DRV_LB_IN2_PIN);                                                                     // 配置电机引脚IN2
    this->Motor_LB->Configure_ENCODER_A(this->Motor_LB, ENCODER_LB_PORT, ENCODER_LB_LB_A_PIN);                                                                       // 配置电机引脚编码器A
    this->Motor_LB->Configure_ENCODER_B(this->Motor_LB, ENCODER_LB_PORT, ENCODER_LB_LB_B_PIN);                                                                       // 配置电机引脚编码器B
    this->Motor_LB->Configure_PWM(this->Motor_LB, PWM_MOTOR_L_INST, GPIO_PWM_MOTOR_L_C1_IDX);                                                                            // 配置电机PWM
    this->Motor_LB->Configure_STBY(this->Motor_LB, MOTOR_DRV_STBY_B_PORT, MOTOR_DRV_STBY_B_PIN);                                                                     // 配置电机待机引脚

    this->Motor_RF->Init(this->Motor_RF, WHEEL_RADIUS, 1600, 1.5, WHEEL_Gearbox_Rate, WHEEL_Per_Pulse, 4);                                                                 // 初始化电机对象
    this->Motor_RF->PID_Speed->PID_Init(this->Motor_RF->PID_Speed, 2000, 10000.0, 2000.0, 5000.0, 1500, 1600, PID_MOTOR_TIMER_T, 0.05, 0.0, 0.0, 0.0, PID_D_First_ENABLE); // 初始化PID参数
    this->Motor_RF->Configure_IN_1(this->Motor_RF, MOTOR_DRV_RF_IN1_PORT, MOTOR_DRV_RF_IN1_PIN);                                                                     // 配置电机引脚IN1
    this->Motor_RF->Configure_IN_2(this->Motor_RF, MOTOR_DRV_RF_IN2_PORT, MOTOR_DRV_RF_IN2_PIN);                                                                     // 配置电机引脚IN2
    this->Motor_RF->Configure_ENCODER_A(this->Motor_RF, ENCODER_RF_PORT, ENCODER_RF_RF_A_PIN);                                                                       // 配置电机引脚编码器A
    this->Motor_RF->Configure_ENCODER_B(this->Motor_RF, ENCODER_RF_PORT, ENCODER_RF_RF_B_PIN);                                                                       // 配置电机引脚编码器B
    this->Motor_RF->Configure_PWM(this->Motor_RF, PWM_MOTOR_R_INST, GPIO_PWM_MOTOR_R_C0_IDX);                                                                            // 配置电机PWM
    this->Motor_RF->Configure_STBY(this->Motor_RF, MOTOR_DRV_STBY_F_PORT, MOTOR_DRV_STBY_F_PIN);                                                                     // 配置电机待机引脚

    this->Motor_RB->Init(this->Motor_RB, WHEEL_RADIUS, 1600, 1.5, WHEEL_Gearbox_Rate, WHEEL_Per_Pulse, 4);                                                                 // 初始化电机对象
    this->Motor_RB->PID_Speed->PID_Init(this->Motor_RB->PID_Speed, 2000, 10000.0, 2000.0, 5000.0, 1500, 1600, PID_MOTOR_TIMER_T, 0.05, 0.0, 0.0, 0.0, PID_D_First_ENABLE); // 初始化PID参数
    this->Motor_RB->Configure_IN_1(this->Motor_RB, MOTOR_DRV_RB_IN1_PORT, MOTOR_DRV_RB_IN1_PIN);                                                                     // 配置电机引脚IN1
    this->Motor_RB->Configure_IN_2(this->Motor_RB, MOTOR_DRV_RB_IN2_PORT, MOTOR_DRV_RB_IN2_PIN);                                                                     // 配置电机引脚IN2
    this->Motor_RB->Configure_ENCODER_A(this->Motor_RB, ENCODER_RB_PORT, ENCODER_RB_RB_A_PIN);                                                                       // 配置电机引脚编码器A
    this->Motor_RB->Configure_ENCODER_B(this->Motor_RB, ENCODER_RB_PORT, ENCODER_RB_RB_B_PIN);                                                                       // 配置电机引脚编码器B
    this->Motor_RB->Configure_PWM(this->Motor_RB, PWM_MOTOR_R_INST, GPIO_PWM_MOTOR_R_C1_IDX);                                                                            // 配置电机PWM
    this->Motor_RB->Configure_STBY(this->Motor_RB, MOTOR_DRV_STBY_B_PORT, MOTOR_DRV_STBY_B_PIN);                                                                     // 配置电机待机引脚

    // 初始化PID
    this->PID_Straight_Position->PID_Init(this->PID_Straight_Position, 0.01, 0.01, 0.01, 0.01, 1.3, 1.3, PID_CAR_POSITION_TIMER_T, 0.05, 0.0, 0.0, 0.0, PID_D_First_ENABLE); // 初始化PID参数
    this->PID_Linear->PID_Init(this->PID_Linear, 0.3, 15.0, 15.0, 0.5, 1.3, 1.3, PID_CAR_SPEED_TIMER_T, 0.05, 0.0, 0.0, 0.0, PID_D_First_ENABLE); // 初始化PID参数
    this->PID_Angular->PID_Init(this->PID_Angular, 0.05, 15.0, 15.0, 0.05, 1.3, 1.3, PID_CAR_POSITION_TIMER_T, 0.02, 0.0, 0.0, 0.0, PID_D_First_ENABLE); // 初始化PID参数

    // 初始化完成标志位
    this->is_inited = true;
}

/**
 * @brief 小车运动学正解
 *
 * @param this
 */
void Car_Kinematic_Forward(pClass_Car this)
{
    this->Now_Speed.linear_velocity = (this->Motor_LB->Now_Speed + this->Motor_LF->Now_Speed + this->Motor_RF->Now_Speed + -(this->Motor_RB->Now_Speed)) / 4.0f;
    this->Now_Speed.angular_velocity = (this->Motor_RF->Now_Speed + -(this->Motor_RB->Now_Speed) - this->Motor_LF->Now_Speed - this->Motor_LB->Now_Speed) / (4.0f * (WHEEL_TRACK + WHEEL_BASE));
}

/**
 * @brief 小车运动学逆解
 *
 * @param this
 */
void Car_Kinematic_Inverse(pClass_Car this)
{
    this->Motor_LB->Target_Speed = this->Output_Speed.linear_velocity - this->Output_Speed.angular_velocity * (WHEEL_TRACK + WHEEL_BASE);
    this->Motor_LF->Target_Speed = this->Output_Speed.linear_velocity - this->Output_Speed.angular_velocity * (WHEEL_TRACK + WHEEL_BASE);
    this->Motor_RF->Target_Speed = this->Output_Speed.linear_velocity + this->Output_Speed.angular_velocity * (WHEEL_TRACK + WHEEL_BASE);
    this->Motor_RB->Target_Speed = -(this->Output_Speed.linear_velocity + this->Output_Speed.angular_velocity * (WHEEL_TRACK + WHEEL_BASE));
}

/**
 * @brief 小车里程计更新
 *
 * @param this
 */
void Car_Update_Odom(pClass_Car this)
{
    this->Now_Position.yaw += this->Now_Speed.angular_velocity * ENCODER_TIMER_T;
    this->Now_Position.yaw = TransAngleInPI(this->Now_Position.yaw);

    float delta_distance = this->Now_Speed.linear_velocity * ENCODER_TIMER_T;

    this->Now_Position.x += delta_distance * cos(this->Now_Position.yaw);
    this->Now_Position.y += delta_distance * sin(this->Now_Position.yaw);
}

/**
 * @brief 小车定时器回调函数
 *
 * @param this
 */
void Car_TIM_PID_PeriodElapsedCallback(pClass_Car this)
{
    this->PID_Straight_Position->Set_Target(this->PID_Straight_Position, this->Target_Position.x);
    this->PID_Straight_Position->Set_Now(this->PID_Straight_Position, this->Now_Position.x);
    this->PID_Straight_Position->TIM_Adjust_PeriodElapsedCallback(this->PID_Straight_Position);

    this->Target_Speed.linear_velocity = this->PID_Straight_Position->Get_PID_Out(this->PID_Straight_Position);
    this->Target_Speed.angular_velocity = 0;

    this->PID_Linear->Set_Target(this->PID_Linear, this->Target_Speed.linear_velocity);
    this->PID_Linear->Set_Now(this->PID_Linear, this->Now_Speed.linear_velocity);
    this->PID_Linear->TIM_Adjust_PeriodElapsedCallback(this->PID_Linear);

    this->PID_Angular->Set_Target(this->PID_Angular, this->Target_Speed.angular_velocity);
    this->PID_Angular->Set_Now(this->PID_Angular, this->Now_Speed.angular_velocity);
    this->PID_Angular->TIM_Adjust_PeriodElapsedCallback(this->PID_Angular);

    this->Output_Speed.linear_velocity = (this->PID_Linear->Get_PID_Out(this->PID_Linear));
    this->Output_Speed.angular_velocity = (this->PID_Angular->Get_PID_Out(this->PID_Angular));

    this->Kinematic_Inverse(this);
}
