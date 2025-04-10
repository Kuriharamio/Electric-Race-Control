#ifndef _TIMER_H_
#define _TIMER_H_

#include "ti_msp_dl_config.h"
#include "Base_Modules/adc_button.h"
#include "Car/car.h"    

void PID_MOTOR_INST_IRQHandler(void); // 电机PID定时器中断处理函数
void ENCODER_INST_IRQHandler(void); // 编码器测速定时器中断处理函数
void ADC_BUTTON_INST_IRQHandler(void); // ADC按键定时器中断处理函数

#endif // _TIMER_H_