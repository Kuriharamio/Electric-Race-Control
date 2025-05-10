#include "Base_Modules/reminder.h"

#ifdef USE_LED
REMINDER_STATE LED_STATE;
/**
 * @brief 控制LED灯的状态
 *
 * @param state
 */
void LED(REMINDER_STATE state)
{
    
    switch (state)
    {
    case ON:
        DL_GPIO_setPins(REMINDER_PORT, REMINDER_LED_PIN);
        LED_STATE = state;
        break;
    case OFF:
        DL_GPIO_clearPins(REMINDER_PORT, REMINDER_LED_PIN);
        LED_STATE = state;
        break;
    case TOGGLE:
        DL_GPIO_togglePins(REMINDER_PORT, REMINDER_LED_PIN);
        break;
    default:
        break;
    }
}
#endif

#ifdef USE_BUZZ
REMINDER_STATE BUZZ_STATE;
/**
 * @brief 控制BUZZ的状态
 *
 * @param state
 */
void BUZZ(REMINDER_STATE state)
{
    switch (state)
    {
    case ON:
        DL_GPIO_setPins(REMINDER_PORT, REMINDER_BUZZ_PIN);
        BUZZ_STATE = state;
        break;
    case OFF:
        DL_GPIO_clearPins(REMINDER_PORT, REMINDER_BUZZ_PIN);
        BUZZ_STATE = state;
        break;
    case TOGGLE:
        DL_GPIO_togglePins(REMINDER_PORT, REMINDER_BUZZ_PIN);
        break;
    case BEEP:
        DL_GPIO_setPins(REMINDER_PORT, REMINDER_BUZZ_PIN);
        delay_ms(200);
        DL_GPIO_clearPins(REMINDER_PORT, REMINDER_BUZZ_PIN);
    default:
        break;
    }
}

void software_pwm(int duty_cycle)
{
    // 假设系统时钟足够快，下面的延时需要根据实际硬件调整
    for (int i = 0; i < 100; i++)
    { // 控制PWM周期
        if (i < duty_cycle)
        {
            BUZZ(ON); // 高电平时间（占空比）
        }
        else
        {
            BUZZ(OFF); // 低电平时间
        }
        delay_us(10); // 微秒级延时，需要根据实际需求调整
    }
}

#endif