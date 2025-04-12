#include "Algorithm/pure_pursuit.h"
static Class_PurePursuit _PURE_PURSUIT = {};
// Smooth Left Turn Trajectory Points
static const float trajectory_left[POINTS_NUM][2] = {
    {0.000000, 0.000000},
    {0.031978, -0.000019},
    {0.063269, -0.000072},
    {0.093881, -0.000150},
    {0.123822, -0.000247},
    {0.153098, -0.000356},
    {0.181718, -0.000467},
    {0.209689, -0.000576},
    {0.237017, -0.000673},
    {0.263711, -0.000751},
    {0.289779, -0.000804},
    {0.315226, -0.000823},
    {0.340062, -0.000801},
    {0.364292, -0.000732},
    {0.387926, -0.000606},
    {0.410969, -0.000417},
    {0.433430, -0.000158},
    {0.455316, 0.000179},
    {0.476634, 0.000601},
    {0.497392, 0.001116},
    {0.517597, 0.001731},
    {0.537257, 0.002454},
    {0.556379, 0.003292},
    {0.574970, 0.004253},
    {0.593038, 0.005343},
    {0.610590, 0.006570},
    {0.627634, 0.007942},
    {0.644177, 0.009467},
    {0.660227, 0.011150},
    {0.675791, 0.013001},
    {0.690876, 0.015026},
    {0.705490, 0.017233},
    {0.719640, 0.019629},
    {0.733333, 0.022222},
    {0.746578, 0.025019},
    {0.759381, 0.028027},
    {0.771751, 0.031255},
    {0.783693, 0.034708},
    {0.795216, 0.038396},
    {0.806328, 0.042324},
    {0.817035, 0.046501},
    {0.827345, 0.050934},
    {0.837265, 0.055631},
    {0.846804, 0.060598},
    {0.855967, 0.065844},
    {0.864763, 0.071375},
    {0.873200, 0.077199},
    {0.881284, 0.083324},
    {0.889022, 0.089757},
    {0.896424, 0.096505},
    {0.903495, 0.103576},
    {0.910243, 0.110978},
    {0.916676, 0.118716},
    {0.922801, 0.126800},
    {0.928625, 0.135237},
    {0.934156, 0.144033},
    {0.939402, 0.153196},
    {0.944369, 0.162735},
    {0.949066, 0.172655},
    {0.953499, 0.182965},
    {0.957676, 0.193672},
    {0.961604, 0.204784},
    {0.965292, 0.216307},
    {0.968745, 0.228249},
    {0.971973, 0.240619},
    {0.974981, 0.253422},
    {0.977778, 0.266667},
    {0.980371, 0.280360},
    {0.982767, 0.294510},
    {0.984974, 0.309124},
    {0.986999, 0.324209},
    {0.988850, 0.339773},
    {0.990533, 0.355823},
    {0.992058, 0.372366},
    {0.993430, 0.389410},
    {0.994657, 0.406962},
    {0.995747, 0.425030},
    {0.996708, 0.443621},
    {0.997546, 0.462743},
    {0.998269, 0.482403},
    {0.998884, 0.502608},
    {0.999399, 0.523366},
    {0.999821, 0.544684},
    {1.000158, 0.566570},
    {1.000417, 0.589031},
    {1.000606, 0.612074},
    {1.000732, 0.635708},
    {1.000801, 0.659938},
    {1.000823, 0.684774},
    {1.000804, 0.710221},
    {1.000751, 0.736289},
    {1.000673, 0.762983},
    {1.000576, 0.790311},
    {1.000467, 0.818282},
    {1.000356, 0.846902},
    {1.000247, 0.876178},
    {1.000150, 0.906119},
    {1.000072, 0.936731},
    {1.000019, 0.968022},
    {1.000000, 1.000000},
};

