/**
 * @file kinematics.c
 * @brief 四驱车运动学解算
 * @version 0.1
 * @date 2025-04-06
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "Algorithm/kinematics.h"

/**
 * @brief 四驱车运动学正解算
 * 
 * @param wheel1_speed 
 * @param wheel2_speed 
 * @param wheel3_speed 
 * @param wheel4_speed 
 * @param wheel_distance_ 
 * @param linear_speed 
 * @param angular_speed 
 */
void kinematic_forward(float wheel1_speed, float wheel2_speed, float wheel3_speed, float wheel4_speed, float wheel_distance_, float *linear_speed, float *angular_speed)
{
    *linear_speed = (wheel1_speed + wheel2_speed + wheel3_speed + wheel4_speed) / 4.0;
    *angular_speed = (wheel1_speed - wheel2_speed + wheel3_speed - wheel4_speed) / (4.0 * wheel_distance_); 
}

/**
 * @brief 四驱车运动学逆解算
 * 
 * @param linear_speed 
 * @param angular_speed 
 * @param wheel_distance_ 
 * @param out_wheel1_speed 
 * @param out_wheel2_speed 
 * @param out_wheel3_speed 
 * @param out_wheel4_speed 
 */
void kinematic_inverse(float linear_speed, float angular_speed, float wheel_distance_, float *out_wheel1_speed, float *out_wheel2_speed, float *out_wheel3_speed, float *out_wheel4_speed)
{
    *out_wheel1_speed = linear_speed + (angular_speed * wheel_distance_) / 2.0;
    *out_wheel2_speed = linear_speed - (angular_speed * wheel_distance_) / 2.0;
    *out_wheel3_speed = linear_speed + (angular_speed * wheel_distance_) / 2.0;
    *out_wheel4_speed = linear_speed - (angular_speed * wheel_distance_) / 2.0;
}


/**
 * @brief 更新里程计
 * 
 * @param dt 时间间隔
 * @param linear_speed 线速度
 * @param angular_speed 角速度
 * @param pos 当前位姿
 * @return POSITION 更新后的位姿
 */
POSITION kinematic_update_odom(float dt, float linear_speed, float angular_speed, POSITION pos)
{
    POSITION odom_ = pos;
    odom_.yaw += angular_speed * dt;

    odom_.yaw = TransAngleInPI(odom_.yaw);

    float delta_distance = linear_speed * dt; 

    odom_.x += delta_distance * cos(pos.yaw);
    odom_.y += delta_distance * sin(pos.yaw);
    return odom_;
}

