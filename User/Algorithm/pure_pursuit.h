#ifndef _PURE_PURSUIT_H_
#define _PURE_PURSUIT_H_

#include "config.h"
#include <math.h>
#include <string.h>

// 控制方向枚举，根据实际情况补充
typedef enum
{
    TURN_LEFT,
    TURN_RIGHT,
    STRAIGHT,
    FOLLOW,
    STOP
} CONTROL_MODE;

typedef struct
{
    float x;   // x坐标
    float y;   // y坐标
    float yaw; // 偏航角
} POSITION;

typedef struct
{
    float linear_velocity;  // 线速度
    float angular_velocity; // 角速度
} SPEED;

typedef struct Class_PurePursuit{
    float Trajectory[POINTS_NUM][2]; // 轨迹点

    CONTROL_MODE Control_Dir; // 控制方向

    SPEED Target_Speed; // 目标速度
    SPEED Now_Speed;      // 当前速度

    POSITION Target_Position;   // 目标位置
    POSITION Now_Position;      // 当前位置

    bool is_inited; // 是否初始化
    bool finish_current_mode; // 当前模式是否完成

    void (*Init)(struct Class_PurePursuit *this); // 初始化
    void (*Update_Now_Speed)(struct Class_PurePursuit *this, SPEED speed); // 更新位置
    bool (*Calculate_Target_Speed)(struct Class_PurePursuit *this);        // 计算目标速度
    SPEED (*Get_Output_Speed)(struct Class_PurePursuit *this);      // 获取输出速度
    void (*Set_Mode)(struct Class_PurePursuit *this, CONTROL_MODE dir); // 设置控制方向
    int (*Find_Closest_Point)(struct Class_PurePursuit *this, float *min_dist_sq); // 查找最接近的点
    int (*Find_Lookahead_Point)(struct Class_PurePursuit *this, float lookahead_distance);
}Class_PurePursuit, *pClass_PurePursuit;

pClass_PurePursuit Create_PurePursuit();
void PurePursuit_Init(pClass_PurePursuit this);
void PurePursuit_Update_Now_Speed(pClass_PurePursuit this, SPEED speed);
bool PurePursuit_Calculate_Target_Speed(pClass_PurePursuit this);
SPEED PurePursuit_Get_Output_Speed(pClass_PurePursuit this);
void PurePursuit_Set_Mode(pClass_PurePursuit this, CONTROL_MODE dir);
int PurePursuit_Find_Closest_Point(pClass_PurePursuit this, float *min_dist_sq);
int PurePursuit_Find_Lookahead_Point(pClass_PurePursuit this, float lookahead_distance);

#endif // _PURE_PURSUIT_H_