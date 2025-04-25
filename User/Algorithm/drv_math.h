/**
 * @file drv_math.h
 * @brief 一些数学
 */

#ifndef DRV_MATH_H
#define DRV_MATH_H

#include <stdint.h>
#include <limits.h>
#include <math.h>
#include <float.h>

// 圆周率PI
#define PI (3.141592653589f)

void Math_Endian_Reverse_16(void *Address);
void Math_Endian_Reverse_16_dual(void *Source, void *Destination);
void Math_Endian_Reverse_32(void *Address);
void Math_Endian_Reverse_32_dual(void *Source, void *Destination);

uint8_t Math_Sum_8(uint8_t *Address, uint32_t Length);
uint16_t Math_Sum_16(uint16_t *Address, uint32_t Length);
uint32_t Math_Sum_32(uint32_t *Address, uint32_t Length);

float Math_Sinc(float x);

void Math_Constrain_float(float *x, float Min, float Max);
void Math_Constrain_int(int *x, int Min, int Max);

float Math_Abs_float(float x);
int Math_Abs_int(int x);

float TransAngleInPI(float angle);

#endif
