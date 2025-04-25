/**
 * @file timer.c
 * @author KuriharaMio
 * @brief 定时器中断处理函数实现文件
 * @note 该文件包含了电机PID定时器、编码器测速定时器和ADC按钮读取数据定时器的中断处理函数。
 * @version 0.1
 * @date 2025-04-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "BSP/timer.h"
// 计数器定时器中断处理函数

#ifdef USE_PID_TIMER
// 电机PID定时器中断处理函数
void PID_INST_IRQHandler(void)
{
    switch (DL_TimerA_getPendingInterrupt(PID_INST))
    {
    case DL_TIMER_IIDX_ZERO:

        // // pClass_Motor Motor_LB = Get_Motor_INST(LEFT_BACK);
        // if (Get_Motor_INST(LEFT_BACK)->is_inited)
        //     Get_Motor_INST(LEFT_BACK)->TIM_PID_PeriodElapsedCallback(Get_Motor_INST(LEFT_BACK));
        // // pClass_Motor Motor_RB = Get_Motor_INST(RIGHT_BACK);
        // if (Get_Motor_INST(RIGHT_BACK)->is_inited)
        //     Get_Motor_INST(RIGHT_BACK)->TIM_PID_PeriodElapsedCallback(Get_Motor_INST(RIGHT_BACK));
        // // pClass_Motor Motor_LF = Get_Motor_INST(LEFT_FRONT);
        // if (Get_Motor_INST(LEFT_FRONT)->is_inited)
        //     Get_Motor_INST(LEFT_FRONT)->TIM_PID_PeriodElapsedCallback(Get_Motor_INST(LEFT_FRONT));
        // // pClass_Motor Motor_RF = Get_Motor_INST(RIGHT_FRONT);
        // if (Get_Motor_INST(RIGHT_FRONT)->is_inited)
        //     Get_Motor_INST(RIGHT_FRONT)->TIM_PID_PeriodElapsedCallback(Get_Motor_INST(RIGHT_FRONT));
        // if (Get_Car_Handle()->is_inited)
        // {
        //     Get_Car_Handle()->Update_Speed_PID(Get_Car_Handle());
        //     if (Get_Car_Handle()->follow_error)
        //         Get_Car_Handle()->Update_Follow_PID(Get_Car_Handle());
        // }

        // pClass_Servo servo_up = Get_Servo_INST(SERVO_UP_INDEX);
        if (Get_Servo_INST(SERVO_UP_INDEX)->is_inited)
        {
            Get_Servo_INST(SERVO_UP_INDEX)->Update_PID(Get_Servo_INST(SERVO_UP_INDEX));
        }
        // pClass_Servo servo_down = Get_Servo_INST(SERVO_DOWN_INDEX);
        if (Get_Servo_INST(SERVO_DOWN_INDEX)->is_inited)
        {
            Get_Servo_INST(SERVO_DOWN_INDEX)->Update_PID(Get_Servo_INST(SERVO_DOWN_INDEX));
        }
        break;

    default:
        break;
    }
}
#endif

#ifdef USE_ENCODER_TIMER
// 编码器测速定时器中断处理函数
void ENCODER_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(ENCODER_INST))
    {
    case DL_TIMER_IIDX_ZERO:
        // 计算速度
        // pClass_Motor Motor_LB = Get_Motor_INST(LEFT_BACK);
        if (Get_Motor_INST(LEFT_BACK)->is_inited)
        {
            Get_Motor_INST(LEFT_BACK)->Now_Speed = (float)(Get_Motor_INST(LEFT_BACK)->Total_Encoder_Tick) / (float)(Get_Motor_INST(LEFT_BACK)->Encoder_Num_Per_Round) / ENCODER_TIMER_T * 2 * PI * Get_Motor_INST(LEFT_BACK)->Radius;
            Get_Motor_INST(LEFT_BACK)->Total_Encoder_Tick = 0;
        }
        // pClass_Motor Motor_LF = Get_Motor_INST(LEFT_FRONT);
        if (Get_Motor_INST(LEFT_FRONT)->is_inited)
        {
            Get_Motor_INST(LEFT_FRONT)->Now_Speed = (float)(Get_Motor_INST(LEFT_FRONT)->Total_Encoder_Tick) / (float)(Get_Motor_INST(LEFT_FRONT)->Encoder_Num_Per_Round) / ENCODER_TIMER_T * 2 * PI * Get_Motor_INST(LEFT_FRONT)->Radius;
            Get_Motor_INST(LEFT_FRONT)->Total_Encoder_Tick = 0;
        }
        // pClass_Motor Motor_RB = Get_Motor_INST(RIGHT_BACK);
        if (Get_Motor_INST(RIGHT_BACK)->is_inited)
        {
            Get_Motor_INST(RIGHT_BACK)->Now_Speed = (float)(Get_Motor_INST(RIGHT_BACK)->Total_Encoder_Tick) / (float)(Get_Motor_INST(RIGHT_BACK)->Encoder_Num_Per_Round) / ENCODER_TIMER_T * 2 * PI * Get_Motor_INST(RIGHT_BACK)->Radius;
            Get_Motor_INST(RIGHT_BACK)->Total_Encoder_Tick = 0;
        }
        // pClass_Motor Motor_RF = Get_Motor_INST(RIGHT_FRONT);
        if (Get_Motor_INST(RIGHT_FRONT)->is_inited)
        {
            Get_Motor_INST(RIGHT_FRONT)->Now_Speed = (float)(Get_Motor_INST(RIGHT_FRONT)->Total_Encoder_Tick) / (float)(Get_Motor_INST(RIGHT_FRONT)->Encoder_Num_Per_Round) / ENCODER_TIMER_T * 2 * PI * Get_Motor_INST(RIGHT_FRONT)->Radius;
            Get_Motor_INST(RIGHT_FRONT)->Total_Encoder_Tick = 0;
        }

        // 更新里程计
        if (Get_Car_Handle()->is_inited)
        {
            Get_Car_Handle()->Kinematic_Forward(Get_Car_Handle());
            Get_Car_Handle()->Update_Odom(Get_Car_Handle());
            Get_Car_Handle()->PurePursuit->Update_Now_Speed(Get_Car_Handle()->PurePursuit, Get_Car_Handle()->Now_Speed);
        }
        break;

    default:
        break;
    }
}
#endif

#ifdef USE_ADC_BUTTON
// ADC按钮读取数据定时器中断处理函数
void ADC_BUTTON_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(ADC_BUTTON_INST))
    {
    case DL_TIMER_IIDX_ZERO:
        // pClass_ADCButton ADC_Button = GET_ADCButton_INST();         // 获取ADC按钮实例
        if(GET_ADCButton_INST()->is_inited){
            GET_ADCButton_INST()->Check_And_Trigger(GET_ADCButton_INST()); // 获取当前ADC值
        }
            
        break;

    default:
        break;
    }
}
#endif