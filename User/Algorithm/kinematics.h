#ifndef __KINEMATICS_H__
#define __KINEMATICS_H__

#include "Algorithm/drv_math.h"

typedef struct
{
    float x;
    float y;
    float yaw;
} POSITION;

void kinematic_forward(float wheel1_speed, float wheel2_speed, float wheel3_speed, float wheel4_speed, float wheel_distance_, float *linear_speed, float *angular_speed);
void kinematic_inverse(float linear_speed, float angular_speed, float wheel_distance_, float *out_wheel1_speed, float *out_wheel2_speed, float *out_wheel3_speed, float *out_wheel4_speed);
POSITION kinematic_update_odom(float dt, float linear_speed, float angular_speed, POSITION pos);

#endif