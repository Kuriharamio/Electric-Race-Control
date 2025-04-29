#ifndef _TIMER_H_
#define _TIMER_H_

#include "config.h"
#include "Base_Modules/servo.h"

#ifdef USE_PID
void PID_TIMER_INST_IRQHandler(void); // 电机PID定时器中断处理函数
#endif

#ifdef USE_ENCODER
void ENCODER_INST_IRQHandler(void); // 编码器测速定时器中断处理函数
#endif

#ifdef USE_ADC_BUTTON
#include "Base_Modules/adc_button.h"
void ADC_BUTTON_TIMER_INST_IRQHandler(void); // ADC按键定时器中断处理函数
#endif

#endif // _TIMER_H_