#ifndef __HMI_H_
#define __HMI_H_

#include "config.h"
#include "BSP/uart.h"

typedef enum
{
    Task_None,
    Task_1,
    Task_2,
    Task_3,
    Task_4,
    Task_5,
    Task_6,
} TASK;

extern TASK Now_Task;

void HMI_Rx_Callback(pClass_UART this);

#endif