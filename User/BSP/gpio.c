/**
 * @file gpio.c
 * @author KuriharaMio
 * @brief GPIO中断处理函数实现文件
 * @note 该文件包含了电机编码器中断处理函数的实现。
 * @version 0.1
 * @date 2025-04-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "gpio.h"

void GROUP1_IRQHandler(void)
{

    /*******************************
                编码器中断
    *******************************/
    // 左后轮
    pClass_Motor Motor_LB = Get_Motor_INST(LEFT_BACK);
    if (Motor_LB->is_inited)
    {
        uint32_t EN_LB = DL_GPIO_getEnabledInterruptStatus(Motor_LB->ENCODER_PORT, Motor_LB->ENCODER_A_PIN | Motor_LB->ENCODER_B_PIN);

        if ((EN_LB & Motor_LB->ENCODER_A_PIN) == Motor_LB->ENCODER_A_PIN)
        {
            Motor_LB->Encoder_Callback(Motor_LB, 'A');
            DL_GPIO_clearInterruptStatus(Motor_LB->ENCODER_PORT, Motor_LB->ENCODER_A_PIN);
        }

        if ((EN_LB & Motor_LB->ENCODER_B_PIN) == Motor_LB->ENCODER_B_PIN)
        {
            Motor_LB->Encoder_Callback(Motor_LB, 'B');
            DL_GPIO_clearInterruptStatus(Motor_LB->ENCODER_PORT, Motor_LB->ENCODER_B_PIN);
        }
    }

    // 右后轮
    pClass_Motor Motor_RB = Get_Motor_INST(RIGHT_BACK);
    if (Motor_RB->is_inited)
    {
        uint32_t EN_RB = DL_GPIO_getEnabledInterruptStatus(Motor_RB->ENCODER_PORT, Motor_RB->ENCODER_A_PIN | Motor_RB->ENCODER_B_PIN);

        if ((EN_RB & Motor_RB->ENCODER_A_PIN) == Motor_RB->ENCODER_A_PIN)
        {
            Motor_RB->Encoder_Callback(Motor_RB, 'A');
            DL_GPIO_clearInterruptStatus(Motor_RB->ENCODER_PORT, Motor_RB->ENCODER_A_PIN);
        }

        if ((EN_RB & Motor_RB->ENCODER_B_PIN) == Motor_RB->ENCODER_B_PIN)
        {
            Motor_RB->Encoder_Callback(Motor_RB, 'B');
            DL_GPIO_clearInterruptStatus(Motor_RB->ENCODER_PORT, Motor_RB->ENCODER_B_PIN);
        }
    }

    // 左前轮
    pClass_Motor Motor_LF = Get_Motor_INST(LEFT_FRONT);
    if (Motor_LF->is_inited)
    {
        uint32_t EN_LF = DL_GPIO_getEnabledInterruptStatus(Motor_LF->ENCODER_PORT, Motor_LF->ENCODER_A_PIN | Motor_LF->ENCODER_B_PIN);

        if ((EN_LF & Motor_LF->ENCODER_A_PIN) == Motor_LF->ENCODER_A_PIN)
        {
            Motor_LF->Encoder_Callback(Motor_LF, 'A');
            DL_GPIO_clearInterruptStatus(Motor_LF->ENCODER_PORT, Motor_LF->ENCODER_A_PIN);
        }

        if ((EN_LF & Motor_LF->ENCODER_B_PIN) == Motor_LF->ENCODER_B_PIN)
        {
            Motor_LF->Encoder_Callback(Motor_LF, 'B');
            DL_GPIO_clearInterruptStatus(Motor_LF->ENCODER_PORT, Motor_LF->ENCODER_B_PIN);
        }
    }

    // 右前轮
    pClass_Motor Motor_RF = Get_Motor_INST(RIGHT_FRONT);
    if (Motor_RF->is_inited)
    {
        uint32_t EN_RF = DL_GPIO_getEnabledInterruptStatus(Motor_RF->ENCODER_PORT, Motor_RF->ENCODER_A_PIN | Motor_RF->ENCODER_B_PIN);

        if ((EN_RF & Motor_RF->ENCODER_A_PIN) == Motor_RF->ENCODER_A_PIN)
        {
            Motor_RF->Encoder_Callback(Motor_RF, 'A');
            DL_GPIO_clearInterruptStatus(Motor_RF->ENCODER_PORT, Motor_RF->ENCODER_A_PIN);
        }

        if ((EN_RF & Motor_RF->ENCODER_B_PIN) == Motor_RF->ENCODER_B_PIN)
        {
            Motor_RF->Encoder_Callback(Motor_RF, 'B');
            DL_GPIO_clearInterruptStatus(Motor_RF->ENCODER_PORT, Motor_RF->ENCODER_B_PIN);
        }
    }
}
