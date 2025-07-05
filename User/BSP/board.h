#ifndef __BOARD__H__
#define __BOARD__H__

#include "config.h"
#include "BSP/uart.h"

void softwareReset(void);
void Enable_All_Interrupt(void);
int fputc(int ch, FILE *stream);

#endif // __BOARD__H__