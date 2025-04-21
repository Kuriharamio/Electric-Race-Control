#ifndef __REMINDER_H__
#define __REMINDER_H__

#include "config.h"
#include "BSP/delay.h"

typedef enum
{
    ON,
    OFF,
    TOGGLE,
    BEEP,
} REMINDER_STATE;

#ifdef USE_LED
void LED(REMINDER_STATE state);
#endif

#ifdef USE_BUZZER
void BUZZER(REMINDER_STATE state);
#endif

#endif // __LED_H__