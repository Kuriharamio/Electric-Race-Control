#include "Base_Modules/buzz.h"

/**
 * @brief 控制BUZZ的状态
 * 
 * @param state 
 */
void BUZZ(BUZZ_STATE state)
{
    switch (state) {
        case On:
            DL_GPIO_setPins(BUZZ_GROUP_PORT, BUZZ_GROUP_BUZZ_PIN);
            break;
        case Off:
            DL_GPIO_clearPins(BUZZ_GROUP_PORT, BUZZ_GROUP_BUZZ_PIN);
            break;
        case Toggle:
            DL_GPIO_togglePins(BUZZ_GROUP_PORT, BUZZ_GROUP_BUZZ_PIN);
            break;
		case Beep:
			DL_GPIO_setPins(BUZZ_GROUP_PORT, BUZZ_GROUP_BUZZ_PIN);
			delay_ms(1000);
			DL_GPIO_clearPins(BUZZ_GROUP_PORT, BUZZ_GROUP_BUZZ_PIN);
        default:
            break;
    }
}