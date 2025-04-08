#ifndef __LED_H__
#define __LED_H__

#include "ti_msp_dl_config.h"

typedef enum{
    ON,
    OFF,
    TOGGLE,
}LED_STATE;

void LED(LED_STATE state);

#endif // __LED_H__