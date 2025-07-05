#include "Algorithm/pure_pursuit.h"

#ifdef USE_PURE_PURSUIT
// <<<<<<<< 实例声明 <<<<<<<<
static Class_PurePursuit _PURE_PURSUIT = {};
// >>>>>>>> 实例声明 >>>>>>>>

// Smooth Left Turn Trajectory Points
static const float trajectory_left[POINTS_NUM][2] = {
    {0.000, 0.000},
    {0.050, 0.000},
    {0.098, 0.002},
    {0.143, 0.004},
    {0.185, 0.006},
    {0.224, 0.010},
    {0.262, 0.015},
    {0.296, 0.022},
    {0.329, 0.029},
    {0.359, 0.038},
    {0.387, 0.048},
    {0.413, 0.060},
    {0.436, 0.073},
    {0.458, 0.087},
    {0.478, 0.104},
    {0.496, 0.122},
    {0.513, 0.142},
    {0.527, 0.164},
    {0.540, 0.187},
    {0.552, 0.213},
    {0.562, 0.241},
    {0.571, 0.271},
    {0.578, 0.304},
    {0.585, 0.338},
    {0.590, 0.376},
    {0.594, 0.415},
    {0.596, 0.457},
    {0.598, 0.502},
    {0.600, 0.550},
    {0.600, 0.600},
};

// Smooth Right Turn Trajectory Points
static const float trajectory_right[POINTS_NUM][2] = {
    {0.000, 0.000},
    {0.050, -0.000},
    {0.098, -0.002},
    {0.143, -0.004},
    {0.185, -0.006},
    {0.224, -0.010},
    {0.262, -0.015},
    {0.296, -0.022},
    {0.329, -0.029},
    {0.359, -0.038},
    {0.387, -0.048},
    {0.413, -0.060},
    {0.436, -0.073},
    {0.458, -0.087},
    {0.478, -0.104},
    {0.496, -0.122},
    {0.513, -0.142},
    {0.527, -0.164},
    {0.540, -0.187},
    {0.552, -0.213},
    {0.562, -0.241},
    {0.571, -0.271},
    {0.578, -0.304},
    {0.585, -0.338},
    {0.590, -0.376},
    {0.594, -0.415},
    {0.596, -0.457},
    {0.598, -0.502},
    {0.600, -0.550},
    {0.600, -0.600},
};

// Custom Trajectory Points
static const float trajectory_custom[POINTS_NUM][2] = {
    {0.000000, 0.000000},
    {0.036538, 0.011329},
    {0.076566, 0.042063},
    {0.119155, 0.087323},
    {0.163379, 0.142228},
    {0.208306, 0.201899},
    {0.253010, 0.261456},
    {0.296561, 0.316020},
    {0.338031, 0.360710},
    {0.400000, 0.400000},
    {0.411526, 0.402176},
    {0.448875, 0.407772},
    {0.489625, 0.411969},
    {0.532803, 0.414766},
    {0.577439, 0.416165},
    {0.622561, 0.416165},
    {0.667197, 0.414766},
    {0.710375, 0.411969},
    {0.751125, 0.407772},
    {0.800000, 0.400000},
    {0.823509, 0.390648},
    {0.861969, 0.360710},
    {0.903439, 0.316020},
    {0.946990, 0.261456},
    {0.991694, 0.201899},
    {1.036621, 0.142228},
    {1.080845, 0.087323},
    {1.123434, 0.042063},
    {1.163462, 0.011329},
    {1.200000, 0.000000},
};

pClass_PurePursuit Create_PurePursuit()
{
    pClass_PurePursuit pure_pursuit = &_PURE_PURSUIT;
    pure_pursuit->Init = PurePursuit_Init;
    pure_pursuit->Set_Mode = PurePursuit_Set_Mode;
    pure_pursuit->Calculate_Target_Speed = PurePursuit_Calculate_Target_Speed;
    pure_pursuit->Get_Output_Speed = PurePursuit_Get_Output_Speed;
    pure_pursuit->Find_Closest_Point = PurePursuit_Find_Closest_Point;
    pure_pursuit->Find_Lookahead_Point = PurePursuit_Find_Lookahead_Point;
    pure_pursuit->Trajectory = NULL;
    return pure_pursuit;
}

void PurePursuit_Init(pClass_PurePursuit this)
{
    this->Control_Dir = STOP;
    this->Target_Speed.linear_velocity = 0.0f;
    this->Target_Speed.angular_velocity = 0.0f;
    this->Now_Speed.linear_velocity = 0.0f;
    this->Now_Speed.angular_velocity = 0.0f;
    this->Target_Position.x = 0.0f;
    this->Target_Position.y = 0.0f;
    this->Target_Position.yaw = 0.0f;
    this->Now_Position.x = 0.0f;
    this->Now_Position.y = 0.0f;
    this->Now_Position.yaw = 0.0f;
    this->last_closest_index = -1;
    this->last_angular_velocity = 0.0f;
    this->finish_current_mode = true;
    this->is_inited = true;
}

SPEED PurePursuit_Get_Output_Speed(pClass_PurePursuit this)
{
    return this->Target_Speed;
}

