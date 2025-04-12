#ifndef __BOARD__H__
#define __BOARD__H__

#include "ti_msp_dl_config.h"
#include "BSP/uart.h"
#include "BSP/delay.h"
#include "Base_Modules/reminder.h"

void softwareReset(uint32_t resetType);
void board_init(void);

#endif // __BOARD__H__