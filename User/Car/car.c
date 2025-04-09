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
    car->kinematic_forward = Car_kinematic_forward;
    car->kinematic_inverse = Car_kinematic_inverse;
    car->kinematic_update_odom = Car_kinematic_update_odom;
    car->TIM_PID_PeriodElapsedCallback = Car_TIM_PID_PeriodElapsedCallback;
    
    return car;
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


void Car_kinematic_forward(pClass_Car this)
{
    // *linear_speed = (wheel1_speed + wheel2_speed + wheel3_speed + wheel4_speed) / 4.0;
    // *angular_speed = (wheel1_speed - wheel2_speed + wheel3_speed - wheel4_speed) / (4.0 * wheel_distance_);
}

void Car_kinematic_inverse(pClass_Car this)
{
    // *out_wheel1_speed = linear_speed + (angular_speed * wheel_distance_) / 2.0;
    // *out_wheel2_speed = linear_speed - (angular_speed * wheel_distance_) / 2.0;
    // *out_wheel3_speed = linear_speed + (angular_speed * wheel_distance_) / 2.0;
    // *out_wheel4_speed = linear_speed - (angular_speed * wheel_distance_) / 2.0;
}

void Car_kinematic_update_odom(pClass_Car this)
{
    // POSITION odom_ = pos;
    // odom_.yaw += angular_speed * dt;

    // odom_.yaw = TransAngleInPI(odom_.yaw);

    // float delta_distance = linear_speed * dt;

    // odom_.x += delta_distance * cos(pos.yaw);
    // odom_.y += delta_distance * sin(pos.yaw);
    // return odom_;
}

/**
 * @brief 小车定时器回调函数
 * 
 * @param this 
 */ 
void Car_TIM_PID_PeriodElapsedCallback(pClass_Car this)
{

}
