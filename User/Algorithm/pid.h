/**
 * @file calcu_pid.h
 * @brief PID算法
 */

#ifndef __PID_H__
#define __PID_H__

#include "Algorithm/drv_math.h"
#include "stdlib.h"

/**
 * @brief 微分先行
 */
typedef enum Enum_PID_D_First
{
    PID_D_First_DISABLE = 0,
    PID_D_First_ENABLE,
} Enum_PID_D_First;

/**
 * @brief Reusable, PID算法
 */
typedef struct Class_PID
{
    /*** 初始化相关常量 ***/

    float D_T;                // PID计时器周期, s
    float Dead_Zone;          // 死区, Error在其绝对值内不输出
    Enum_PID_D_First D_First; // 微分先行

    /*** 常量 ***/
    float Pre_Now;    // 之前的当前值
    float Pre_Target; // 之前的目标值
    float Pre_Out;    // 之前的输出值
    float Pre_Error;  // 前向误差

    float Out; // 输出值

    float K_P;                  // PID的P
    float K_I;                  // PID的I
    float K_D;                  // PID的D
    float K_F;                  // 前馈
    float I_Out_Max;            // 积分限幅, 0为不限制
    float Out_Max;              // 输出限幅, 0为不限制
    float I_Variable_Speed_A;   // 变速积分定速内段阈值, 0为不限制
    float I_Variable_Speed_B;   // 变速积分变速区间, 0为不限制
    float I_Separate_Threshold; // 积分分离阈值，需为正数, 0为不限制

    float Target; // 目标值
    float Now;    // 当前值

    float Integral_Error; // 积分值

    /**
     * @brief PID初始化
     *
     * @param __K_P P值
     * @param __K_I I值
     * @param __K_D D值
     * @param __K_F 前馈
     * @param __I_Out_Max 积分限幅
     * @param __Out_Max 输出限幅
     * @param __D_T 时间片长度
     * @param __Dead_Zone 死区
     * @param __I_Variable_Speed_A 定速内段阈值
     * @param __I_Variable_Speed_B 变速区间
     * @param __I_Separate_Threshold 积分分离阈值
     * @param __D_First 微分先行
     */
    void (*PID_Init)(struct Class_PID *pid, float __K_P, float __K_I, float __K_D, float __K_F, float __I_Out_Max, float __Out_Max, float __D_T, float __Dead_Zone, float __I_Variable_Speed_A, float __I_Variable_Speed_B, float __I_Separate_Threshold, Enum_PID_D_First __D_First);

    /**
     * @brief 获取积分误差
     *
     * @return float 输出值
     */
    float (*Get_Integral_Error)(struct Class_PID *pid);

    /**
     * @brief 获取输出值
     *
     * @return float 输出值
     */
    float (*Get_PID_Out)(struct Class_PID *pid);

    /**
     * @brief 设定PID的P
     *
     * @param __K_P PID的P
     */
    void (*Set_K_P)(struct Class_PID *pid, float __K_P);

    /**
     * @brief 设定PID的I
     *
     * @param __K_I PID的I
     */
    void (*Set_K_I)(struct Class_PID *pid, float __K_I);

    /**
     * @brief 设定PID的D
     *
     * @param __K_D PID的D
     */
    void (*Set_K_D)(struct Class_PID *pid, float __K_D);

    /**
     * @brief 设定前馈
     *
     * @param __K_D 前馈
     */
    void (*Set_K_F)(struct Class_PID *pid, float __K_F);

    /**
     * @brief 设定积分限幅, 0为不限制
     *
     * @param __I_Out_Max 积分限幅, 0为不限制
     */
    void (*Set_I_Out_Max)(struct Class_PID *pid, float __I_Out_Max);

    /**
     * @brief 设定输出限幅, 0为不限制
     *
     * @param __Out_Max 输出限幅, 0为不限制
     */
    void (*Set_Out_Max)(struct Class_PID *pid, float __Out_Max);

    /**
     * @brief 设定定速内段阈值, 0为不限制
     *
     * @param __I_Variable_Speed_A 定速内段阈值, 0为不限制
     */
    void (*Set_I_Variable_Speed_A)(struct Class_PID *pid, float __I_Variable_Speed_A);

    /**
     * @brief 设定变速区间, 0为不限制
     *
     * @param __I_Variable_Speed_B 变速区间, 0为不限制
     */
    void (*Set_I_Variable_Speed_B)(struct Class_PID *pid, float __I_Variable_Speed_B);

    /**
     * @brief 设定积分分离阈值，需为正数, 0为不限制
     *
     * @param __I_Separate_Threshold 积分分离阈值，需为正数, 0为不限制
     */
    void (*Set_I_Separate_Threshold)(struct Class_PID *pid, float __I_Separate_Threshold);

    /**
     * @brief 设定目标值
     *
     * @param __Target 目标值
     */
    void (*Set_Target)(struct Class_PID *pid, float __Target);

    /**
     * @brief 设定当前值
     *
     * @param __Now 当前值
     */
    void (*Set_Now)(struct Class_PID *pid, float __Now);

    /**
     * @brief 设定积分, 一般用于积分清零
     *
     * @param __Set_Integral_Error 积分值
     */
    void (*Set_Integral_Error)(struct Class_PID *pid, float __Integral_Error);

    /**
     * @brief PID调整值
     *
     * @return float 输出值
     */
    void (*TIM_Adjust_PeriodElapsedCallback)(struct Class_PID *pid);

} Class_PID, *pClass_PID;
pClass_PID create_PID(void);
void PID_Init(pClass_PID pid, float __K_P, float __K_I, float __K_D, float __K_F, float __I_Out_Max, float __Out_Max, float __D_T, float __Dead_Zone, float __I_Variable_Speed_A, float __I_Variable_Speed_B, float __I_Separate_Threshold, Enum_PID_D_First __D_First);

float Get_Integral_Error(pClass_PID pid);
float Get_PID_Out(pClass_PID pid);

void Set_K_P(pClass_PID pid, float __K_P);
void Set_K_I(pClass_PID pid, float __K_I);
void Set_K_D(pClass_PID pid, float __K_D);
void Set_K_F(pClass_PID pid, float __K_F);
void Set_I_Out_Max(pClass_PID pid, float __I_Out_Max);
void Set_Out_Max(pClass_PID pid, float __Out_Max);
void Set_I_Variable_Speed_A(pClass_PID pid, float __I_Variable_Speed_A);
void Set_I_Variable_Speed_B(pClass_PID pid, float __I_Variable_Speed_B);
void Set_I_Separate_Threshold(pClass_PID pid, float __I_Separate_Threshold);
void Set_Target(pClass_PID pid, float __Target);
void Set_Now(pClass_PID pid, float __Now);
void Set_Integral_Error(pClass_PID pid, float __Integral_Error);

void TIM_Adjust_PeriodElapsedCallback(pClass_PID pid);

#endif