// Smooth Right Turn Trajectory Points
static const float trajectory_right[POINTS_NUM][2] = {
    {0.000000, 0.000000},
    {0.031978, 0.000019},
    {0.063269, 0.000072},
    {0.093881, 0.000150},
    {0.123822, 0.000247},
    {0.153098, 0.000356},
    {0.181718, 0.000467},
    {0.209689, 0.000576},
    {0.237017, 0.000673},
    {0.263711, 0.000751},
    {0.289779, 0.000804},
    {0.315226, 0.000823},
    {0.340062, 0.000801},
    {0.364292, 0.000732},
    {0.387926, 0.000606},
    {0.410969, 0.000417},
    {0.433430, 0.000158},
    {0.455316, -0.000179},
    {0.476634, -0.000601},
    {0.497392, -0.001116},
    {0.517597, -0.001731},
    {0.537257, -0.002454},
    {0.556379, -0.003292},
    {0.574970, -0.004253},
    {0.593038, -0.005343},
    {0.610590, -0.006570},
    {0.627634, -0.007942},
    {0.644177, -0.009467},
    {0.660227, -0.011150},
    {0.675791, -0.013001},
    {0.690876, -0.015026},
    {0.705490, -0.017233},
    {0.719640, -0.019629},
    {0.733333, -0.022222},
    {0.746578, -0.025019},
    {0.759381, -0.028027},
    {0.771751, -0.031255},
    {0.783693, -0.034708},
    {0.795216, -0.038396},
    {0.806328, -0.042324},
    {0.817035, -0.046501},
    {0.827345, -0.050934},
    {0.837265, -0.055631},
    {0.846804, -0.060598},
    {0.855967, -0.065844},
    {0.864763, -0.071375},
    {0.873200, -0.077199},
    {0.881284, -0.083324},
    {0.889022, -0.089757},
    {0.896424, -0.096505},
    {0.903495, -0.103576},
    {0.910243, -0.110978},
    {0.916676, -0.118716},
    {0.922801, -0.126800},
    {0.928625, -0.135237},
    {0.934156, -0.144033},
    {0.939402, -0.153196},
    {0.944369, -0.162735},
    {0.949066, -0.172655},
    {0.953499, -0.182965},
    {0.957676, -0.193672},
    {0.961604, -0.204784},
    {0.965292, -0.216307},
    {0.968745, -0.228249},
    {0.971973, -0.240619},
    {0.974981, -0.253422},
    {0.977778, -0.266667},
    {0.980371, -0.280360},
    {0.982767, -0.294510},
    {0.984974, -0.309124},
    {0.986999, -0.324209},
    {0.988850, -0.339773},
    {0.988850, -0.339773},
    {0.990533, -0.355823},
    {0.992058, -0.372366},
    {0.992058, -0.372366},
    {0.993430, -0.389410},
    {0.994657, -0.406962},
    {0.994657, -0.406962},
    {0.995747, -0.425030},
    {0.996708, -0.443621},
    {0.997546, -0.462743},
    {0.998269, -0.482403},
    {0.998884, -0.502608},
    {0.998884, -0.502608},
    {0.999399, -0.523366},
    {0.999399, -0.523366},
    {0.999821, -0.544684},
    {1.000158, -0.566570},
    {1.000417, -0.589031},
    {1.000606, -0.612074},
    {1.000732, -0.635708},
    {1.000801, -0.659938},
    {1.000823, -0.684774},
    {1.000804, -0.710221},
    {1.000751, -0.736289},
    {1.000673, -0.762983},
    {1.000576, -0.790311},
    {1.000467, -0.818282},
    {1.000356, -0.846902},
    {1.000247, -0.876178},
    {1.000150, -0.906119},
    {1.000072, -0.936731},
    {1.000019, -0.968022},
    {1.000000, -1.000000},
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
    if(this->finish_current_mode){
        switch (this->Control_Dir)
        {
        case TURN_LEFT:
            memcpy(this->Trajectory, trajectory_left, sizeof(float) * POINTS_NUM * 2);
            break;
        case TURN_RIGHT:
            memcpy(this->Trajectory, trajectory_right, sizeof(float) * POINTS_NUM * 2);
            break;
        default:
            break;
        }
        this->finish_current_mode = false;
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
    if(this->Control_Dir == STOP){
        this->finish_current_mode = true;

        this->Now_Position.x = 0.0f;
        this->Now_Position.y = 0.0f;
        this->Now_Position.yaw = 0.0f;
        this->Target_Speed.linear_velocity = 0.0f;
        this->Target_Speed.angular_velocity = 0.0f;

        return false;
    }
    // 动态计算前视距离，基于当前速度
    float lookahead_distance = fmax(MIN_LOOKAHEAD_DISTANCE, fmin(MAX_LOOKAHEAD_DISTANCE, LOOKAHEAD_DISTANCE_FACTOR * this->Now_Speed.linear_velocity));

    int lookahead_point_index = this->Find_Lookahead_Point(this, lookahead_distance);

    // 如果未找到合适的前视点，则停止车辆
    if (lookahead_point_index == -1)
    {
        this->finish_current_mode = true;

        this->Now_Position.x = 0.0f;
        this->Now_Position.y = 0.0f;
        this->Now_Position.yaw = 0.0f;
        this->Target_Speed.linear_velocity = 0.0f;
        this->Target_Speed.angular_velocity = 0.0f;
        return false;
    }

    // 获取前视点在车辆坐标系中的位置
    float dx = this->Trajectory[lookahead_point_index][0] - this->Now_Position.x;
    float dy = this->Trajectory[lookahead_point_index][1] - this->Now_Position.y;

    float local_x = dx * cos(-this->Now_Position.yaw) - dy * sin(-this->Now_Position.yaw);
    float local_y = dx * sin(-this->Now_Position.yaw) + dy * cos(-this->Now_Position.yaw);

    // 计算Pure Pursuit曲率
    float curvature_pure_pursuit = (2.0f * local_y) / (lookahead_distance * lookahead_distance);

    // 限制曲率以防止过度转向
    if (curvature_pure_pursuit > MAX_CURVATURE)
        curvature_pure_pursuit = MAX_CURVATURE;
    else if (curvature_pure_pursuit < -MAX_CURVATURE)
        curvature_pure_pursuit = -MAX_CURVATURE;

    // 计算Pure Pursuit转向角
    float steering_angle_pure_pursuit = atan(WHEEL_BASE * curvature_pure_pursuit);

    // // ********** Stanley方法部分 **********
    // // 计算最近点的索引和距离
    // float min_dist_sq = 0.0f;
    // int closest_index = this->Find_Closest_Point(this, &min_dist_sq);
    // if (closest_index == -1 || min_dist_sq == 0.0f)
    // {
    //     this->Set_Mode(this, STOP);
    //     this->Target_Speed.linear_velocity = 0.0f;
    //     this->Target_Speed.angular_velocity = 0.0f;
    //     return false;
    // }

    // // 计算横向误差（cross-track error）
    // float cross_track_error = sqrtf(min_dist_sq);
    // // 判断横向误差的符号
    // float path_heading = atan2(this->Trajectory[closest_index][1] - this->Trajectory[closest_index > 0 ? closest_index - 1 : 0][1],
    //                            this->Trajectory[closest_index][0] - this->Trajectory[closest_index > 0 ? closest_index - 1 : 0][0]);
    // float heading_error = normalize_angle(this->Now_Position.yaw - path_heading);

    // // 确定横向误差的符号
    // float sign = (this->Trajectory[closest_index][0] - this->Now_Position.x) * sin(this->Now_Position.yaw) -
    //              (this->Trajectory[closest_index][1] - this->Now_Position.yaw) * cos(this->Now_Position.yaw);
    // if (sign < 0)
    //     cross_track_error = -cross_track_error;

    // // 计算Stanley方法的角度调整
    // float theta = normalize_angle(-heading_error);
    // float steering_angle_stanley = K_STANLEY_HEADING * theta + atan2(K_STANLEY_CROSSTRACK * cross_track_error, this->Now_Speed.linear_velocity + 1e-5f); // 加小量避免除零

    // // ********** 综合Pure Pursuit和Stanley方法的角度 **********
    // // 通过加权平均综合两种方法的转向角
    // float alpha = 0.5f; // 权重因子，可根据需要调整
    // float steering_angle = alpha * steering_angle_pure_pursuit + (1.0f - alpha) * steering_angle_stanley;

    // 计算综合曲率
    float curvature = tan(steering_angle_pure_pursuit) / WHEEL_BASE;

    // 限制曲率以防止过度转向
    if (curvature > MAX_CURVATURE)
        curvature = MAX_CURVATURE;
    else if (curvature < -MAX_CURVATURE)
        curvature = -MAX_CURVATURE;

    // 根据曲率调整目标速度
    float target_speed = MAX_SPEED / (1.0f + fabs(curvature));

    // 设置目标速度
    this->Target_Speed.linear_velocity = fmin(target_speed, MAX_SPEED);
    this->Target_Speed.angular_velocity = this->Target_Speed.linear_velocity * curvature;

    return true;

    // 如果需要，可以在此处添加速度平滑或加速度限制
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