void PurePursuit_Set_Mode(pClass_PurePursuit this, CONTROL_MODE dir)
{
    this->Control_Dir = dir;

    switch (this->Control_Dir)
    {
    case TRAJECTORY_1:
        // memcpy(this->Trajectory, trajectory_left, sizeof(trajectory_left));
        this->Trajectory = trajectory_left;
        break;
    case TRAJECTORY_2:
        this->Trajectory = trajectory_right;
        // memcpy(this->Trajectory, trajectory_right, sizeof(trajectory_right));
        break;
    case TRAJECTORY_3:
        this->Trajectory = trajectory_custom;
        // memcpy(this->Trajectory, trajectory_custom, sizeof(trajectory_custom));
        break;
    default:
        this->finish_current_mode = true;
        this->Now_Position.x = 0.0f;
        this->Now_Position.y = 0.0f;
        this->Now_Position.yaw = 0.0f;
        this->Target_Speed.linear_velocity = 0.0f;
        this->Target_Speed.angular_velocity = 0.0f;
        this->Control_Dir = STOP;
    }
    
}

// 计算最近点的索引
int PurePursuit_Find_Closest_Point(pClass_PurePursuit this, float *min_dist_sq)
{
    *min_dist_sq = 10;
    int closest_index = -1;

    for (int i = 0; i < POINTS_NUM; i++)
    {
        float dx = this->Trajectory[i][0] - this->Now_Position.x;
        float dy = this->Trajectory[i][1] - this->Now_Position.y;
        float dist_sq = dx * dx + dy * dy;
        if (dist_sq < *min_dist_sq)
        {
            *min_dist_sq = dist_sq;
            closest_index = i;
        }
    }
    return closest_index;
}

// 遍历轨迹点，寻找第一个超出前视距离的点
int PurePursuit_Find_Lookahead_Point(pClass_PurePursuit this, float lookahead_distance)
{
    int lookahead_point_index = -1;
    float min_distance_sq = lookahead_distance * lookahead_distance;

    for (int i = 0; i < POINTS_NUM; i++)
    {
        float dx = this->Trajectory[i][0] - this->Now_Position.x;
        float dy = this->Trajectory[i][1] - this->Now_Position.y;

        // 将轨迹点转换到车辆坐标系
        float local_x = dx * cos(-this->Now_Position.yaw) - dy * sin(-this->Now_Position.yaw);
        float local_y = dx * sin(-this->Now_Position.yaw) + dy * cos(-this->Now_Position.yaw);

        // 只考虑车辆前方的点
        if (local_x > 0)
        {
            float distance_sq = local_x * local_x + local_y * local_y;
            if (distance_sq >= min_distance_sq)
            {
                lookahead_point_index = i;
                break;
            }
        }
    }

    return lookahead_point_index;
}


bool PurePursuit_Calculate_Target_Speed(pClass_PurePursuit this)
{
    if (this->Trajectory == NULL || this->Control_Dir == STOP)
        return false;

    const float *last_point = this->Trajectory[POINTS_NUM - 1];
    float dx_end = last_point[0] - this->Now_Position.x;
    float dy_end = last_point[1] - this->Now_Position.y;
    float distance_to_end = hypotf(dx_end, dy_end);
    if (distance_to_end < 0.04f)
    {
        return false;
    }
    float min_dist_sq = INFINITY;
    int closest_index = -1;
    const int search_start = (this->last_closest_index > 0) ? this->last_closest_index : 0;

    for (int i = search_start; i < POINTS_NUM; i++)
    {
        float dx = this->Trajectory[i][0] - this->Now_Position.x;
        float dy = this->Trajectory[i][1] - this->Now_Position.y;
        float dist_sq = dx * dx + dy * dy;

        if (dist_sq < min_dist_sq)
        {
            min_dist_sq = dist_sq;
            closest_index = i;
        }
    }

    if (closest_index == -1)
    {
        return false;
    }

    this->last_closest_index = closest_index;

    int target_index = POINTS_NUM - 1;
    float accum_dist = 0.0f;
    for (int i = closest_index; i < POINTS_NUM - 1; i++)
    {
        float dx = this->Trajectory[i + 1][0] - this->Trajectory[i][0];
        float dy = this->Trajectory[i + 1][1] - this->Trajectory[i][1];
        accum_dist += hypotf(dx, dy);

        if (accum_dist >= LOOKAHEAD_DISTANCE)
        {
            target_index = i + 1;
            break;
        }
    }

    float dx = this->Trajectory[target_index][0] - this->Now_Position.x;
    float dy = this->Trajectory[target_index][1] - this->Now_Position.y;
    float alpha = atan2f(dy, dx) - this->Now_Position.yaw;

    alpha = fmodf(alpha + PI, 2 * PI) - PI;

    float dist = hypotf(dx, dy);
    if (dist < 1e-6f)
        return false;
    float curvature = 2.0f * sinf(alpha) / dist;

    this->Target_Speed.linear_velocity = TARGET_SPEED / (1.0f + K_CURVATURE * fabsf(curvature));

    if (distance_to_end < DECELERATION_START)
    {
        float factor = (distance_to_end) / DECELERATION_START;
        factor = fmaxf(factor, 0.0f); 
        this->Target_Speed.linear_velocity *= factor;
        
    }

    float new_angular = this->Target_Speed.linear_velocity * curvature;
    this->Target_Speed.angular_velocity = FILTER_GAIN * this->last_angular_velocity + (1 - FILTER_GAIN) * new_angular;
    this->last_angular_velocity = this->Target_Speed.angular_velocity;

    return true;
}

#endif