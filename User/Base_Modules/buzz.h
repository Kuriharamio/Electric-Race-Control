#ifndef __BUZZ_H__
#define __BUZZ_H__

#include "ti_msp_dl_config.h"
#include "BSP/delay.h"

typedef enum{
    On,
    Off,
    Toggle,
	Beep
}BUZZ_STATE;

void BUZZ(BUZZ_STATE state);

#endif // __BUZZ_H__