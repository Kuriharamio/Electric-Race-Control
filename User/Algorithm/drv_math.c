/**
 * @file drv_math.c
 * @brief 一些数学
 */


#include "Algorithm/drv_math.h"


/**
 * @brief 16位大小端转换
 *
 * @param Address 地址
 */
void Math_Endian_Reverse_16(void *Address)
{
    uint8_t *temp_address_8   = (uint8_t *)Address;
    uint16_t *temp_address_16 = (uint16_t *)Address;
    *temp_address_16          = temp_address_8[0] << 8 | temp_address_8[1];
}

/**
 * @brief 16位大小端转换
 *
 * @param Source 源数据地址
 * @param Destination 目标存储地址
 */
void Math_Endian_Reverse_16_dual(void *Source, void *Destination)
{
    uint8_t *temp_source, *temp_destination;
    temp_source      = (uint8_t *)Source;
    temp_destination = (uint8_t *)Destination;

    temp_destination[0] = temp_source[1];
    temp_destination[1] = temp_source[0];
}

/**
 * @brief 32位大小端转换
 *
 * @param Address 地址
 */
void Math_Endian_Reverse_32(void *Address)
{
    uint8_t *temp_address_8   = (uint8_t *)Address;
    uint32_t *temp_address_32 = (uint32_t *)Address;
    *temp_address_32          = temp_address_8[0] << 24 | temp_address_8[1] << 16 | temp_address_8[2] << 8 | temp_address_8[3];
}

/**
 * @brief 32位大小端转换
 *
 * @param Source 源数据地址
 * @param Destination 目标存储地址
 */
void Math_Endian_Reverse_32_dual(void *Source, void *Destination)
{
    uint8_t *temp_source, *temp_destination;
    temp_source      = (uint8_t *)Source;
    temp_destination = (uint8_t *)Destination;

    temp_destination[0] = temp_source[3];
    temp_destination[1] = temp_source[2];
    temp_destination[2] = temp_source[1];
    temp_destination[3] = temp_source[0];
}

/**
 * @brief 求和
 *
 * @param Address 起始地址
 * @param Length 被加的数据的数量, 注意不是字节数
 * @return uint8_t 结果
 */
uint8_t Math_Sum_8(uint8_t *Address, uint32_t Length)
{
    uint8_t sum = 0;
    for (uint32_t i = 0; i < Length; i++) {
        sum += Address[i];
    }
    return sum;
}

/**
 * @brief 求和
 *
 * @param Address 起始地址
 * @param Length 被加的数据的数量, 注意不是字节数
 * @return uint16_t 结果
 */
uint16_t Math_Sum_16(uint16_t *Address, uint32_t Length)
{
    uint16_t sum = 0;
    for (uint32_t i = 0; i < Length; i++) {
        sum += Address[i];
    }
    return sum;
}

/**
 * @brief 求和
 *
 * @param Address 起始地址
 * @param Length 被加的数据的数量, 注意不是字节数
 * @return uint32_t 结果
 */
uint32_t Math_Sum_32(uint32_t *Address, uint32_t Length)
{
    uint32_t sum = 0;
    for (uint32_t i = 0; i < Length; i++) {
        sum += Address[i];
    }
    return sum;
}

/**
 * @brief sinc函数的实现
 *
 * @param x 输入
 * @return float 输出
 */
float Math_Sinc(float x)
{
    // 分母为0则按极限求法
    if (Math_Abs_float(x) <= 2.0f * FLT_EPSILON) {
        return 1.0f;
    }

    return sin(x) / x;
}

/**
 * @brief 限幅函数 (float)
 *
 * @param x 传入数据
 * @param Min 最小值
 * @param Max 最大值
 */
void Math_Constrain_float(float *x, float Min, float Max)
{
    if (*x < Min) {
        *x = Min;
    } else if (*x > Max) {
        *x = Max;
    }
}

/**
 * @brief 限幅函数 (int)
 *
 * @param x 传入数据
 * @param Min 最小值
 * @param Max 最大值
 */
void Math_Constrain_int(int *x, int Min, int Max)
{
    if (*x < Min) {
        *x = Min;
    } else if (*x > Max) {
        *x = Max;
    }
}

/**
 * @brief 求绝对值 (float)
 *
 * @param x 传入数据
 * @return float x的绝对值
 */
float Math_Abs_float(float x)
{
    return ((x > 0) ? x : -x);
}

/**
 * @brief 求绝对值 (int)
 *
 * @param x 传入数据
 * @return int x的绝对值
 */
int Math_Abs_int(int x)
{
    return ((x > 0) ? x : -x);
}

/**
 * @brief 将角度转换到[-PI, PI]区间
 *
 * @param angle 输入角度
 * @return double 转换后的角度
 */
float TransAngleInPI(float angle)
{
    float out_angle = angle;
    if (angle > PI)
    {
        out_angle -= 2 * PI;
    }
    else if (angle < -PI)
    {
        out_angle += 2 * PI;
    }
    return out_angle;
}