#ifndef __HMI_H_
#define __HMI_H_

#include "config.h"
#include "BSP/uart.h"
#include "Car/car.h"
#include "Base_Modules/reminder.h"

void HMI_Rx_Callback(pClass_UART this);

#endif