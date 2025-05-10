/**
 * @file pid.c
 * @brief PID算法
 */

#include "Algorithm/pid.h"

#ifdef USE_PID

/**
 * @brief 创建一个PID对象
 *
 * @return Class_PID*
 */
pClass_PID create_PID(void)
{
    pClass_PID tmp_pid_ptr = NULL;

#ifdef USE_FREERTOS
    tmp_pid_ptr = (pClass_PID)pvPortMalloc(sizeof(Class_PID));
#else
    tmp_pid_ptr = (pClass_PID)malloc(sizeof(Class_PID));
#endif

    tmp_pid_ptr->PID_Init = PID_Init;
    tmp_pid_ptr->Get_Integral_Error = Get_Integral_Error;
    tmp_pid_ptr->Get_PID_Out = Get_PID_Out;
    tmp_pid_ptr->Set_K_P = Set_K_P;
    tmp_pid_ptr->Set_K_I = Set_K_I;
    tmp_pid_ptr->Set_K_D = Set_K_D;
    tmp_pid_ptr->Set_K_F = Set_K_F;
    tmp_pid_ptr->Set_I_Out_Max = Set_I_Out_Max;
    tmp_pid_ptr->Set_Out_Max = Set_Out_Max;
    tmp_pid_ptr->Set_I_Variable_Speed_A = Set_I_Variable_Speed_A;
    tmp_pid_ptr->Set_I_Variable_Speed_B = Set_I_Variable_Speed_B;
    tmp_pid_ptr->Set_I_Separate_Threshold = Set_I_Separate_Threshold;
    tmp_pid_ptr->Set_Target = Set_Target;
    tmp_pid_ptr->Set_Now = Set_Now;
    tmp_pid_ptr->Set_Integral_Error = Set_Integral_Error;
    tmp_pid_ptr->TIM_Adjust_PeriodElapsedCallback = TIM_Adjust_PeriodElapsedCallback;
    return tmp_pid_ptr;
}

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
void PID_Init(pClass_PID pid, float __K_P, float __K_I, float __K_D, float __K_F, float __I_Out_Max, float __Out_Max, float __D_T, float __Dead_Zone, float __I_Variable_Speed_A, float __I_Variable_Speed_B, float __I_Separate_Threshold, Enum_PID_D_First __D_First)
{
    pid->K_P = __K_P;
    pid->K_I = __K_I;
    pid->K_D = __K_D;
    pid->K_F = __K_F;
    pid->I_Out_Max = __I_Out_Max;
    pid->Out_Max = __Out_Max;
    pid->D_T = __D_T;
    pid->Dead_Zone = __Dead_Zone;
    pid->I_Variable_Speed_A = __I_Variable_Speed_A;
    pid->I_Variable_Speed_B = __I_Variable_Speed_B;
    pid->I_Separate_Threshold = __I_Separate_Threshold;
    pid->D_First = __D_First;

    pid->Pre_Now = 0.0f;
    pid->Pre_Target = 0.0f;
    pid->Pre_Out = 0.0f;
    pid->Pre_Error = 0.0f;
    pid->Out = 0.0f;
    pid->Target = 0.0f;
    pid->Now = 0.0f;
    pid->Integral_Error = 0.0f;
}

/**
 * @brief 获取积分误差
 *
 * @return float 输出值
 */
float Get_Integral_Error(pClass_PID pid)
{
    return (pid->Integral_Error);
}

/**
 * @brief 获取输出值
 *
 * @return float 输出值
 */
float Get_PID_Out(pClass_PID pid)
{
    return (pid->Out);
}

/**
 * @brief 设定PID的P
 *
 * @param __K_P PID的P
 */
void Set_K_P(pClass_PID pid, float __K_P)
{
    pid->K_P = __K_P;
}

/**
 * @brief 设定PID的I
 *
 * @param __K_I PID的I
 */
void Set_K_I(pClass_PID pid, float __K_I)
{
    pid->K_I = __K_I;
}

/**
 * @brief 设定PID的D
 *
 * @param __K_D PID的D
 */
void Set_K_D(pClass_PID pid, float __K_D)
{
    pid->K_D = __K_D;
}

/**
 * @brief 设定前馈
 *
 * @param __K_D 前馈
 */
void Set_K_F(pClass_PID pid, float __K_F)
{
    pid->K_F = __K_F;
}

/**
 * @brief 设定积分限幅, 0为不限制
 *
 * @param __I_Out_Max 积分限幅, 0为不限制
 */
void Set_I_Out_Max(pClass_PID pid, float __I_Out_Max)
{
    pid->I_Out_Max = __I_Out_Max;
}

/**
 * @brief 设定输出限幅, 0为不限制
 *
 * @param __Out_Max 输出限幅, 0为不限制
 */
void Set_Out_Max(pClass_PID pid, float __Out_Max)
{
    pid->Out_Max = __Out_Max;
}

