#include "Base_Modules/led.h"

/**
 * @brief 控制LED灯的状态
 * 
 * @param state 
 */
void LED(LED_STATE state)
{
    switch (state) {
        case ON:
            DL_GPIO_setPins(LED_GROUP_PORT, LED_GROUP_LED_PIN);
            break;
        case OFF:
            DL_GPIO_clearPins(LED_GROUP_PORT, LED_GROUP_LED_PIN);
            break;
        case TOGGLE:
            DL_GPIO_togglePins(LED_GROUP_PORT, LED_GROUP_LED_PIN);
            break;
        default:
            break;
    }
}