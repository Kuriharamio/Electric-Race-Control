#ifndef __REMINDER_H__
#define __REMINDER_H__

#include "config.h"
#include "BSP/delay.h"

typedef enum
{
    OFF,
    ON,
    TOGGLE,
    BEEP,
} REMINDER_STATE;

#ifdef USE_LED
void LED(REMINDER_STATE state);
extern REMINDER_STATE LED_STATE;
#endif

#ifdef USE_BUZZ
void BUZZ(REMINDER_STATE state);
extern REMINDER_STATE BUZZ_STATE;
#endif

#endif // __LED_H__