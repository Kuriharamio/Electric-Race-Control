#ifndef __REMINDER_H__
#define __REMINDER_H__

#include "ti_msp_dl_config.h"
#include "BSP/delay.h"

typedef enum
{
    ON,
    OFF,
    TOGGLE,
    BEEP,
} REMINDER_STATE;

void LED(REMINDER_STATE state);
void BUZZ(REMINDER_STATE state);

#endif // __LED_H__