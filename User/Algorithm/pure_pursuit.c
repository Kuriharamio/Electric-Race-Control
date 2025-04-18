#include "Algorithm/pure_pursuit.h"

// <<<<<<<< 实例声明 <<<<<<<<
static Class_PurePursuit _PURE_PURSUIT = {};
// >>>>>>>> 实例声明 >>>>>>>>

// Smooth Left Turn Trajectory Points
static const float trajectory_left[POINTS_NUM][2] = {
    {0.000, 0.000},
    {0.016, -0.000},
    {0.030, -0.000},
    {0.043, -0.000},
    {0.053, 0.000},
    {0.063, 0.001},
    {0.071, 0.002},
    {0.078, 0.003},
    {0.083, 0.005},
    {0.088, 0.008},
    {0.092, 0.012},
    {0.095, 0.017},
    {0.097, 0.022},
    {0.098, 0.029},
    {0.099, 0.037},
    {0.100, 0.047},
    {0.100, 0.057},
    {0.100, 0.070},
    {0.100, 0.084},
    {0.100, 0.100},
};

// Smooth Right Turn Trajectory Points
static const float trajectory_right[POINTS_NUM][2] = {
    {0.000, 0.000},
    {0.016, 0.000},
    {0.030, 0.000},
    {0.043, 0.000},
    {0.053, -0.000},
    {0.063, -0.001},
    {0.071, -0.002},
    {0.078, -0.003},
    {0.083, -0.005},
    {0.088, -0.008},
    {0.092, -0.012},
    {0.095, -0.017},
    {0.097, -0.022},
    {0.098, -0.029},
    {0.099, -0.037},
    {0.100, -0.047},
    {0.100, -0.057},
    {0.100, -0.070},
    {0.100, -0.084},
    {0.100, -0.100},
};

pClass_PurePursuit Create_PurePursuit()
{
    pClass_PurePursuit pure_pursuit = &_PURE_PURSUIT;
    pure_pursuit->Init = PurePursuit_Init;
    pure_pursuit->Update_Now_Speed = PurePursuit_Update_Now_Speed;
    pure_pursuit->Set_Mode = PurePursuit_Set_Mode;
    pure_pursuit->Calculate_Target_Speed = PurePursuit_Calculate_Target_Speed;
    pure_pursuit->Get_Output_Speed = PurePursuit_Get_Output_Speed;
    pure_pursuit->Find_Closest_Point = PurePursuit_Find_Closest_Point;
    pure_pursuit->Find_Lookahead_Point = PurePursuit_Find_Lookahead_Point;

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
    this->finish_current_mode = true;
    this->is_inited = true;
}
void PurePursuit_Update_Now_Speed(pClass_PurePursuit this, SPEED speed)
{
    this->Now_Speed = speed;
    this->Now_Position.yaw += this->Now_Speed.angular_velocity * ENCODER_TIMER_T;
    this->Now_Position.yaw = TransAngleInPI(this->Now_Position.yaw);
    float delta_distance = this->Now_Speed.linear_velocity * ENCODER_TIMER_T;
    this->Now_Position.x += delta_distance * cos(this->Now_Position.yaw);
    this->Now_Position.y += delta_distance * sin(this->Now_Position.yaw);
}

SPEED PurePursuit_Get_Output_Speed(pClass_PurePursuit this)
{
    return this->Target_Speed;
}

