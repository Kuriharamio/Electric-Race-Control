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
#endif