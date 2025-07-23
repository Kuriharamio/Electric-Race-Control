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

#ifdef USE_PID
// 电机PID定时器中断处理函数
void PID_TIMER_INST_IRQHandler(void)
{
    static uint8_t count = 0;
    switch (DL_TimerA_getPendingInterrupt(PID_TIMER_INST))
    {
    case DL_TIMER_IIDX_ZERO:
        count++;
#ifdef USE_CAR
        if (Get_Car_Handle()->is_inited)
        {
            Get_Car_Handle()->Judge_Mode(Get_Car_Handle());
        }

        Get_Car_Handle()->Kinematic_Inverse(Get_Car_Handle());

        if (count % PID_MOTOR_FACTOR == 0)
        {
            if (Get_Motor_INST(LEFT)->is_inited)
                Get_Motor_INST(LEFT)->Update_PID(Get_Motor_INST(LEFT));
            if (Get_Motor_INST(RIGHT)->is_inited)
                Get_Motor_INST(RIGHT)->Update_PID(Get_Motor_INST(RIGHT));
        }

        // if (count % PID_CAR_SPEED_FACTOR == 0)
        // {
        //     if (Get_Car_Handle()->is_inited){
        //         if (Get_Car_Handle()->Mode != FOLLOW_Circle && Get_Car_Handle()->Mode != MOTOR_TEST)
        //             Get_Car_Handle()->Update_Speed_PID(Get_Car_Handle());
        //     }
        // }

        if (count % PID_CAR_POS_FACTOR == 0)
        {
            if (Get_Car_Handle()->is_inited)
            {
                if (Get_Car_Handle()->Mode == POSISITON_L_Circle)
                {
                    Get_Car_Handle()->Update_Linear_Position_PID(Get_Car_Handle());
#ifdef USE_IMU_IN_ANGULAR_PID
                    Get_Car_Handle()->Update_Angle_Position_PID_IMU(Get_Car_Handle());
#else
                    Get_Car_Handle()->Update_Angle_Position_PID_WHEEL(Get_Car_Handle());
#endif
                }
                else if (Get_Car_Handle()->Mode == POSISITON_A_Circle)
                {
#ifdef USE_IMU_IN_ANGULAR_PID
                    Get_Car_Handle()->Update_Angle_Position_PID_IMU(Get_Car_Handle());
#else
                    Get_Car_Handle()->Update_Angle_Position_PID_WHEEL(Get_Car_Handle());
#endif
                }
                else if (Get_Car_Handle()->Mode == POSISITON_XY_Circle)
                {
                    Get_Car_Handle()->Update_XY_Position_PID(Get_Car_Handle());
                }
                else if (Get_Car_Handle()->Mode == TRAJECTORY_1 || Get_Car_Handle()->Mode == TRAJECTORY_2 || Get_Car_Handle()->Mode == TRAJECTORY_3)
                {
                    Get_Car_Handle()->Upadate_Controller(Get_Car_Handle());
                }
            }
        }

#ifdef USE_GRAY_SENSOR
        if (count % PID_CAR_FOLLOW_FACTOR == 0)
        {
            if (Get_Car_Handle()->is_inited)
            {
                if (Get_Car_Handle()->Mode == FOLLOW_Circle)
                    Get_Car_Handle()->Update_Follow_PID(Get_Car_Handle());
            }
        }
#endif

#endif

#ifdef USE_SERVO
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
#endif
        break;

    default:
        break;
    }
}
#endif

#ifdef USE_ENCODER
// 编码器测速定时器中断处理函数
void ENCODER_TIMER_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(ENCODER_TIMER_INST))
    {
    case DL_TIMER_IIDX_ZERO:
        // 计算速度
        if (Get_Motor_INST(RIGHT)->is_inited)
        {
            Get_Motor_INST(RIGHT)->Now_Speed = -((float)(Get_Motor_INST(RIGHT)->Total_Encoder_Tick) / (float)(Get_Motor_INST(RIGHT)->Encoder_Num_Per_Round) / ENCODER_TIMER_T * 2 * PI * Get_Motor_INST(RIGHT)->Radius);
            Get_Motor_INST(RIGHT)->Total_Encoder_Tick = 0;
        }
        if (Get_Motor_INST(LEFT)->is_inited)
        {
            Get_Motor_INST(LEFT)->Now_Speed = (float)(Get_Motor_INST(LEFT)->Total_Encoder_Tick) / (float)(Get_Motor_INST(LEFT)->Encoder_Num_Per_Round) / ENCODER_TIMER_T * 2 * PI * Get_Motor_INST(LEFT)->Radius;
            Get_Motor_INST(LEFT)->Total_Encoder_Tick = 0;
        }

        // 更新里程计
        if (Get_Car_Handle()->is_inited)
        {
            Get_Car_Handle()->Kinematic_Forward(Get_Car_Handle());
            Get_Car_Handle()->Update_Odom(Get_Car_Handle(), ENCODER_TIMER_T);
        }
        break;

    default:
        break;
    }
}
#endif

#if defined(USE_ADC_BUTTON) || defined(USE_GRAY_SENSOR)
// 读取数据定时器中断处理函数
void READ_TIMER_INST_IRQHandler(void)
{
    uint8_t cnt = 0;
    cnt++;
    switch (DL_TimerG_getPendingInterrupt(READ_TIMER_INST))
    {
    case DL_TIMER_IIDX_ZERO:
#ifdef USE_ADC_BUTTON
        if (cnt % ADC_BUTTON_TIMER_FACTOR == 0)
        {
            if (GET_ADCButton_INST()->is_inited)
            {
                GET_ADCButton_INST()->Check_And_Trigger(GET_ADCButton_INST()); // 获取当前ADC值
            }
        }
#endif

#ifdef USE_GRAY_SENSOR
        if (cnt % GRAY_SENSOR_TIMER_FACTOR == 0)
        {
            if (Get_Car_Handle()->Mode == FOLLOW_Circle)
            {
                Get_Car_Handle()->gray_scale_sensor->Update(Get_Car_Handle()->gray_scale_sensor);
                // printf("%d %d %d %d %d %d %d %d\n", Car->gray_scale_sensor->Analog_value[0], Car->gray_scale_sensor->Analog_value[1], Car->gray_scale_sensor->Analog_value[2], Car->gray_scale_sensor->Analog_value[3], Car->gray_scale_sensor->Analog_value[4], Car->gray_scale_sensor->Analog_value[5], Car->gray_scale_sensor->Analog_value[6], Car->gray_scale_sensor->Analog_value[7]);
            }
        }
#endif

        break;

    default:
        break;
    }
}
#endif