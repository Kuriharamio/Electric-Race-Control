#include "car.h"

static Class_Car _Car = {0};

/**
 * @brief 创建小车对象
 * 
 * @return Class_Car* 
 */
pClass_Car create_car(void)
{
    pClass_Car car = &_Car;

    // 创建电机对象
    car->Motor_RB = create_motor(RIGHT_BACK);  // 右后轮
    car->Motor_LB = create_motor(LEFT_BACK);   // 左后轮
    car->Motor_RF = create_motor(RIGHT_FRONT); // 右前轮
    car->Motor_LF = create_motor(LEFT_FRONT);  // 左前轮

    // 创建PID对象
    car->PID_Speed = create_PID(); // 速度环PID
    car->PID_Position = create_PID(); // 位置环PID

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

    // 初始化电机

    // 初始化PID

    // 初始化完成标志位
}


/**
 * @brief 小车运动学正解
 * 
 * @param this 
 */
void Car_Kinematic_Forward(pClass_Car this)
{
    this->Now_Speed.linear_velocity = (this->Motor_LB->Now_Speed + this->Motor_LF->Now_Speed + this->Motor_RF->Now_Speed + this->Motor_RB->Now_Speed) / 4.0f;
    this->Now_Speed.angular_velocity = (this->Motor_RF->Now_Speed + this->Motor_RB->Now_Speed - this->Motor_LF->Now_Speed - this->Motor_LB->Now_Speed) / (4.0f * (WHEEL_TRACK + WHEEL_BASE));
}

/**
 * @brief 小车运动学逆解
 * 
 * @param this 
 */
void Car_Kinematic_Inverse(pClass_Car this)
{
    this->Motor_LB->Target_Speed = this->Target_Speed.linear_velocity - this->Target_Speed.angular_velocity * (WHEEL_TRACK + WHEEL_BASE);
    this->Motor_LF->Target_Speed = this->Target_Speed.linear_velocity - this->Target_Speed.angular_velocity * (WHEEL_TRACK + WHEEL_BASE);
    this->Motor_RF->Target_Speed = this->Target_Speed.linear_velocity + this->Target_Speed.angular_velocity * (WHEEL_TRACK + WHEEL_BASE);
    this->Motor_RB->Target_Speed = this->Target_Speed.linear_velocity + this->Target_Speed.angular_velocity * (WHEEL_TRACK + WHEEL_BASE);
}

/**
 * @brief 小车里程计更新
 * 
 * @param this 
 */
void Car_Update_Odom(pClass_Car this)
{
    this->Position.yaw += this->Now_Speed.angular_velocity * ENCODER_TIMER_T;
    this->Position.yaw = TransAngleInPI(this->Position.yaw);

    float delta_distance = this->Now_Speed.linear_velocity * ENCODER_TIMER_T;

    this->Position.x += delta_distance * cos(this->Position.yaw);
    this->Position.y += delta_distance * sin(this->Position.yaw);
}

/**
 * @brief 小车定时器回调函数
 * 
 * @param this 
 */ 
void Car_TIM_PID_PeriodElapsedCallback(pClass_Car this)
{

}