/**
 * @brief 设定定速内段阈值, 0为不限制
 *
 * @param __I_Variable_Speed_A 定速内段阈值, 0为不限制
 */
void Set_I_Variable_Speed_A(pClass_PID pid, float __I_Variable_Speed_A)
{
    pid->I_Variable_Speed_A = __I_Variable_Speed_A;
}

/**
 * @brief 设定变速区间, 0为不限制
 *
 * @param __I_Variable_Speed_B 变速区间, 0为不限制
 */
void Set_I_Variable_Speed_B(pClass_PID pid, float __I_Variable_Speed_B)
{
    pid->I_Variable_Speed_B = __I_Variable_Speed_B;
}

/**
 * @brief 设定积分分离阈值，需为正数, 0为不限制
 *
 * @param __I_Separate_Threshold 积分分离阈值，需为正数, 0为不限制
 */
void Set_I_Separate_Threshold(pClass_PID pid, float __I_Separate_Threshold)
{
    pid->I_Separate_Threshold = __I_Separate_Threshold;
}

/**
 * @brief 设定目标值
 *
 * @param __Target 目标值
 */
void Set_Target(pClass_PID pid, float __Target)
{
    pid->Target = __Target;
}

/**
 * @brief 设定当前值
 *
 * @param __Now 当前值
 */
void Set_Now(pClass_PID pid, float __Now)
{
    pid->Now = __Now;
}

/**
 * @brief 设定积分, 一般用于积分清零
 *
 * @param __Set_Integral_Error 积分值
 */
void Set_Integral_Error(pClass_PID pid, float __Integral_Error)
{
    pid->Integral_Error = __Integral_Error;
}

/**
 * @brief PID调整值
 *
 * @return float 输出值
 */
void TIM_Adjust_PeriodElapsedCallback(pClass_PID pid)
{
    // P输出
    float p_out = 0.0f;
    // I输出
    float i_out = 0.0f;
    // D输出
    float d_out = 0.0f;
    // F输出
    float f_out = 0.0f;
    // 误差
    float error = 0.0f;
    // 绝对值误差
    float abs_error = 0.0f;
    // 线性变速积分
    float speed_ratio = 0.0f;

    error = pid->Target - pid->Now;
    abs_error = Math_Abs_float(error);

    // 判断死区
    if (abs_error < pid->Dead_Zone)
    {
        pid->Target = pid->Now;
        error = 0.0f;
        abs_error = 0.0f;
    }

    // 计算p项

    p_out = pid->K_P * error;

    // 计算i项

    if (pid->I_Variable_Speed_A == 0.0f && pid->I_Variable_Speed_B == 0.0f)
    {
        // 非变速积分
        speed_ratio = 1.0f;
    }
    else
    {
        // 变速积分
        if (abs_error <= pid->I_Variable_Speed_B)
        {
            speed_ratio = 1.0f;
        }
        else if (pid->I_Variable_Speed_B < abs_error && abs_error < pid->I_Variable_Speed_A + pid->I_Variable_Speed_B)
        {
            speed_ratio = (pid->I_Variable_Speed_A + pid->I_Variable_Speed_B - abs_error) / pid->I_Variable_Speed_A;
        }
        if (abs_error >= pid->I_Variable_Speed_B)
        {
            speed_ratio = 0.0f;
        }
    }
    // 积分限幅
    if (pid->I_Out_Max != 0.0f)
    {
        Math_Constrain_float(&pid->Integral_Error, -pid->I_Out_Max / pid->K_I, pid->I_Out_Max / pid->K_I);
    }

    if (pid->I_Separate_Threshold == 0.0f)
    {
        // 没有积分分离
        pid->Integral_Error += speed_ratio * pid->D_T * error;
        i_out = pid->K_I * pid->Integral_Error;
    }
    else
    {
        // 积分分离使能
        if (abs_error < pid->I_Separate_Threshold)
        {
            pid->Integral_Error += speed_ratio * pid->D_T * error;
            i_out = pid->K_I * pid->Integral_Error;
        }
        else
        {
            pid->Integral_Error = 0.0f;
            i_out = 0.0f;
        }
    }

    // 计算d项
    if (pid->D_First == PID_D_First_DISABLE)
    {
        // 没有微分先行
        d_out = pid->K_D * (error - pid->Pre_Error) / pid->D_T;
    }
    else
    {
        // 微分先行使能
        d_out = pid->K_D * (pid->Out - pid->Pre_Out) / pid->D_T;
    }

    // 计算前馈
    f_out = (pid->Target - pid->Pre_Target) * pid->K_F;

    // 计算总共的输出
    pid->Out = p_out + i_out + d_out + f_out;

    // 输出限幅
    if (pid->Out_Max != 0.0f)
    {
        Math_Constrain_float(&pid->Out, -pid->Out_Max, pid->Out_Max);
    }

    // 善后工作
    pid->Pre_Now = pid->Now;
    pid->Pre_Target = pid->Target;
    pid->Pre_Out = pid->Out;
    pid->Pre_Error = error;
}

#endif