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

// 电机PID定时器中断处理函数
void PID_INST_IRQHandler(void)
{
    static int tick = 0;
    tick++;
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

        // if(tick == 2){
            // pClass_Car car = Get_Car_Handle();
        if (Get_Car_Handle()->is_inited)
        {
            Get_Car_Handle()->Kinematic_Forward(Get_Car_Handle());
            //     car->TIM_PID_Speed_PeriodElapsedCallback(car);
        }
        // }

        // if(tick == 3){
        //     pClass_Car car = Get_Car_Handle();
        //     if (car->is_inited)
        //     {
        //         car->TIM_PID_Position_PeriodElapsedCallback(car);
        //     }
        //     tick = 0;
        // }
        
        break;

    default:
        break;
    }
}

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
        pClass_Car car = Get_Car_Handle();
        if (car->is_inited)
            car->Update_Odom(car);
        break;

    default:
        break;
    }
}

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