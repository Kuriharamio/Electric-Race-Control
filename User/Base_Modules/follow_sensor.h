#ifndef __FOLLOW_SENSOR_H__
#define __FOLLOW_SENSOR_H__

#include "config.h"
#include "BSP/delay.h"

#define Direction 0

#define Switch_Address_0(i) ((i) ? (DL_GPIO_setPins(GRAY_SCALE_PORT, GRAY_SCALE_AD0_PIN)) : (DL_GPIO_clearPins(GRAY_SCALE_PORT, GRAY_SCALE_AD0_PIN))) // 地址位0控制
#define Switch_Address_1(i) ((i) ? (DL_GPIO_setPins(GRAY_SCALE_PORT, GRAY_SCALE_AD1_PIN)) : (DL_GPIO_clearPins(GRAY_SCALE_PORT, GRAY_SCALE_AD1_PIN))) // 地址位1控制
#define Switch_Address_2(i) ((i) ? (DL_GPIO_setPins(GRAY_SCALE_PORT, GRAY_SCALE_AD2_PIN)) : (DL_GPIO_clearPins(GRAY_SCALE_PORT, GRAY_SCALE_AD2_PIN))) // 地址位2控制

extern unsigned short white[8];
extern unsigned short black[8];

typedef enum
{
    NORMAL_MODE,

} FOLLOW_MODE;

typedef struct Class_GraySensor
{
    unsigned short Analog_value[8];  // 原始模拟量值
    float Normal_value[8];  // 归一化后的值
    unsigned short Digital_value[8]; // 数字量值

    unsigned short Calibrated_white[8]; // 白校准基准值
    unsigned short Calibrated_black[8]; // 黑校准基准值
    unsigned short Gray_white[8];       // 白平衡灰度值
    unsigned short Gray_black[8];       // 黑平衡灰度值
    double Normal_factor[8];            // 归一化系数
    double bits;                        // ADC分辨率对应位数
    unsigned char Digtal;               // 数字输出状态
    unsigned char Time_out;             // 超时标志
    unsigned char Tick;                 // 时基计数器

    FOLLOW_MODE Mode;   // 传感器工作模式

    float k1;
    float k2;
    float k3;
    float k4;
    float Follow_Error; // 跟随误差
    bool undetected;    // 未检测到黑线标志
    int Search_Direction; // 搜索方向
    float Linear_Speed_Max; // 最大线速度

    unsigned int ADC_Value; // ADC采样值
    bool ADC_Flag; // ADC采样标志
    bool is_inited; // 传感器就绪标志

    void (*Init_With_Params)(struct Class_GraySensor *this, unsigned short *Calibrated_white, unsigned short *Calibrated_black); // 带校准参数的初始化
    void (*Init_Without_Params)(struct Class_GraySensor *this);                                                                  // 首次初始化
    void (*Update)(struct Class_GraySensor *this);                                                                               // 更新传感器全部数据

} Class_GraySensor, *pClass_GraySensor;

pClass_GraySensor Create_GraySensor(void);
pClass_GraySensor Get_Sensor_Handle(void);

void GraySensor_Init_Without_Params(pClass_GraySensor sensor);                                                                  // 首次初始化
void GraySensor_Init_With_Params(pClass_GraySensor sensor, unsigned short *Calibrated_white, unsigned short *Calibrated_black); // 带校准参数的初始化
void GraySensor_Update(pClass_GraySensor sensor);

#endif // __FOLLOW_SENSOR_H__