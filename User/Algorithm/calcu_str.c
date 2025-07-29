/**
 * @file calcu_str.c
 * @brief 递推最小二乘自适应控制器实现
 */
#include "Algorithm/calcu_str.h"


/* ---------- 内部小工具 ---------- */
static inline float vec_dot(const float *a, const float *b, int n)
{
    float s = 0.0f;
    for (int i = 0; i < n; ++i)
        s += a[i] * b[i];
    return s;
}

/* ---------- 对象工厂 ---------- */
pClass_STR create_STR(void)
{
    pClass_STR obj = NULL;
#ifdef USE_FREERTOS
    obj = (pClass_STR)pvPortMalloc(sizeof(Class_STR));
#else
    obj = (pClass_STR)malloc(sizeof(Class_STR));
#endif
    obj->Init = STR_Init;
    obj->Get_Out = STR_Get_Out;
    obj->Set_Target = STR_Set_Target;
    obj->Set_Now = STR_Set_Now;
    obj->Update_Value = STR_Update_Value;
    return obj;
}

/* ---------- 初始化 ---------- */
void STR_Init(pClass_STR str,
              Enum_STR_Mode __Mode,
              float __Lambda,
              const float __A_m[2],
              float __B_m,
              float __R_weight,
              float __Out_Max,
              float __Dead_Zone,
              float __D_T)
{
    str->Mode = __Mode;
    str->Lambda = __Lambda;
    str->A_m[0] = __A_m[0];
    str->A_m[1] = __A_m[1];
    str->B_m = __B_m;
    str->R_weight = __R_weight;
    str->Out_Max = __Out_Max;
    str->Dead_Zone = __Dead_Zone;
    str->D_T = __D_T; /* 保留兼容 */

    /* 历史清零 */
    str->y[0] = str->y[1] = 0.0f;
    str->u[0] = str->u[1] = 0.0f;

    /* 参数初值(零附近) */
    str->theta[0] = 0.0f; /* a1 */
    str->theta[1] = 0.0f; /* a2 */
    str->theta[2] = 0.1f; /* b0 */
    str->theta[3] = 0.0f; /* b1 */

    /* P = 1000*I */
    for (int i = 0; i < 4 * 4; ++i)
        str->P[i] = 0.0f;
    for (int i = 0; i < 4; ++i)
        str->P[i * 4 + i] = 1000.0f;

    str->Out = 0.0f;
    str->Target = 0.0f;
    str->Now = 0.0f;
}

/* ---------- 公共服务 ---------- */
float STR_Get_Out(pClass_STR str) { return str->Out; }
void STR_Set_Target(pClass_STR str, float t) { str->Target = t; }
void STR_Set_Now(pClass_STR str, float n) { str->Now = n; }

/* ---------- 主调度 ---------- */
void STR_Update_Value(pClass_STR str)
{
    float yk = str->Now;
    float rk = str->Target;

    /* 死区 */
    if (fabsf(rk - yk) < str->Dead_Zone)
        yk = rk;

    /* 回归向量 φ(k) = [-y(k-1) -y(k-2) u(k-1) u(k-2)]^T */
    float phi[4] = {-str->y[0], -str->y[1], str->u[0], str->u[1]};

    /* ---------- 1. 递推最小二乘 ---------- */
    float y_hat = vec_dot(phi, str->theta, 4); /* 预测输出 */
    float e = yk - y_hat;                      /* 预测误差 */

    /* P*φ */
    float P_phi[4];
    for (int i = 0; i < 4; ++i)
    {
        P_phi[i] = 0.0f;
        for (int j = 0; j < 4; ++j)
            P_phi[i] += str->P[i * 4 + j] * phi[j];
    }

    /* 分母 λ + φ^T*P*φ */
    float denom = str->Lambda + vec_dot(phi, P_phi, 4);

    /* K = P*φ / denom */
    float K[4];
    for (int i = 0; i < 4; ++i)
        K[i] = P_phi[i] / denom;

    /* θ = θ + K*e */
    for (int i = 0; i < 4; ++i)
        str->theta[i] += K[i] * e;

    /* P = (P - K*φ^T*P)/λ */
    float K_phi_P[4 * 4];
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            K_phi_P[i * 4 + j] = K[i] * phi[j];

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
        {
            int idx = i * 4 + j;
            str->P[idx] = (str->P[idx] - K_phi_P[i * 4 + j]) / str->Lambda;
        }
    if (str->theta[2] < 0.01f)
        str->theta[2] = 0.01f;
    /* ---------- 2. 计算控制量 ---------- */
    float uk = 0.0f;

    if (str->Mode == STR_MODE_POLE_PLACE)
    {
        /* 极点配置: A_m(z)y = B_m z r
           控制律: R u = T r - S y
           其中 R = B_m,  S = A_m - A,  T = B_m
           这里简化为一步求解
        */
        float a1 = str->theta[0], a2 = str->theta[1];
        float b0 = str->theta[2], b1 = str->theta[3];

        /* 期望闭环特征多项式 A_m = z^2 + A_m[0]z + A_m[1] */
        float s0 = str->A_m[0] - a1;
        float s1 = str->A_m[1] - a2;

        /* 控制律: u = (B_m r - s0 y(k) - s1 y(k-1) - r1 u(k-1)) / r0
           取 R = b0 + b1 z^{-1} 之 r0=b0, r1=b1
        */
        uk = (str->B_m * rk - s0 * yk - s1 * str->y[0] - b1 * str->u[0]) / b0;
    }
    else /* STR_MODE_MIN_VAR */
    {
        /* 最小方差 u(k) = (r - y_hat)/b0 * (1 + R_weight) */
        uk = (rk - y_hat) * (1.0f + str->R_weight) / str->theta[2];
    }

    /* 限幅 */
    if (str->Out_Max > 0.0f)
    {
        if (uk > str->Out_Max)
            uk = str->Out_Max;
        if (uk < -str->Out_Max)
            uk = -str->Out_Max;
    }

    /* ---------- 3. 更新历史 ---------- */
    str->y[1] = str->y[0];
    str->y[0] = yk;
    str->u[1] = str->u[0];
    str->u[0] = uk;

    str->Out = uk;
}