/**
 * @file calcu_str.h
 * @brief 递推最小二乘自适应控制器 (STR)
 *        模型: y(k)+a1*y(k-1)+a2*y(k-2) = b0*u(k-1)+b1*u(k-2)+e(k)
 *        控制律: 极点配置 / 最小方差可选
 */

#ifndef __STR_H__
#define __STR_H__

#include "config.h"

/**
 * @brief 控制律选择
 */
typedef enum Enum_STR_Mode
{
    STR_MODE_POLE_PLACE = 0, /* 极点配置 */
    STR_MODE_MIN_VAR,        /* 最小方差 */
} Enum_STR_Mode;

/**
 * @brief 可复用的 STR 对象
 */
typedef struct Class_STR
{
    /*** 初始化相关常量 ***/
    float D_T;          /* 周期, s(仅做兼容, 算法中未使用) */
    float Dead_Zone;    /* 死区(同 PID) */
    Enum_STR_Mode Mode; /* 控制律类型 */

    /*** 运行期变量 ***/
    float Out;    /* 当前输出 */
    float Target; /* 设定值 */
    float Now;    /* 过程值 */

    /* 2 拍历史 */
    float y[2]; /* y(k-1), y(k-2) */
    float u[2]; /* u(k-1), u(k-2) */

    /* 参数估计 θ = [a1 a2 b0 b1]^T */
    float theta[4];
    /* 协方差 P 4×4 行主序 */
    float P[4 * 4];
    /* 遗忘因子 λ */
    float Lambda;

    /* 极点配置用期望多项式系数 */
    float A_m[3]; /* z^2 + A_m[0]z + A_m[1] */
    float B_m;    /* B_m */

    /* 最小方差用权重 */
    float R_weight; /* 控制加权 */

    /* 输出限幅 */
    float Out_Max;

    /* 接口函数指针 */
    void (*Init)(struct Class_STR *str,
                     Enum_STR_Mode __Mode,
                     float __Lambda,
                     const float __A_m[2],
                     float __B_m,
                     float __R_weight,
                     float __Out_Max,
                     float __Dead_Zone,
                     float __D_T);

    float (*Get_Out)(struct Class_STR *str);
    void (*Set_Target)(struct Class_STR *str, float __Target);
    void (*Set_Now)(struct Class_STR *str, float __Now);
    void (*Update_Value)(struct Class_STR *str);
} Class_STR, *pClass_STR;

/* 工厂函数 */
pClass_STR create_STR(void);

/* 对外裸函数声明 */
void STR_Init(pClass_STR str,
              Enum_STR_Mode __Mode,
              float __Lambda,
              const float __A_m[2],
              float __B_m,
              float __R_weight,
              float __Out_Max,
              float __Dead_Zone,
              float __D_T);

float STR_Get_Out(pClass_STR str);
void STR_Set_Target(pClass_STR str, float __Target);
void STR_Set_Now(pClass_STR str, float __Now);
void STR_Update_Value(pClass_STR str);

#endif /* __STR_H__ */