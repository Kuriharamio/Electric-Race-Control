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

#ifdef USE_CAR
static Class_Car _Car = {0};

/**
 * @brief 创建小车对象
 *
 * @return Class_Car*
 */
pClass_Car Create_Car(void)
{
    pClass_Car car = &_Car;
#ifdef USE_GRAY_SENSOR
    car->gray_scale_sensor = Create_GraySensor(); // 创建循迹传感器对象
#endif
    // 创建电机对象
    car->Motor_RB = create_motor(RIGHT_BACK);  // 右后轮
    car->Motor_LB = create_motor(LEFT_BACK);   // 左后轮
    car->Motor_RF = create_motor(RIGHT_FRONT); // 右前轮
    car->Motor_LF = create_motor(LEFT_FRONT);  // 左前轮

    // 创建控制器对象
    car->PurePursuit = Create_PurePursuit();

    // 创建PID对象
    car->PID_Linear_Position = create_PID(); // 直线位置PID
    car->PID_Angle_Position_WHEEL = create_PID();
    car->PID_Angle_Position_IMU = create_PID();
    car->PID_Linear = create_PID();  // 速度环PID
    car->PID_Angular = create_PID(); // 角度环PID
    car->PID_Follow = create_PID();

    // 函数指针赋值
    car->Init = Car_Init;
    car->Kinematic_Forward = Car_Kinematic_Forward;
    car->Kinematic_Inverse = Car_Kinematic_Inverse;
    car->Update_Odom = Car_Update_Odom;
    car->Upadate_Controller = Car_Upadate_Controller;

    car->Judge_Mode = Car_Judge_Mode;
    car->Update_Mode = Car_Update_Mode;

    car->Update_Follow_PID = Car_Update_Follow_PID;
    car->Update_Speed_PID = Car_Update_Speed_PID;
    car->Update_Linear_Position_PID = Car_Update_Linear_Position_PID;
    car->Update_Angle_Position_PID_WHEEL = Car_Update_Angle_Position_PID_WHEEL;
    car->Update_Angle_Position_PID_IMU = Car_Update_Angle_Position_PID_IMU;
    car->Update_XY_Position_PID = Car_Update_XY_Position_PID;

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

    this->Begin_Yaw = 0.0f; // 起始IMU偏航角

    this->Target_Speed.linear_velocity = 0.0f;  // 目标线速度
    this->Target_Speed.angular_velocity = 0.0f; // 目标角速度
    this->Output_Speed.linear_velocity = 0.0f;  // 输出线速度
    this->Output_Speed.angular_velocity = 0.0f; // 输出角速度
    this->Now_Speed.linear_velocity = 0.0f;     // 实际线速度
    this->Now_Speed.angular_velocity = 0.0f;    // 实际角速度

    this->Mode = STOP;

    this->Finish_Current_Mode = true;
    this->Task_ID = 0;

#ifdef USE_GRAY_SENSOR
    // 初始化循迹传感器
    this->gray_scale_sensor->Init_With_Params(this->gray_scale_sensor, white, black);
    // 初始化循迹 PID
    this->PID_Follow->PID_Init(this->PID_Follow, 2.0f, 0.0f, 0.1f, 10.0f, MAX_ANGULAR_SPEED * 0.2, MAX_ANGULAR_SPEED, PID_DELTA_T * PID_CAR_FOLLOW_FACTOR, 0.0f, 0.0f, 0.0f, 0.0f, PID_D_First_DISABLE);

#endif
    // 初始化电机
    this->Motor_LF->Init(this->Motor_LF, WHEEL_RADIUS, 1600, 1.5, WHEEL_Gearbox_Rate, WHEEL_Per_Pulse, 4);                                                            // 初始化电机对象
    this->Motor_LF->PID_Speed->PID_Init(this->Motor_LF->PID_Speed, 1000, 8000.0, 10.0, 0.0, 600, 1400, PID_DELTA_T * PID_MOTOR_FACTOR, 0.08, 0.0, 0.0, 0.4, PID_D_First_DISABLE); // 初始化PID参数
    this->Motor_LF->Configure_IN_1(this->Motor_LF, MOTOR_DRV_LF_IN1_PORT, MOTOR_DRV_LF_IN1_PIN);                                                                      // 配置电机引脚IN1
    this->Motor_LF->Configure_IN_2(this->Motor_LF, MOTOR_DRV_LF_IN2_PORT, MOTOR_DRV_LF_IN2_PIN);                                                                      // 配置电机引脚IN2
    this->Motor_LF->Configure_ENCODER_A(this->Motor_LF, ENCODER_LF_PORT, ENCODER_LF_LF_A_PIN);                                                                        // 配置电机引脚编码器A
    this->Motor_LF->Configure_ENCODER_B(this->Motor_LF, ENCODER_LF_PORT, ENCODER_LF_LF_B_PIN);                                                                        // 配置电机引脚编码器B
    this->Motor_LF->Configure_PWM(this->Motor_LF, PWM_MOTOR_L_INST, GPIO_PWM_MOTOR_L_C0_IDX);                                                                         // 配置电机PWM
    this->Motor_LF->Configure_STBY(this->Motor_LF, MOTOR_DRV_STBY_F_PORT, MOTOR_DRV_STBY_F_PIN);                                                                      // 配置电机待机引脚

    this->Motor_LB->Init(this->Motor_LB, WHEEL_RADIUS, 1600, 1.5, WHEEL_Gearbox_Rate, WHEEL_Per_Pulse, 4);                                                            // 初始化电机对象
    this->Motor_LB->PID_Speed->PID_Init(this->Motor_LB->PID_Speed, 1000, 8000.0, 10.0, 0.0, 600, 1400, PID_DELTA_T * PID_MOTOR_FACTOR, 0.08, 0.0, 0.0, 0.4, PID_D_First_DISABLE); // 初始化PID参数
    this->Motor_LB->Configure_IN_1(this->Motor_LB, MOTOR_DRV_LB_IN1_PORT, MOTOR_DRV_LB_IN1_PIN);                                                                      // 配置电机引脚IN1
    this->Motor_LB->Configure_IN_2(this->Motor_LB, MOTOR_DRV_LB_IN2_PORT, MOTOR_DRV_LB_IN2_PIN);                                                                      // 配置电机引脚IN2
    this->Motor_LB->Configure_ENCODER_A(this->Motor_LB, ENCODER_LB_PORT, ENCODER_LB_LB_A_PIN);                                                                        // 配置电机引脚编码器A
    this->Motor_LB->Configure_ENCODER_B(this->Motor_LB, ENCODER_LB_PORT, ENCODER_LB_LB_B_PIN);                                                                        // 配置电机引脚编码器B
    this->Motor_LB->Configure_PWM(this->Motor_LB, PWM_MOTOR_L_INST, GPIO_PWM_MOTOR_L_C1_IDX);                                                                         // 配置电机PWM
    this->Motor_LB->Configure_STBY(this->Motor_LB, MOTOR_DRV_STBY_B_PORT, MOTOR_DRV_STBY_B_PIN);                                                                      // 配置电机待机引脚

    this->Motor_RF->Init(this->Motor_RF, WHEEL_RADIUS, 1600, 1.5, WHEEL_Gearbox_Rate, WHEEL_Per_Pulse, 4);                                                            // 初始化电机对象
    this->Motor_RF->PID_Speed->PID_Init(this->Motor_RF->PID_Speed, 1000, 8000.0, 10.0, 0.0, 600, 1400, PID_DELTA_T * PID_MOTOR_FACTOR, 0.08, 0.0, 0.0, 0.4, PID_D_First_DISABLE); // 初始化PID参数
    this->Motor_RF->Configure_IN_1(this->Motor_RF, MOTOR_DRV_RF_IN1_PORT, MOTOR_DRV_RF_IN1_PIN);                                                                      // 配置电机引脚IN1
    this->Motor_RF->Configure_IN_2(this->Motor_RF, MOTOR_DRV_RF_IN2_PORT, MOTOR_DRV_RF_IN2_PIN);                                                                      // 配置电机引脚IN2
    this->Motor_RF->Configure_ENCODER_A(this->Motor_RF, ENCODER_RF_PORT, ENCODER_RF_RF_A_PIN);                                                                        // 配置电机引脚编码器A
    this->Motor_RF->Configure_ENCODER_B(this->Motor_RF, ENCODER_RF_PORT, ENCODER_RF_RF_B_PIN);                                                                        // 配置电机引脚编码器B
    this->Motor_RF->Configure_PWM(this->Motor_RF, PWM_MOTOR_R_INST, GPIO_PWM_MOTOR_R_C0_IDX);                                                                         // 配置电机PWM
    this->Motor_RF->Configure_STBY(this->Motor_RF, MOTOR_DRV_STBY_F_PORT, MOTOR_DRV_STBY_F_PIN);                                                                      // 配置电机待机引脚

    this->Motor_RB->Init(this->Motor_RB, WHEEL_RADIUS, 1600, 1.5, WHEEL_Gearbox_Rate, WHEEL_Per_Pulse, 4);
    this->Motor_RB->PID_Speed->PID_Init(this->Motor_RB->PID_Speed, 1000, 8000.0, 10.0, 0.0, 600, 1400, PID_DELTA_T * PID_MOTOR_FACTOR, 0.08, 0.0, 0.0, 0.4, PID_D_First_DISABLE); // 初始化PID参数
    this->Motor_RB->Configure_IN_1(this->Motor_RB, MOTOR_DRV_RB_IN1_PORT, MOTOR_DRV_RB_IN1_PIN);                                                                      // 配置电机引脚IN1
    this->Motor_RB->Configure_IN_2(this->Motor_RB, MOTOR_DRV_RB_IN2_PORT, MOTOR_DRV_RB_IN2_PIN);                                                                      // 配置电机引脚IN2
    this->Motor_RB->Configure_ENCODER_A(this->Motor_RB, ENCODER_RB_PORT, ENCODER_RB_RB_A_PIN);                                                                        // 配置电机引脚编码器A
    this->Motor_RB->Configure_ENCODER_B(this->Motor_RB, ENCODER_RB_PORT, ENCODER_RB_RB_B_PIN);                                                                        // 配置电机引脚编码器B
    this->Motor_RB->Configure_PWM(this->Motor_RB, PWM_MOTOR_R_INST, GPIO_PWM_MOTOR_R_C1_IDX);                                                                         // 配置电机PWM
    this->Motor_RB->Configure_STBY(this->Motor_RB, MOTOR_DRV_STBY_B_PORT, MOTOR_DRV_STBY_B_PIN);                                                                      // 配置电机待机引脚

    // 初始化控制器
    this->PurePursuit->Init(this->PurePursuit);

    // 初始化PID
    this->PID_Linear->PID_Init(this->PID_Linear, 0.6f, 10.0f, 0.0f, 0.0f, MAX_LINEAR_SPEED, MAX_LINEAR_SPEED, PID_DELTA_T * PID_CAR_SPEED_FACTOR, 0.010f, 0.0f, 0.0f, 0.0f, PID_D_First_DISABLE);
    this->PID_Angular->PID_Init(this->PID_Angular, 0.6f, 10.0f, 0.0f, 0.0f, MAX_ANGULAR_SPEED, MAX_ANGULAR_SPEED, PID_DELTA_T * PID_CAR_SPEED_FACTOR, 0.050f, 0.0f, 0.0f, 0.00f, PID_D_First_DISABLE);

    this->PID_Linear_Position->PID_Init(this->PID_Linear_Position, 2.2f, 0.08f, 0.05f, 0.4f, MAX_LINEAR_SPEED, MAX_LINEAR_SPEED, PID_DELTA_T * PID_CAR_POS_FACTOR, 0.010f, 0.0f, 0.0f, 0.0f, PID_D_First_DISABLE);
    this->PID_Angle_Position_WHEEL->PID_Init(this->PID_Angle_Position_WHEEL, 2.2f, 0.5f, 0.01f, 12.2f, MAX_ANGULAR_SPEED, MAX_ANGULAR_SPEED, PID_DELTA_T * PID_CAR_POS_FACTOR, 0.010f, 0.0f, 0.0f, 0.0f, PID_D_First_DISABLE);
    this->PID_Angle_Position_IMU->PID_Init(this->PID_Angle_Position_IMU, 2.2f, 0.5f, 0.01f, 12.2f, MAX_ANGULAR_SPEED, MAX_ANGULAR_SPEED, PID_DELTA_T * PID_CAR_POS_FACTOR, 0.010f, 0.0f, 0.0f, 0.0f, PID_D_First_DISABLE);


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
    this->Now_Speed.linear_velocity = (this->Motor_LB->Now_Speed + this->Motor_LF->Now_Speed + this->Motor_RF->Now_Speed + (-(this->Motor_RB->Now_Speed))) / 4.0f;
    this->Now_Speed.angular_velocity = (this->Motor_RF->Now_Speed + (-(this->Motor_RB->Now_Speed)) - this->Motor_LF->Now_Speed - this->Motor_LB->Now_Speed) / (4.0f * (WHEEL_TRACK + WHEEL_BASE));
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
void Car_Update_Odom(pClass_Car this, float dt)
{
#ifdef USE_IMU_IN_ODOM
    this->Now_Position.yaw = this->IMU_Yaw;
    float delta_distance = this->Now_Speed.linear_velocity * dt;
    this->Now_Position.x += delta_distance * cos(this->Now_Position.yaw);
    this->Now_Position.y += delta_distance * sin(this->Now_Position.yaw);
#else
    this->Now_Position.yaw += this->Now_Speed.angular_velocity * dt;
    this->Now_Position.yaw = TransAngleInPI(this->Now_Position.yaw);

    float delta_distance = this->Now_Speed.linear_velocity * dt;

    this->Now_Position.x += delta_distance * cos(this->Now_Position.yaw);
    this->Now_Position.y += delta_distance * sin(this->Now_Position.yaw);
#endif

    this->Current_Mode_Position.yaw += this->Now_Speed.angular_velocity * dt;
    this->Current_Mode_Position.yaw = TransAngleInPI(this->Current_Mode_Position.yaw);

    delta_distance = this->Now_Speed.linear_velocity * dt;

    this->Current_Mode_Position.x += delta_distance * cos(this->Current_Mode_Position.yaw);
    this->Current_Mode_Position.y += delta_distance * sin(this->Current_Mode_Position.yaw);
}

/**
 * @brief 直线位置PID
 *
 * @param this
 */
void Car_Update_Linear_Position_PID(pClass_Car this)
{
    this->PID_Linear_Position->Set_Target(this->PID_Linear_Position, this->Target_Position.x);
    this->PID_Linear_Position->Set_Now(this->PID_Linear_Position, this->Current_Mode_Position.x);
    this->PID_Linear_Position->Update_Value(this->PID_Linear_Position);

    this->Target_Speed.linear_velocity = this->PID_Linear_Position->Get_PID_Out(this->PID_Linear_Position);
}

/**
 * @brief 航向角PID
 *
 * @param this
 */
void Car_Update_Angle_Position_PID_WHEEL(pClass_Car this)
{
    float error_yaw = TransAngleInPI(this->Target_Position.yaw - this->Now_Position.yaw);
    this->PID_Angle_Position_WHEEL->Set_Target(this->PID_Angle_Position_WHEEL, error_yaw);
    this->PID_Angle_Position_WHEEL->Set_Now(this->PID_Angle_Position_WHEEL, 0);
    this->PID_Angle_Position_WHEEL->Update_Value(this->PID_Angle_Position_WHEEL);

    this->Target_Speed.angular_velocity = this->PID_Angle_Position_WHEEL->Get_PID_Out(this->PID_Angle_Position_WHEEL);
}

void Car_Update_Angle_Position_PID_IMU(pClass_Car this)
{
    float error_yaw = TransAngleInPI(this->Target_Position.yaw - this->IMU_Yaw);

    this->PID_Angle_Position_IMU->Set_Target(this->PID_Angle_Position_IMU, error_yaw);
    this->PID_Angle_Position_IMU->Set_Now(this->PID_Angle_Position_IMU, 0);
    this->PID_Angle_Position_IMU->Update_Value(this->PID_Angle_Position_IMU);

    this->Target_Speed.angular_velocity = this->PID_Angle_Position_IMU->Get_PID_Out(this->PID_Angle_Position_IMU);
}

/**
 * @brief xy坐标位置PID
 *
 * @param this
 */
void Car_Update_XY_Position_PID(pClass_Car this)
{
    this->PID_Linear_Position->Set_Target(this->PID_Linear_Position, this->Target_Position.x);
    this->PID_Linear_Position->Set_Now(this->PID_Linear_Position, this->Current_Mode_Position.x);
    this->PID_Linear_Position->Update_Value(this->PID_Linear_Position);
    float linear_pid = this->PID_Linear_Position->Get_PID_Out(this->PID_Linear_Position);

    float predict_x = this->Current_Mode_Position.x + this->Now_Speed.linear_velocity * cos(this->Current_Mode_Position.yaw) * PID_CAR_POS_FACTOR * PID_DELTA_T * 2;
    float predict_y = this->Current_Mode_Position.y + this->Now_Speed.linear_velocity * sin(this->Current_Mode_Position.yaw) * PID_CAR_POS_FACTOR * PID_DELTA_T * 2;

    float predict_dx = this->Target_Position.x - predict_x;
    float predict_dy = this->Target_Position.y - predict_y;

    this->Target_Position.yaw = atan2f(predict_dy, predict_dx);
    float error_yaw = TransAngleInPI(this->Target_Position.yaw - this->Current_Mode_Position.yaw);
    this->PID_Angle_Position_WHEEL->Set_Target(this->PID_Angle_Position_WHEEL, error_yaw);
    this->PID_Angle_Position_WHEEL->Set_Now(this->PID_Angle_Position_WHEEL, 0);
    this->PID_Angle_Position_WHEEL->Update_Value(this->PID_Angle_Position_WHEEL);
    float angular_pid = this->PID_Angle_Position_WHEEL->Get_PID_Out(this->PID_Angle_Position_WHEEL);

    this->Target_Speed.linear_velocity = linear_pid * cosf(this->Target_Position.yaw);
    this->Target_Speed.angular_velocity = angular_pid + (linear_pid * sinf(this->Target_Position.yaw));

}

#ifdef USE_GRAY_SENSOR
/**
 * @brief 循迹PID
 *
 * @param this
 */
void Car_Update_Follow_PID(pClass_Car this)
{
    this->PID_Follow->Set_Target(this->PID_Follow, 0.0);
    this->PID_Follow->Set_Now(this->PID_Follow, this->gray_scale_sensor->Follow_Error);
    this->PID_Follow->Update_Value(this->PID_Follow);

    float max_speed = MAX_ANGULAR_SPEED * (WHEEL_TRACK + WHEEL_BASE);
    this->Target_Speed.angular_velocity = this->PID_Follow->Get_PID_Out(this->PID_Follow);
    this->Target_Speed.linear_velocity = max_speed - fabs(this->Target_Speed.angular_velocity * (WHEEL_TRACK + WHEEL_BASE));

    Math_Constrain_float(&this->Target_Speed.linear_velocity, -10.0f, this->gray_scale_sensor->Linear_Speed_Max);

    this->Target_Speed.linear_velocity = -this->Target_Speed.linear_velocity;
    // Car_Kinematic_Inverse(this);
}
#endif


/**
 * @brief 速度PID
 *
 * @param this
 */
void Car_Update_Speed_PID(pClass_Car this)
{
    // 线速度PID
    this->PID_Linear->Set_Target(this->PID_Linear, this->Target_Speed.linear_velocity);
    this->PID_Linear->Set_Now(this->PID_Linear, this->Now_Speed.linear_velocity);
    this->PID_Linear->Update_Value(this->PID_Linear);

    // 角速度PID
    this->PID_Angular->Set_Target(this->PID_Angular, this->Target_Speed.angular_velocity);
    this->PID_Angular->Set_Now(this->PID_Angular, this->Now_Speed.angular_velocity);
    this->PID_Angular->Update_Value(this->PID_Angular);

    // 获取输出
    this->Output_Speed.linear_velocity = (this->PID_Linear->Get_PID_Out(this->PID_Linear));
    this->Output_Speed.angular_velocity = (this->PID_Angular->Get_PID_Out(this->PID_Angular));
    this->Kinematic_Inverse(this);
}

/**
 * @brief 小车模式更新
 *
 * @param this
 */
void Car_Update_Mode(pClass_Car this, CONTROL_MODE mode)
{
    this->Current_Mode_Position.x = 0.0f;
    this->Current_Mode_Position.y = 0.0f;
    this->Current_Mode_Position.yaw = 0.0f;

    this->Mode = mode;
}

/**
 * @brief 小车模式判断
 *
 * @param this
 */
void Car_Judge_Mode(pClass_Car this)
{
    static uint8_t Task_1_Step = 0;
    static uint8_t Task_2_Step = 0;
    static uint8_t Task_3_Step = 0;
    

    switch (this->Mode) 
    {
    case STOP:
        this->Target_Speed.linear_velocity = 0.0f;
        this->Target_Speed.angular_velocity = 0.0f;
        this->Finish_Current_Mode = true;
        break;
    case POSISITON_LA_Circle:
        if (fabs(this->Target_Position.x - this->Current_Mode_Position.x) < 0.03f && fabs(TransAngleInPI(this->Target_Position.yaw - this->Now_Position.yaw)) < 0.03f)
        {
            this->Finish_Current_Mode = true;
        }
        break;
    case POSISITON_XY_Circle:
        if (fabs(this->Target_Position.x - this->Current_Mode_Position.x) < 0.02f && fabs(this->Target_Position.y - this->Current_Mode_Position.y) < 0.02f)
        {
            this->Finish_Current_Mode = true;
        }
        break;
    case FOLLOW_Circle:
        if(this->gray_scale_sensor->Finish){
            this->Finish_Current_Mode = true;
            this->gray_scale_sensor->Finish = false;
        }
        break;
    default:
        break;
    }

    if (!this->Finish_Current_Mode)
        return;

    this->Finish_Current_Mode = false;
    switch (this->Task_ID)
    {
    case 1:
        Task_1_Step++;
        switch (Task_1_Step)
        {
        case 1: //* 直走
#ifdef USE_IMU_IN_ANGULAR_PID
            this->Begin_Yaw = this->IMU_Yaw;
#else
            this->Begin_Yaw = this->Now_Position.yaw;
#endif
            this->Target_Position.x = 0.6f;
            this->Target_Position.y = 0.0f;
            this->Target_Position.yaw = this->Begin_Yaw;

            this->Update_Mode(this, POSISITON_LA_Circle);
            break;
        case 3: //* 入库
            this->Update_Mode(this, TRAJECTORY_3);
            break;
        case 4://* 转回原角度
            this->Target_Position.x = -0.08f;
            this->Target_Position.y = 0.0f;
            this->Target_Position.yaw = this->Begin_Yaw;
            this->Update_Mode(this, POSISITON_LA_Circle);
            break;
        case 5://* 结束
            this->Update_Mode(this, STOP);
            Task_1_Step--;
            break;
        default:
            this->Finish_Current_Mode = true;
            break;
        }
        break;
    case 2:
        Task_2_Step++;
        switch (Task_2_Step)
        {
        case 1: //* 直走
#ifdef USE_IMU_IN_ANGULAR_PID
            this->Begin_Yaw = this->IMU_Yaw;
#else
            this->Begin_Yaw = this->Now_Position.yaw;
#endif
            this->Target_Position.x = -1.0f;
            this->Target_Position.y = 0.0f;
            this->Target_Position.yaw = this->Begin_Yaw;

            this->Update_Mode(this, POSISITON_LA_Circle);
            break;
        case 2: //* 巡线
            this->Update_Mode(this, FOLLOW_Circle);
            break;
        case 3: //* 转向相反方向
            this->Target_Position.x = 0.0f;
            this->Target_Position.y = 0.0f;
            this->Target_Position.yaw = this->Begin_Yaw + PI;
            break;
        case 4: //* 直走
            this->Target_Position.x = -1.0f;
            this->Target_Position.y = 0.0f;
            this->Target_Position.yaw = this->Begin_Yaw + PI;

            this->Update_Mode(this, POSISITON_LA_Circle);
            break;
        case 5: //* 巡线
            this->Update_Mode(this, FOLLOW_Circle);
            break;
        case 6: //* 转回原角度
            this->Target_Position.x = 0.0f;
            this->Target_Position.y = 0.0f;
            this->Target_Position.yaw = this->Begin_Yaw;

            this->Update_Mode(this, POSISITON_LA_Circle);
            break;
        case 7: //* 结束
            this->Update_Mode(this, STOP);
            Task_2_Step--;
            break;
        default:
            break;
        }
        break;
    case 3:
        Task_3_Step++;
        switch (Task_3_Step) 
        {
        case 1: //* 转
#ifdef USE_IMU_IN_ANGULAR_PID
            this->Begin_Yaw = this->IMU_Yaw;
#else
            this->Begin_Yaw = this->Now_Position.yaw;
#endif
            this->Target_Position.x = 0.0f;
            this->Target_Position.y = 0.0f;
            this->Target_Position.yaw = this->Begin_Yaw - atan2f(1.0f, 0.8f);

            this->Update_Mode(this, POSISITON_LA_Circle);
            break;
        case 2: //* 直走
            this->Target_Position.x = -sqrt(0.8f * 0.8f + 1.0f * 1.0f);
            this->Target_Position.y = 0.0f;
            this->Target_Position.yaw = this->Begin_Yaw - atan2f(1.0f, 0.8f);

            this->Update_Mode(this, POSISITON_LA_Circle);
            break;


        case 3: //* 巡线
            this->Update_Mode(this, FOLLOW_Circle);
            break;
        case 4: //* 转向相反方向
            this->Target_Position.x = 0.0f;
            this->Target_Position.y = 0.0f;
            this->Target_Position.yaw = this->Begin_Yaw + PI - atan2f(1.0f, 0.8f);
            break;
        case 5: //* 直走
            this->Target_Position.x = -sqrt(0.8f * 0.8f + 1.0f * 1.0f);
            this->Target_Position.y = 0.0f;
            this->Target_Position.yaw = this->Begin_Yaw + PI - atan2f(1.0f, 0.8f);

            this->Update_Mode(this, POSISITON_LA_Circle);
            break;
        case 6: //* 巡线
            this->Update_Mode(this, FOLLOW_Circle);
            break;
        case 7: //* 转回原角度
            this->Target_Position.x = 0.0f;
            this->Target_Position.y = 0.0f;
            this->Target_Position.yaw = this->Begin_Yaw;

            this->Update_Mode(this, POSISITON_LA_Circle);
            break;
        case 8: //* 结束
            this->Update_Mode(this, STOP);
            Task_2_Step--;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

/**
 * @brief 控制器
 *
 * @param this
 */
void Car_Upadate_Controller(pClass_Car this)
{
    // 启动控制
    this->PurePursuit->Set_Mode(this->PurePursuit, this->Mode);
    this->PurePursuit->Now_Position = this->Current_Mode_Position;
    this->PurePursuit->Now_Speed = this->Now_Speed;
    if(this->PurePursuit->Calculate_Target_Speed(this->PurePursuit)){
        // 更新小车目标速度
        this->Target_Speed = this->PurePursuit->Get_Output_Speed(this->PurePursuit);
    }else{
        this->Mode = STOP;
        this->PurePursuit->Set_Mode(this->PurePursuit, STOP);
    }
}


#endif