#include "Algorithm/pure_pursuit.h"

#ifdef USE_PURE_PURSUIT
// <<<<<<<< 实例声明 <<<<<<<<
static Class_PurePursuit _PURE_PURSUIT = {};
// >>>>>>>> 实例声明 >>>>>>>>

// Smooth Left Turn Trajectory Points
static const float trajectory_1[POINTS_NUM][2] = {
    {0.000000, 0.000000},
    {0.030540, 0.000521},
    {0.054356, 0.001843},
    {0.073189, 0.003607},
    {0.088782, 0.005450},
    {0.102875, 0.007013},
    {0.117211, 0.007935},
    {0.133532, 0.007855},
    {0.153578, 0.006412},
    {0.200000, 0.000000},
    {0.212634, -0.004123},
    {0.264597, -0.040954},
    {0.333815, -0.107808},
    {0.415359, -0.196107},
    {0.504298, -0.297273},
    {0.595702, -0.402727},
    {0.684641, -0.503893},
    {0.766185, -0.592192},
    {0.835403, -0.659046},
    {0.900000, -0.700000},
    {0.920907, -0.703246},
    {0.946422, -0.706412},
    {0.966468, -0.707855},
    {0.982789, -0.707935},
    {0.997125, -0.707013},
    {1.011218, -0.705450},
    {1.026811, -0.703607},
    {1.045644, -0.701843},
    {1.069460, -0.700521},
    {1.100000, -0.700000},
};
// Smooth Right Turn Trajectory Points
static const float trajectory_2[POINTS_NUM][2] = {
    {0.000000, 0.000000},
    {0.030540, -0.000521},
    {0.054356, -0.001843},
    {0.073189, -0.003607},
    {0.088782, -0.005450},
    {0.102875, -0.007013},
    {0.117211, -0.007935},
    {0.133532, -0.007855},
    {0.153578, -0.006412},
    {0.200000, 0.000000},
    {0.212634, 0.004123},
    {0.264597, 0.040954},
    {0.333815, 0.107808},
    {0.415359, 0.196107},
    {0.504298, 0.297273},
    {0.595702, 0.402727},
    {0.684641, 0.503893},
    {0.766185, 0.592192},
    {0.835403, 0.659046},
    {0.900000, 0.700000},
    {0.920907, 0.703246},
    {0.946422, 0.706412},
    {0.966468, 0.707855},
    {0.982789, 0.707935},
    {0.997125, 0.707013},
    {1.011218, 0.705450},
    {1.026811, 0.703607},
    {1.045644, 0.701843},
    {1.069460, 0.700521},
    {1.100000, 0.700000},
};
// Custom Trajectory Points
static const float trajectory_3[POINTS_NUM][2] = {
    {0.000000, 0.000000},
    {0.031936, 0.004577},
    {0.059801, 0.016598},
    {0.085125, 0.033493},
    {0.109439, 0.052695},
    {0.134276, 0.071636},
    {0.161166, 0.087749},
    {0.200000, 0.100000},
    {0.227129, 0.106058},
    {0.267298, 0.119793},
    {0.310551, 0.137641},
    {0.355273, 0.157034},
    {0.399844, 0.175404},
    {0.442648, 0.190182},
    {0.500000, 0.200000},
    {0.517934, 0.198802},
    {0.557352, 0.190182},
    {0.600156, 0.175404},
    {0.644727, 0.157034},
    {0.689449, 0.137641},
    {0.732702, 0.119793},
    {0.800000, 0.100000},
    {0.808358, 0.098465},
    {0.838834, 0.087749},
    {0.865724, 0.071636},
    {0.890561, 0.052695},
    {0.914875, 0.033493},
    {0.940199, 0.016598},
    {0.968064, 0.004577},
    {1.000000, 0.000000},
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
        this->Trajectory = trajectory_1;
        break;
    case TRAJECTORY_2:
        this->Trajectory = trajectory_2;
        // memcpy(this->Trajectory, trajectory_right, sizeof(trajectory_right));
        break;
    case TRAJECTORY_3:
        this->Trajectory = trajectory_3;
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