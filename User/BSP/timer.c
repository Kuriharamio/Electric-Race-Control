#include "BSP/timer.h"

// 电机PID定时器中断处理函数
void PID_INST_IRQHandler(void)
{
    switch (DL_TimerA_getPendingInterrupt(PID_INST))
    {
    case DL_TIMER_IIDX_ZERO:
        // pClass_Motor Motor_LB = Get_Motor_INST(LEFT_BACK);
        if (Get_Motor_INST(LEFT_BACK)->is_inited)
            Get_Motor_INST(LEFT_BACK)->TIM_PID_PeriodElapsedCallback(Get_Motor_INST(LEFT_BACK));

        // pClass_Motor Motor_RB = Get_Motor_INST(RIGHT_BACK);
        if (Get_Motor_INST(RIGHT_BACK)->is_inited)
            Get_Motor_INST(RIGHT_BACK)->TIM_PID_PeriodElapsedCallback(Get_Motor_INST(RIGHT_BACK));

        // pClass_Motor Motor_LF = Get_Motor_INST(LEFT_FRONT);
        if (Get_Motor_INST(LEFT_FRONT)->is_inited)
            Get_Motor_INST(LEFT_FRONT)->TIM_PID_PeriodElapsedCallback(Get_Motor_INST(LEFT_FRONT));

        // pClass_Motor Motor_RF = Get_Motor_INST(RIGHT_FRONT);
        if (Get_Motor_INST(RIGHT_FRONT)->is_inited)
            Get_Motor_INST(RIGHT_FRONT)->TIM_PID_PeriodElapsedCallback(Get_Motor_INST(RIGHT_FRONT));

        pClass_Car car = Get_Car_Handle();
        if (car->is_inited)
        {
            car->Kinematic_Forward(car);
            car->TIM_PID_PeriodElapsedCallback(car);
        }
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
        GET_ADCButton_INST()->Check_And_Trigger(GET_ADCButton_INST()); // 检查状态并触发回调
        break;

    default:
        break;
    }
}