void PurePursuit_Set_Mode(pClass_PurePursuit this, CONTROL_MODE dir)
{
    this->Control_Dir = dir;
    // if (this->finish_current_mode)
    // {
    //     switch (this->Control_Dir)
    //     {
    //     case TURN_LEFT:
    //         memcpy(this->Trajectory, trajectory_left, sizeof(float) * POINTS_NUM * 2);
    //         break;
    //     case TURN_RIGHT:
    //         memcpy(this->Trajectory, trajectory_right, sizeof(float) * POINTS_NUM * 2);
    //         break;
    //     default:
    //         break;
    //     }
    //     this->finish_current_mode = false;
    // }
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

// bool PurePursuit_Calculate_Target_Speed(pClass_PurePursuit this)
// {
//     if(this->Control_Dir == STOP){
//         this->finish_current_mode = true;

//         this->Now_Position.x = 0.0f;
//         this->Now_Position.y = 0.0f;
//         this->Now_Position.yaw = 0.0f;
//         this->Target_Speed.linear_velocity = 0.0f;
//         this->Target_Speed.angular_velocity = 0.0f;

//         return false;
//     }
//     // 动态计算前视距离，基于当前速度
//     float lookahead_distance = fmax(MIN_LOOKAHEAD_DISTANCE, fmin(MAX_LOOKAHEAD_DISTANCE, LOOKAHEAD_DISTANCE_FACTOR * this->Now_Speed.linear_velocity));

//     int lookahead_point_index = this->Find_Lookahead_Point(this, lookahead_distance);

//     // 如果未找到合适的前视点，则停止车辆
//     if (lookahead_point_index == -1)
//     {
//         this->finish_current_mode = true;

//         this->Now_Position.x = 0.0f;
//         this->Now_Position.y = 0.0f;
//         this->Now_Position.yaw = 0.0f;
//         this->Target_Speed.linear_velocity = 0.0f;
//         this->Target_Speed.angular_velocity = 0.0f;
//         return false;
//     }

//     // 获取前视点在车辆坐标系中的位置
//     float dx = this->Trajectory[lookahead_point_index][0] - this->Now_Position.x;
//     float dy = this->Trajectory[lookahead_point_index][1] - this->Now_Position.y;

//     float local_x = dx * cos(-this->Now_Position.yaw) - dy * sin(-this->Now_Position.yaw);
//     float local_y = dx * sin(-this->Now_Position.yaw) + dy * cos(-this->Now_Position.yaw);

//     // 计算Pure Pursuit曲率
//     float curvature_pure_pursuit = (2.0f * local_y) / (lookahead_distance * lookahead_distance);

//     // 限制曲率以防止过度转向
//     if (curvature_pure_pursuit > MAX_CURVATURE)
//         curvature_pure_pursuit = MAX_CURVATURE;
//     else if (curvature_pure_pursuit < -MAX_CURVATURE)
//         curvature_pure_pursuit = -MAX_CURVATURE;

//     // 计算Pure Pursuit转向角
//     float steering_angle_pure_pursuit = atan(WHEEL_BASE * curvature_pure_pursuit);

//     // // ********** Stanley方法部分 **********
//     // // 计算最近点的索引和距离
//     // float min_dist_sq = 0.0f;
//     // int closest_index = this->Find_Closest_Point(this, &min_dist_sq);
//     // if (closest_index == -1 || min_dist_sq == 0.0f)
//     // {
//     //     this->Set_Mode(this, STOP);
//     //     this->Target_Speed.linear_velocity = 0.0f;
//     //     this->Target_Speed.angular_velocity = 0.0f;
//     //     return false;
//     // }

//     // // 计算横向误差（cross-track error）
//     // float cross_track_error = sqrtf(min_dist_sq);
//     // // 判断横向误差的符号
//     // float path_heading = atan2(this->Trajectory[closest_index][1] - this->Trajectory[closest_index > 0 ? closest_index - 1 : 0][1],
//     //                            this->Trajectory[closest_index][0] - this->Trajectory[closest_index > 0 ? closest_index - 1 : 0][0]);
//     // float heading_error = normalize_angle(this->Now_Position.yaw - path_heading);

//     // // 确定横向误差的符号
//     // float sign = (this->Trajectory[closest_index][0] - this->Now_Position.x) * sin(this->Now_Position.yaw) -
//     //              (this->Trajectory[closest_index][1] - this->Now_Position.yaw) * cos(this->Now_Position.yaw);
//     // if (sign < 0)
//     //     cross_track_error = -cross_track_error;

//     // // 计算Stanley方法的角度调整
//     // float theta = normalize_angle(-heading_error);
//     // float steering_angle_stanley = K_STANLEY_HEADING * theta + atan2(K_STANLEY_CROSSTRACK * cross_track_error, this->Now_Speed.linear_velocity + 1e-5f); // 加小量避免除零

//     // // ********** 综合Pure Pursuit和Stanley方法的角度 **********
//     // // 通过加权平均综合两种方法的转向角
//     // float alpha = 0.5f; // 权重因子，可根据需要调整
//     // float steering_angle = alpha * steering_angle_pure_pursuit + (1.0f - alpha) * steering_angle_stanley;

//     // 计算综合曲率
//     float curvature = tan(steering_angle_pure_pursuit) / WHEEL_BASE;

//     // 限制曲率以防止过度转向
//     if (curvature > MAX_CURVATURE)
//         curvature = MAX_CURVATURE;
//     else if (curvature < -MAX_CURVATURE)
//         curvature = -MAX_CURVATURE;

//     // 根据曲率调整目标速度
//     float target_speed = MAX_SPEED / (1.0f + fabs(curvature));

//     // 设置目标速度
//     this->Target_Speed.linear_velocity = fmin(target_speed, MAX_SPEED);
//     this->Target_Speed.angular_velocity = this->Target_Speed.linear_velocity * curvature;

//     return true;

//     // 如果需要，可以在此处添加速度平滑或加速度限制
// }

bool PurePursuit_Calculate_Target_Speed(pClass_PurePursuit this)
{
    switch (this->Control_Dir)
    {
    case TURN_LEFT:
        memcpy(this->Trajectory, trajectory_left, sizeof(float) * POINTS_NUM * 2);
        break;
    case TURN_RIGHT:
        memcpy(this->Trajectory, trajectory_right, sizeof(float) * POINTS_NUM * 2);
        break;
    default:
        this->finish_current_mode = true;
        this->Now_Position.x = 0.0f;
        this->Now_Position.y = 0.0f;
        this->Now_Position.yaw = 0.0f;
        this->Target_Speed.linear_velocity = 0.0f;
        this->Target_Speed.angular_velocity = 0.0f;
        this->Control_Dir = STOP;
        return false;
    }

    if (fabs(this->Now_Position.x - this->Trajectory[POINTS_NUM - 1][0]) < 1e-2 && fabs(this->Now_Position.y - this->Trajectory[POINTS_NUM - 1][1]) < 1e-2)
    {
        this->Control_Dir = STOP;
        return false;
    }

    float lookahead_x = this->Now_Position.x + MIN_LOOKAHEAD_DISTANCE * cos(this->Now_Position.yaw);
    float lookahead_y = this->Now_Position.y + MIN_LOOKAHEAD_DISTANCE * sin(this->Now_Position.yaw);

    float min_dist_sq = 10;
    int closest_point_index = -1;

    for (int i = 0; i < POINTS_NUM; i++)
    {
        float dist_sq = (this->Trajectory[i][0] - lookahead_x) * (this->Trajectory[i][0] - lookahead_x) + (this->Trajectory[i][1] - lookahead_y) * (this->Trajectory[i][1] - lookahead_y);
        if (dist_sq < min_dist_sq)
        {
            min_dist_sq = dist_sq;
            closest_point_index = i;
        }
    }

    if (closest_point_index == -1)
    {
        this->Control_Dir = STOP;
        return false;
    }

    float alpha = atan2(this->Trajectory[closest_point_index][1] - this->Now_Position.y, this->Trajectory[closest_point_index][0] - this->Now_Position.x) - this->Now_Position.yaw;
    float dist = sqrtf(min_dist_sq);

    if (dist < 1e-6)
    {
        this->Control_Dir = STOP;
        return false;
    }

    float curvature = 2 * sin(alpha) / dist;

    // if(curvature > MAX_CURVATURE)
    //     curvature = MAX_CURVATURE;

    float target_speed = 0.2f;
    this->Target_Speed.linear_velocity = target_speed;
    this->Target_Speed.angular_velocity = target_speed * curvature;

    return true;
}

// void Pure_Pursuit_Controller(pClass_PurePursuit this, CONTROL_MODE dir)
// {
//     // 动态计算前视距离，基于当前速度
//     float lookahead_distance = fmax(MIN_LOOKAHEAD_DISTANCE, fmin(MAX_LOOKAHEAD_DISTANCE, LOOKAHEAD_DISTANCE_FACTOR * this->Now_Position.speed));

//     int lookahead_point_index = -1;
//     float min_distance_sq = lookahead_distance * lookahead_distance;

//     // 遍历轨迹点，寻找第一个超出前视距离的点
//     for (int i = 0; i < POINTS_NUM; i++)
//     {
//         float dx = this->Trajectory[i][0] - this->Now_Position.x;
//         float dy = this->Trajectory[i][1] - this->Now_Position.y;

//         // 将轨迹点转换到车辆坐标系
//         float local_x = dx * cos(-this->Now_Position.yaw) - dy * sin(-this->Now_Position.yaw);
//         float local_y = dx * sin(-this->Now_Position.yaw) + dy * cos(-this->Now_Position.yaw);

//         // 只考虑车辆前方的点
//         if (local_x > 0)
//         {
//             float distance_sq = local_x * local_x + local_y * local_y;
//             if (distance_sq >= min_distance_sq)
//             {
//                 lookahead_point_index = i;
//                 break;
//             }
//         }
//     }

//     // 如果未找到合适的前视点，则停止车辆
//     if (lookahead_point_index == -1)
//     {
//         this->Set_Mode(this, STOP);
//         this->Target_Speed.linear_velocity = 0.0f;
//         this->Target_Speed.angular_velocity = 0.0f;
//         return;
//     }

//     // 获取前视点在车辆坐标系中的位置
//     float dx = this->Trajectory[lookahead_point_index][0] - this->Now_Position.x;
//     float dy = this->Trajectory[lookahead_point_index][1] - this->Now_Position.y;

//     float local_x = dx * cos(-this->Now_Position.yaw) - dy * sin(-this->Now_Position.yaw);
//     float local_y = dx * sin(-this->Now_Position.yaw) + dy * cos(-this->Now_Position.yaw);

//     // 计算曲率
//     float curvature = (2.0f * local_y) / (lookahead_distance * lookahead_distance);

//     // 限制曲率以防止过度转向
//     if (curvature > MAX_CURVATURE)
//         curvature = MAX_CURVATURE;
//     else if (curvature < -MAX_CURVATURE)
//         curvature = -MAX_CURVATURE;

//     // 计算转向角，考虑车辆轴距
//     float steering_angle = atan(WHEEL_BASE * curvature);

//     // 根据曲率调整目标速度
//     float target_speed = MAX_SPEED / (1.0f + fabs(curvature));

//     // 设置目标速度
//     this->Target_Speed.linear_velocity = fmin(target_speed, MAX_SPEED);
//     this->Target_Speed.angular_velocity = this->Now_Position.speed * curvature;

//     // 可选：如果需要，可以在此处添加速度平滑或加速度限制
// }

// void Pure_Pursuit(pClass_PurePursuit this, CONTROL_MODE dir)
// {

//     float lookahead_x = this->Now_Position.x + LOOKAHEAD_DISTANCE * cos(this->Now_Position.yaw);
//     float lookahead_y = this->Now_Position.y + LOOKAHEAD_DISTANCE * sin(this->Now_Position.yaw);

//     float min_dist_sq = 10;
//     int closest_point_index = -1;

//     for (int i = 0; i < POINTS_NUM; i++)
//     {
//         float dist_sq = (this->Trajectory[i][0] - lookahead_x) * (this->Trajectory[i][0] - lookahead_x) + (this->Trajectory[i][1] - lookahead_y) * (this->Trajectory[i][1] - lookahead_y);
//         if (dist_sq < min_dist_sq)
//         {
//             min_dist_sq = dist_sq;
//             closest_point_index = i;
//         }
//     }

//     if (closest_point_index == -1)
//     {
//         this->Set_Mode(this, STOP);
//         return;
//     }

//     float alpha = atan2(this->Trajectory[closest_point_index][1] - this->Now_Position.y, this->Trajectory[closest_point_index][0] - this->Now_Position.x) - this->Now_Position.yaw;
//     float dist = sqrtf(min_dist_sq);

//     if (dist < 1e-6)
//     {
//         this->Set_Mode(this, STOP);
//         return;
//     }

//     float curvature = 2 * sin(alpha) / dist;

//     float target_speed = 0.1f;
//     this->Target_Speed.linear_velocity = target_speed;
//     this->Target_Speed.angular_velocity = target_speed * curvature;

// }