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
#ifdef USE_ENCODER
    /*******************************
                编码器中断
    *******************************/
    // 左
    pClass_Motor Motor_L = Get_Motor_INST(LEFT);
    if (Motor_L->is_inited)
    {
        uint32_t EN_LB = DL_GPIO_getEnabledInterruptStatus(Motor_L->ENCODER_PORT, Motor_L->ENCODER_A_PIN | Motor_L->ENCODER_B_PIN);

        if ((EN_LB & Motor_L->ENCODER_A_PIN) == Motor_L->ENCODER_A_PIN)
        {
            Motor_L->Encoder_Callback(Motor_L, 'A');
            DL_GPIO_clearInterruptStatus(Motor_L->ENCODER_PORT, Motor_L->ENCODER_A_PIN);
        }

        if ((EN_LB & Motor_L->ENCODER_B_PIN) == Motor_L->ENCODER_B_PIN)
        {
            Motor_L->Encoder_Callback(Motor_L, 'B');
            DL_GPIO_clearInterruptStatus(Motor_L->ENCODER_PORT, Motor_L->ENCODER_B_PIN);
        }
    }

    // 右
    pClass_Motor Motor_R = Get_Motor_INST(RIGHT);
    if (Motor_R->is_inited)
    {
        uint32_t EN_RB = DL_GPIO_getEnabledInterruptStatus(Motor_R->ENCODER_PORT, Motor_R->ENCODER_A_PIN | Motor_R->ENCODER_B_PIN);

        if ((EN_RB & Motor_R->ENCODER_A_PIN) == Motor_R->ENCODER_A_PIN)
        {
            Motor_R->Encoder_Callback(Motor_R, 'A');
            DL_GPIO_clearInterruptStatus(Motor_R->ENCODER_PORT, Motor_R->ENCODER_A_PIN);
        }

        if ((EN_RB & Motor_R->ENCODER_B_PIN) == Motor_R->ENCODER_B_PIN)
        {
            Motor_R->Encoder_Callback(Motor_R, 'B');
            DL_GPIO_clearInterruptStatus(Motor_R->ENCODER_PORT, Motor_R->ENCODER_B_PIN);
        }
    }
#endif
}
